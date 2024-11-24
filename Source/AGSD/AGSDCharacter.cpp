// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGSDCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h" // 라인 트레이스 함수 사용을 위해 필요
#include "Kismet/KismetMathLibrary.h" //대쉬 거리 계산을 위해 필요
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#include "InputAction.h"
#include "InputMappingContext.h"

#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

#include "WeaponDataTable.h"
#include "WeaponDataTableBeta.h"
#include "UObject/ConstructorHelpers.h"




DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAGSDCharacter

AAGSDCharacter::AAGSDCharacter()
{
	MaxHealth = 100;
	CurrentHealth = 100;
	Defense = 0;

	CharacterLevel = 1;        // 캐릭터 초기 레벨
	CurrentXP = 0;             // 초기 경험치
	XPToNextLevel = 8;       // 첫 번째 레벨 업까지 필요한 경험치

	BounsXPLevel = 1.0f;		//획득 경험치 증가

	Attack = 1.0f; //초기 공격력 수정해도 상관없음


	PrimaryActorTick.bCanEverTick = true; // Tick 함수 활성화

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CharacterMovementComponent = GetCharacterMovement();

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bDoCollisionTest = false; // 카메라 붐 충돌 체크 비활성화 (필요 시 활성화 가능)
	CameraBoom->bUsePawnControlRotation = true; // 캐릭터의 회전에 따라 카메라가 회전하지 않도록 설정

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	WeaponID = "1";

	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/DataTable/WeaponDataTableBeta.WeaponDataTableBeta'"));
	if (WeaponDataTableFinder.Succeeded())
	{
		WeaponDataTableRef = WeaponDataTableFinder.Object;
	}

	//무기 메쉬 컴포넌트 생성
	if (!WeaponMeshComponent) {
		WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>WeaponMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
		if (WeaponMesh.Succeeded())
		{
			WeaponMeshComponent->SetStaticMesh(WeaponMesh.Object);
		}
	}

	FireRate = 1.0f;
	Numberofprojectile = 1;
	SpreadAngle = 10.0f;
}

void AAGSDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// 마우스 입력 모드 설정: 게임 모드에서 마우스 입력을 활성화하고 커서 표시
		PlayerController->SetInputMode(FInputModeGameAndUI());  // FInputModeGameOnly()로 변경하여 순수 게임 모드로 설정 가능
		PlayerController->bShowMouseCursor = false;  // 마우스 커서 표시

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using Playable_Character."));

	if (WeaponDataTableRef)
	{
		FName RowName = FName(*WeaponID);  // FString을 FName으로 변환
		FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(RowName, TEXT("Weapon Lookup"));
		if (WeaponData)
		{
			FString WeaponName = WeaponData->Sname;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Name: %s"), *WeaponName));
		}
	}

	if (HealthBarUIBPClass) //체력바 생성
	{
		HealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarUIBPClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
			UpdateHealthBar();
			UpdateXPBar();
		}
	}

	if (WeaponMeshComponent)//무기 손에 붙히기
	{
		WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
	}
}

void AAGSDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 마우스 위치의 월드 좌표를 구함
	FVector MouseLocation, MouseDirection;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController && PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
	{
		// 캐릭터의 위치를 가져옴
		CharacterLocation = GetActorLocation();

		// 마우스 위치로부터 아래 방향으로 라인 트레이스를 쏘아 바닥과의 충돌 지점을 찾음
		FHitResult HitResult;
		FVector TraceStart = MouseLocation; // 마우스 위치를 시작점으로 설정
		FVector TraceEnd = TraceStart + (MouseDirection * 10000.0f); // 마우스 방향으로 먼 거리까지 라인 트레이스

		// 라인 트레이스 채널 설정 (바닥과 충돌하기 위한 채널 설정, 기본적으로 ECC_Visibility 사용)
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // 자기 자신과의 충돌 무시

		// 라인 트레이스를 수행하여 바닥과 충돌 지점 찾기
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

		if (bHit)
		{
			// 충돌된 지점의 Z 좌표를 사용하여 마우스 위치를 조정
			FVector AdjustedMouseLocation = HitResult.Location;

			// 캐릭터와 마우스 사이의 선을 디버그 선으로 그리기
			DrawDebugLine(GetWorld(), CharacterLocation, AdjustedMouseLocation, FColor::Green, false, -1.0f, 0, 2.0f);

			//라인트레이스 위치와 방향 저장
			TraceHitLocation = HitResult.Location;
			TraceHitDirection = (HitResult.Location - CharacterLocation).GetSafeNormal();
			// 로그로 충돌된 위치 출력 (디버깅 용도)
			//UE_LOG(LogTemp, Log, TEXT("Character Location: %s, Adjusted Mouse Location: %s"), *CharacterLocation.ToString(), *AdjustedMouseLocation.ToString());
			
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed to get the mouse position in world space."));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAGSDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAGSDCharacter::Move);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAGSDCharacter::Look);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AAGSDCharacter::Dash);
		
		// Firing
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AAGSDCharacter::StartFiring);
		PlayerInputComponent->BindAction("Fire", IE_Released, this, &AAGSDCharacter::StopFiring);
		PlayerInputComponent->BindAction("WeaponSwap", IE_Pressed, this, &AAGSDCharacter::WeaponSwap);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AAGSDCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAGSDCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AAGSDCharacter::Dash()
{
	if (CharacterMovementComponent && Controller)
	{
		// 현재 이동 방향으로 대시하도록 설정
		FVector CurrentVelocity = GetVelocity();
		FVector DashDirection = CurrentVelocity.GetSafeNormal();
		if (DashDirection.IsNearlyZero())
		{
			// 이동 중이 아닐 때는 캐릭터가 바라보는 방향으로 대시
			FRotator ControlRotation = Controller->GetControlRotation();
			DashDirection = UKismetMathLibrary::GetForwardVector(ControlRotation);
		}
		FVector DashVector = DashDirection * 1500.f; //대쉬 거리

		// 캐릭터를 대쉬 벡터만큼 이동
		LaunchCharacter(DashVector, true, true);
	}
}
//체력바 갱신 함수
void AAGSDCharacter::UpdateHealthBar()
{
	if (HealthBarWidget)
	{
		float HealthPercentage = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
		UProgressBar* HealthProgressBar = Cast<UProgressBar>(HealthBarWidget->GetWidgetFromName(TEXT("HealthBar")));
		if (HealthProgressBar)
		{
			HealthProgressBar->SetPercent(HealthPercentage);
		}
	}
}
void AAGSDCharacter::UpdateXPBar()
{
	if (HealthBarWidget)
	{
		float XPPercentage = static_cast<float>(CurrentXP) / static_cast<float>(XPToNextLevel);
		UProgressBar* XPProgressBar = Cast<UProgressBar>(HealthBarWidget->GetWidgetFromName(TEXT("XPBar")));
		if (XPProgressBar)
		{
			XPProgressBar->SetPercent(XPPercentage);
		}
	}
}

void AAGSDCharacter::AddXP(int32 XPAmount)
{
	
	CurrentXP += XPAmount; // 주어진 XP를 현재 경험치에 더함
	float XPPercentage = static_cast<float>(CurrentXP) / static_cast<float>(XPToNextLevel);
	UE_LOG(LogTemp, Log, TEXT("Increases XP: %d / %d"), CurrentXP, XPToNextLevel);
	// 캐릭터가 충분한 XP를 모았는지 확인하여 레벨 업 처리
	if (CurrentXP >= XPToNextLevel)
	{
		ShowLevelUpUI();
		LevelUp();
	}
	UpdateXPBar();
}

void AAGSDCharacter::LevelUp()
{
	// 캐릭터 레벨 증가
	CharacterLevel++;

	// 현재 XP를 리셋하고, 다음 레벨로 가기 위한 XP 임계값 증가
	CurrentXP -= XPToNextLevel;
	XPToNextLevel = XPToNextLevel * 1.5; // 예: 다음 레벨로 가기 위한 경험치 50% 증가

	// 선택 사항: 레벨 업을 알리거나 특별한 이벤트를 트리거할 수 있음
	UE_LOG(LogTemp, Log, TEXT("레벨 업! 새로운 레벨: %d"), CharacterLevel);

	// 레벨 업 시 능력치 증가나 새로운 능력 해제 등의 추가 동작을 구현할 수 있음
}

void AAGSDCharacter::ShowLevelUpUI()
{
	if (LevelUpUIBPClass)
	{
		LevelUpWidget = CreateWidget<UUserWidget>(GetWorld(), LevelUpUIBPClass);
		if (LevelUpWidget)
		{
			LevelUpWidget->AddToViewport();

			// 게임 정지
			PauseGameForLevelUp();
		}
	}
}
//레벨업시 옵션
void AAGSDCharacter::ApplyLevelUpOption(int32 OptionIndex)
{
	switch (OptionIndex)
	{
	case 0:
		// 현재 체력 모두 회복
		CurrentHealth = MaxHealth;
		UE_LOG(LogTemp, Log, TEXT("Option 1: Current Health fully restored. CurrentHealth = %d"), CurrentHealth);
		break;

	case 1:
		// 최대 체력 20 증가
		MaxHealth += 20;
		CurrentHealth += 20; // 최대 체력 증가 시 증가량 만큼 체력 회복
		UE_LOG(LogTemp, Log, TEXT("Option 2: Max Health increased. MaxHealth = %d"), MaxHealth);
		break;

	case 2:
		// 공격력 1 증가
		Attack += 1;
		UE_LOG(LogTemp, Log, TEXT("Option 3: Attack Power increased. AttackPower = %.1f"), Attack);
		break;
		
	case 3:
		//획득 경험치 50% 증가
		BounsXPLevel *= 1.5;
		UE_LOG(LogTemp, Log, TEXT("Option 4: XP add amount = %.1f"), BounsXPLevel);
		break;

	case 4:
		//방어력 10% 증가
		Defense += 10;
		UE_LOG(LogTemp, Log, TEXT("Option 5: Defense = %d"), Defense);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid OptionIndex: %d"), OptionIndex);
		break;
	}

	// 게임 재개
	ResumeGameAfterLevelUp();
}
//레벨업시 게임정지
void AAGSDCharacter::PauseGameForLevelUp()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetPause(true);
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}
//레벨업 선택 이후 게임 이어하기
void AAGSDCharacter::ResumeGameAfterLevelUp()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}

	// UI 제거
	if (LevelUpWidget)
	{
		LevelUpWidget->RemoveFromViewport();
		LevelUpWidget = nullptr;
	}
}

void AAGSDCharacter::Fire()
{
	//공격속도 제한
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle)) {
		return;
	}
	else {
		GetWorldTimerManager().SetTimer(FireRateTimerHandle, FTimerDelegate(), FireRate, false);
	}
	if (FireMontage && GetMesh())
	{
		PlayAnimMontage(FireMontage);
	}
	
}

void AAGSDCharacter::WeaponSwap() {
	if (WeaponID == "0") {
		WeaponID = "2";
	}
	else
	{
		WeaponID = "0";
	}
	FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
	FireRate = WeaponData->Frate;
	Numberofprojectile = WeaponData->Inumberofprojectile;
	ProjectileClass = WeaponData->WeaponProjectile;
	FireMontage = WeaponData->WeaponAnimationMontage;
	CurrentWeaponMesh = WeaponData->WeaponMesh;
	WeaponMeshComponent->SetStaticMesh(CurrentWeaponMesh);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Rate: %f"), FireRate));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Projectile: %i"), Numberofprojectile));
}
void AAGSDCharacter::StartFiring()
{
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle)) {

	}
	else
	{
		Fire();
	}
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAGSDCharacter::Fire, 0.1f, true);
}

void AAGSDCharacter::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AAGSDCharacter::CreateProjectile()
{
	// 발사
	if (ProjectileClass)
	{


		// 총구 위치
		MuzzleOffset.Set(50.0f, 0.0f, 0.0f);

		// 총구 방향
		FRotator MuzzleRotation = TraceHitDirection.Rotation();
		MuzzleRotation.Pitch = 0;
		MuzzleRotation.Roll = 0;

		// 총구위치 설정
		MuzzleLocation = CharacterLocation + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
		MuzzleLocation.Z = 90;
		//MuzzleLocation.Normalize();

		MuzzleRotation.Pitch += 0.0f;

		// 탄환 생성
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			//탄환숫자만큼 발사반복
			for (int i = 0; i < Numberofprojectile; i++) {

				// 총구에 탄환 생성.
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("RepeatFire"));
				//AProjectile_A* Projectile = World->SpawnActor<AProjectile_A>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				float AdjustedYaw = MuzzleRotation.Yaw + (i - (Numberofprojectile - 1) / 2.0f) * SpreadAngle;
				FRotator AdjustedRotation = FRotator(MuzzleRotation.Pitch, AdjustedYaw, MuzzleRotation.Roll);
				if (Projectile)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));
					FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
					Projectile->PlayerAttack = Attack;
					if (WeaponData)
					{
						//탄환에서 메쉬,마테리얼,데미지,속도,사거리 설정
						//Projectile->SetProjectileMeshAndMarterial(WeaponData->ProjectileMesh, WeaponData->ProjectileMaterial);
						//Projectile->SetProjectileSpeedDamageAndRange(WeaponData->Fspeedofprojectile, WeaponData->Fdamage, WeaponData->Frange);
						// 탄환 방향설정
						FVector LaunchDirection = AdjustedRotation.Vector();
						Projectile->FireInDirection(LaunchDirection);
					}

				}
			}
		}

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Not Found"));
	}
}

void AAGSDCharacter::Attacked(float Damage)
{
	CurrentHealth -= (int32)(Damage * ((100.0f - (float)Defense) / 100.0f));
	UE_LOG(LogTemp, Display, TEXT("HP : %d"), CurrentHealth);
    if (CurrentHealth <= 0)
    {
        OnDeath(); // 사망 처리
    }
    else
    {
        UpdateHealthBar();
    }
}

void AAGSDCharacter::OnDeath()
{
    // 게임 정지
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetPause(true);
        FInputModeUIOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }

    // Restart UI 생성
    if (RestartUIClass)
    {
        RestartWidget = CreateWidget<UUserWidget>(GetWorld(), RestartUIClass);
        if (RestartWidget)
        {
            RestartWidget->AddToViewport();
        }
    }

    // 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("Character has died."));
}

