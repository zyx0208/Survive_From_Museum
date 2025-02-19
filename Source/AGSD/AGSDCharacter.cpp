// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGSDCharacter.h"
#include "AGSD.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Particles/ParticleSystemComponent.h"//이펙트 만들기
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h" // 라인 트레이스 함수 사용을 위해 필요
#include "Kismet/KismetMathLibrary.h" //대쉬 거리 계산을 위해 필요
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "InputAction.h"
#include "InputMappingContext.h"

#include "XPOrb.h"
#include "Misc/StringUtility.h" // 문자열 유틸리티를 사용하기 위한 헤더 포함
#include "Engine/DataTable.h"
#include "AccessoryData.h" // 장신구 데이터 구조체 헤더
#include "AGSDCharacter_LevelUP.h"

#include "Blueprint/UserWidget.h" 
#include "Components/ProgressBar.h"

#include "DashCooldown_UI.h"

#include "WeaponDataTable.h"
#include "WeaponDataTableBeta.h"
#include "UObject/ConstructorHelpers.h"

#include "WeaponDrop.h"
#include "StorageBox.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"




DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAGSDCharacter

AAGSDCharacter::AAGSDCharacter()
{
	MaxHealth = 100;
	CurrentHealth = 100;
	Defense = 0;
    SpeedLevel = 500.f;

    Attack = 20.0f; //초기 공격력 수정해도 상관없음


	CharacterLevel = 1;        // 캐릭터 초기 레벨
	CurrentXP = 0;             // 초기 경험치
	XPToNextLevel = 8;       // 첫 번째 레벨 업까지 필요한 경험치
	BounsXPLevel = 1.0f;		//획득 경험치 증가
    XPRangeLevel = 200.0f;        //획득 자석 범위

    AttackSpeedLevel = 1.0f;
    AttackRangeLevel = 1.0f;


	PrimaryActorTick.bCanEverTick = true; // Tick 함수 활성화

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAGSDCharacter::OnOverlapBegin);
    GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AAGSDCharacter::OnComponentEndOverlap);

		
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
	GetCharacterMovement()->MaxWalkSpeed = SpeedLevel;
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

    // 자석 범위 콜리전 컴포넌트 생성
    MagnetSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetSphere"));
    MagnetSphere->SetupAttachment(RootComponent);
    MagnetSphere->SetSphereRadius(XPRangeLevel); // 기본 반경
    MagnetSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
    
	WeaponID = "1";

	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/AGSD/AGSD_Character/Weapon/WeaponDataTableBeta.WeaponDataTableBeta'"));
	if (WeaponDataTableFinder.Succeeded())
	{
		WeaponDataTableRef = WeaponDataTableFinder.Object;
	}

	//무기 메쉬 컴포넌트 생성
	if (!WeaponMeshComponent) {
		WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
        WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		static ConstructorHelpers::FObjectFinder<UStaticMesh>WeaponMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
		if (WeaponMesh.Succeeded())
		{
			WeaponMeshComponent->SetStaticMesh(WeaponMesh.Object);
		}
	}

	FireRate = 1.0f;
	Numberofprojectile = 1;
	SpreadAngle = 10.0f;

    // 기본 생성자에서 초기화 (nullptr로 설정)
    LevelUpHandler = nullptr;
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
    if (DashCooldownUIClass) //대쉬 아이콘 생성
    {
        DashCooldownWidget = CreateWidget<UDashCooldown_UI>(GetWorld(), DashCooldownUIClass);
        if (DashCooldownWidget)
        {
            DashCooldownWidget->AddToViewport();
            UpdateDashCooldownUI();
        }
    }

	if (WeaponMeshComponent)//무기 손에 붙히기
	{
		WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
	}

    if (WeaponArray.Num()>1) {
        WeaponID = FString::FromInt(WeaponArray[0]);
        WeaponTake();
    }
    // 현재 맵 이름 확인
    FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
    
    /*
    // GameTimer 생성 및 초기화
    if (CurrentMapName == "stage1")
    {
        InGameTimer = NewObject<UGameTimer>();
        if (InGameTimer)
        {
            InGameTimer->TimeSet(120.0f); // 60초 타이머 설정
            UE_LOG(LogTemp, Warning, TEXT("GameTimer successfully created."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create GameTimer."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameTimer not started. Current map: %s"), *CurrentMapName);
    }
    */
    AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance) {
        UE_LOG(LogTemp, Warning, TEXT("No Anim"));
    }

    // 레벨업 처리 핸들러 생성
    LevelUpHandler = NewObject<AAGSDCharacter_LevelUP>(this);
}

void AAGSDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 마우스 위치의 월드 좌표를 구함
	FVector MouseLocation, MouseDirection;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

    // 자석 기능 처리
    TArray<AActor*> OverlappingActors;
    MagnetSphere->GetOverlappingActors(OverlappingActors);

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
    /*
    if (InGameTimer && InGameTimer->TimeEnd)
    {
        Clear();
    }
    */
    for (int32 i = MagnetField.Num() - 1; i >= 0; i--) // 역순으로 루프
    {
        AXPOrb* XPOrb = MagnetField[i];
        if (!XPOrb)
        {
            MagnetField.RemoveAt(i); // 배열에서 제거
            continue;
        }

        FVector Direction = GetActorLocation() - XPOrb->GetActorLocation();
        float Distance = Direction.Size();
        Direction.Normalize();

        const float AbsorbDistance = 100.0f; // 흡수 거리
        if (Distance <= AbsorbDistance)
        {
            // XP 흡수 처리
            AddXP(XPOrb->XPValue);

            // XPOrb 제거
            XPOrb->Destroy();

            // MagnetField에서 제거
            MagnetField.RemoveAt(i);
        }
        else
        {
            // 끌어당기기
            MagnetStrength += MagnetAcceleration * DeltaTime;
            FVector Force = Direction * MagnetStrength * DeltaTime;
            XPOrb->AddActorWorldOffset(Force, true);
        }
    }

    //충돌 디버깅
    //ECollisionResponse CurrentResponse = GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn);
    //UE_LOG(LogTemp, Warning, TEXT("Current Collision Response to ECC_Pawn: %d"), (int32)CurrentResponse);

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

        // InteractionAction
        EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &AAGSDCharacter::Interaction);
		
		// Firing
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AAGSDCharacter::StartFiring);
		PlayerInputComponent->BindAction("Fire", IE_Released, this, &AAGSDCharacter::StopFiring);
		PlayerInputComponent->BindAction("WeaponSwap", IE_Pressed, this, &AAGSDCharacter::WeaponSwap);

        // 드랍된 무기와 현재 무기 교체
        //PlayerInputComponent->BindAction("GetWeapon", IE_Pressed, this, &AAGSDCharacter::GetWeapon);
        // 무기교체
        //PlayerInputComponent->BindAction("WeaponExchange", IE_Pressed, this, &AAGSDCharacter::);
        //디버그용 버튼
        PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &AAGSDCharacter::Debug);
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
    if (!bCanDash) // 쿨타임 중이면 실행 X
    {
        return;
    }

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

        // 몬스터 충돌 무시 설정
        IgnoreMonsterCollision();
		       

        // 쿨타임 및 무적 설정
        bCanDash = false;           // 쿨타임 시작
        bIsInvincible = true;       // 무적 상태 설정
        DashCooldownTimer = DashCooldown; // UI 업데이트를 위한 타이머 설정

        // 대시 종료 후 충돌 복구 타이머 추가
        GetWorldTimerManager().SetTimer(
            CollisionRestoreTimerHandle, this, &AAGSDCharacter::RestoreMonsterCollision, 1.0f, false);

        // 쿨타임 타이머 시작
        GetWorldTimerManager().SetTimer(
            DashCooldownTimerHandle, this, &AAGSDCharacter::ResetDashCooldown, DashCooldown, false);

        // 무적 해제 타이머 시작
        GetWorldTimerManager().SetTimer(
            InvincibilityTimerHandle, this, &AAGSDCharacter::ResetInvincibility, 1.0f, false);

        // UI 업데이트 타이머 시작
        GetWorldTimerManager().SetTimer(
            DashCooldownUpdateTimerHandle, this, &AAGSDCharacter::UpdateDashCooldownUI, 0.1f, true);
	}
}
void AAGSDCharacter::ResetDashCooldown()
{
    bCanDash = true; // 대쉬 가능 상태로 변경
    GetWorldTimerManager().ClearTimer(DashCooldownUpdateTimerHandle); // UI 업데이트 타이머 정지
    UpdateDashCooldownUI(); // 마지막 업데이트 호출 (쿨타임 완료)
}
void AAGSDCharacter::IgnoreMonsterCollision()
{

    // 기존 충돌 상태 저장
    OriginalMonsterCollision = GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Enemy);

    // 몬스터(Enemy)와 충돌 무시
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);

    // `CharacterMovementComponent`도 충돌 무시 추가
    GetCharacterMovement()->SetAvoidanceEnabled(false);

    // Mesh도 충돌 무시 (추가)
    GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);

    // 충돌 자체를 비활성화하여 완전히 무시 (추가)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // 변경 적용을 강제 업데이트
    GetCapsuleComponent()->UpdateOverlaps();

}


void AAGSDCharacter::RestoreMonsterCollision()
{
    // 기존 충돌 상태로 복원
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Enemy, OriginalMonsterCollision);

    // `CharacterMovementComponent`도 원래대로 복원
    GetCharacterMovement()->SetAvoidanceEnabled(true);

    // Mesh도 원래 상태 복원
    GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Block);

    // 물리 충돌 다시 활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 변경 적용을 강제 업데이트
    GetCapsuleComponent()->UpdateOverlaps();
}

void AAGSDCharacter::ResetInvincibility()
{
    bIsInvincible = false; // 무적 해제
}
//대쉬 쿨타임 갱신 함수
void AAGSDCharacter::UpdateDashCooldownUI()
{
    if (!DashCooldownWidget)
    {
        //UE_LOG(LogTemp, Error, TEXT("DashCooldownWidget is NULL! Check if UI was created in BeginPlay."));
        return;
    }
    if (DashCooldownTimer > 0)
    {
        DashCooldownTimer -= 0.1f;
        float CooldownPercentage = DashCooldownTimer / DashCooldown;

       // UE_LOG(LogTemp, Log, TEXT("Cooldown Percentage: %f"), CooldownPercentage);

        DashCooldownWidget->UpdateDashCooldown(CooldownPercentage);
    }
}

void AAGSDCharacter::Interaction()
{
    if (OverlapDropWeapon) 
    {
        ShowWeaponExchangeUI();
    }
    else if (OverlapBox)
    {
        ShowStorageBoxUI();
        //UE_LOG(LogTemp, Log, TEXT("Box here"));
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
    XPAmount *= BounsXPLevel;
	CurrentXP += XPAmount ; // 주어진 XP를 현재 경험치에 더함
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

void AAGSDCharacter::ApplyLevelUpOption(AAGSDCharacter* Character, const FAccessoryData& SelectedAccessory)
{
    if (LevelUpHandler)
    {
        LevelUpHandler->ApplyLevelUpOption(this, SelectedAccessory);
    }
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
    if (IsValid(LevelUpWidget))
    {
        LevelUpWidget->RemoveFromViewport();
        LevelUpWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("LevelUpWidget removed successfully."));
    }
    else if (IsValid(StorageBoxWidget))
    {
        StorageBoxWidget->RemoveFromViewport();
        DashCooldownWidget->AddToViewport();
        HealthBarWidget->AddToViewport();
        StorageBoxWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("StorageBoxWidget removed successfully."));
    }

    else if (IsValid(WeaponExchangeWidget)) {
        WeaponExchangeWidget->RemoveFromViewport();
        WeaponExchangeWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("WeaponExchangeWidget removed successfully."));
    }

    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelUpWidget is already null or invalid."));
    }
}
void AAGSDCharacter::AddToMagnetField(AXPOrb* XPOrb)
{
    if (XPOrb && !MagnetField.Contains(XPOrb))
    {
        MagnetField.Add(XPOrb);
    }
}

void AAGSDCharacter::RemoveFromMagnetField(AXPOrb* XPOrb)
{
    if (XPOrb)
    {
        MagnetField.Remove(XPOrb);
    }
}

void AAGSDCharacter::Fire()
{
	//공격속도 제한
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle)) {
		return;
	}
	else {
		GetWorldTimerManager().SetTimer(FireRateTimerHandle, FTimerDelegate(), FireRate/AttackSpeedLevel, false);
	}
	if (FireMontage && GetMesh())
	{
        if (!AnimInstance) {
            UE_LOG(LogTemp, Log, TEXT("NO Anim"));
            return;
        }
        PlayFireMontage(FireMontage, RepeatFire);
	}
	
}

void AAGSDCharacter::PlayFireMontage(UAnimMontage* Montage, int RepeatNumber)
{
    if (!Montage || RepeatNumber <= 0) return;

    // 변수 초기화
    CurrentMontage = Montage;
    RepeatCount = RepeatNumber;

    // Delegate 설정
    FOnMontageEnded MontageEndedDelegate;
    MontageEndedDelegate.BindUObject(this, &AAGSDCharacter::OnMontageEnded);

    // 몽타주 재생 및 Delegate 바인딩
    AnimInstance->Montage_Play(Montage,1.0f*RepeatFire*AttackSpeedLevel);
    AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
}

void AAGSDCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != CurrentMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("Montage was interrupted or not the expected montage."));
        return;
    }

    if (CurrentCount < RepeatCount)
    {
        CurrentCount++;
        UE_LOG(LogTemp, Display, TEXT("%d 번째 발사"), CurrentCount);
        PlayFireMontage(Montage, RepeatCount);
        /* 반복 재생
        CurrentCount++;
        AnimInstance->Montage_Play(Montage, 1.0f * RepeatFire * AttackSpeedLevel);
        UE_LOG(LogTemp, Display, TEXT("%d 번째 발사"), CurrentCount);
        OnMontageEnded(Montage, false);*/
    }
    else
    {
        // 반복 종료
        CurrentCount = 0;
        UE_LOG(LogTemp, Display, TEXT("Montage playback completed %d times."), RepeatCount);
    }
}

void AAGSDCharacter::ShowWeaponExchangeUI()
{
    if (WeaponExchangeWidgetClass) {
        UE_LOG(LogTemp, Display, TEXT("ShowWeaponExchange"));
        WeaponExchangeWidget = CreateWidget<UUserWidget>(GetWorld(), WeaponExchangeWidgetClass);
        if (WeaponExchangeWidget) {
            WeaponExchangeWidget->AddToViewport();
            PauseGameForLevelUp();
        }
    }
}

void AAGSDCharacter::WeaponSwap() {
	if (CurrentWeaponSlot) {
        WeaponID = FString::FromInt(WeaponArray[1]);
        WeaponTake();
        CurrentWeaponSlot = !CurrentWeaponSlot;
	}
	else
	{
        WeaponID = FString::FromInt(WeaponArray[0]);
        WeaponTake();
        CurrentWeaponSlot = !CurrentWeaponSlot;
	}
    /*
	FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
	FireRate = WeaponData->Frate;
	Numberofprojectile = WeaponData->Inumberofprojectile;
	ProjectileClass = WeaponData->WeaponProjectile;
	FireMontage = WeaponData->WeaponAnimationMontage;
	CurrentWeaponMesh = WeaponData->WeaponMesh;
	WeaponMeshComponent->SetStaticMesh(CurrentWeaponMesh);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Rate: %f"), FireRate));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Projectile: %i"), Numberofprojectile));*/
}
void AAGSDCharacter::SpawnParticle(FVector Location, FRotator Rotation)
{
    if(WeaponParticle) {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponParticle, Location, Rotation, true);
    }
}
void AAGSDCharacter::SpawnSubWeapon(TSubclassOf<ASubWeapon> SubWeapon)
{
    if (SubWeapon == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Not Found SubWeapon")));
        return;
    }

    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();

    ASubWeapon* NewSubWeapon = GetWorld()->SpawnActor<ASubWeapon>(SubWeapon, SpawnLocation, SpawnRotation);
    if (NewSubWeapon)
    {
        USkeletalMeshComponent* MeshComp = GetMesh();
        if (MeshComp) {
            NewSubWeapon->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("SubWeaponSocket")));
            NewSubWeapon->PlayerAttack = Attack;
            SubWeapons.Add(NewSubWeapon);
        }
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Not Found SubWeapon")));
    }
}
void AAGSDCharacter::WeaponTake()
{
    FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
    FireRate = WeaponData->Frate;
    Numberofprojectile = WeaponData->Inumberofprojectile;
    ProjectileClass = WeaponData->WeaponProjectile;
    FireMontage = WeaponData->WeaponAnimationMontage;
    CurrentWeaponMesh = WeaponData->WeaponMesh;
    RepeatFire = WeaponData->RepeatFire;
    WeaponMeshComponent->SetStaticMesh(CurrentWeaponMesh);
	WeaponType = WeaponData->WeaponType;
    if (WeaponData->WeaponParticle != nullptr) {
        WeaponParticle = WeaponData->WeaponParticle;
    }
    else {
        WeaponParticle = nullptr;
    }
}
void AAGSDCharacter::Debug()
{

    if (GetMesh()->DoesSocketExist(FName(TEXT("SubWeaponSocket"))))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Socket exists!"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Socket does not exist!"));
    }
    SpawnSubWeapon(SubWeaponSelector);
}
void AAGSDCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("OverLap")));
        // 충돌한 오브젝트가 WeaponDrop임을 확인
        if (OtherActor && OtherActor->IsA<AWeaponDrop>())
        {
            OverlapDropWeapon = true;
            // ACharacter로 캐스팅
            OverlapWeaponDrop = Cast<AWeaponDrop>(OtherActor);
            if (OverlapWeaponDrop)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Overlap Weapon: %s"), *OverlapWeaponDrop->WeaponID));
                OverlapID = *OverlapWeaponDrop->WeaponID;
            }
        }
        // 충돌한 오브젝트가 Box임을 확인
        if (OtherActor && OtherActor->IsA<AStorageBox>())
        {
            OverlapBox = true;
            if (OverlapBox)
            {
                UE_LOG(LogTemp, Log, TEXT("OverLap Box"));
            }
        }
    }
}

void AAGSDCharacter::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
    {
        // 충돌한 오브젝트가 WeaponDrop임을 확인
        if (OtherActor && OtherActor->IsA<AWeaponDrop>())
        {
            OverlapDropWeapon = false;
        }
        // 충돌한 오브젝트가 Box임을 확인
        if (OtherActor && OtherActor->IsA<AStorageBox>())
        {
            OverlapBox = false;
        }
    }
}

bool AAGSDCharacter::IsOverlappingActor(const AActor* Other) const
{
    return false;
}



void AAGSDCharacter::GetWeapon()
{
    if (OverlapDropWeapon) {
        if (CurrentWeaponSlot) {
            int32 MyInt = FCString::Atoi(*OverlapID);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("MyInt: %d"),MyInt));
            WeaponArray[0] = MyInt;
            WeaponID = FString::FromInt(WeaponArray[0]);
        }
        else {
            int32 MyInt = FCString::Atoi(*OverlapID);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("MyInt: %d"), MyInt));
            WeaponArray[1] = MyInt;
            WeaponID = FString::FromInt(WeaponArray[1]);
        }
        OverlapWeaponDrop->DestroySelf();
        WeaponTake();
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Not Ovelap WeaponDrop")));
    }
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
    if (!WeaponType) {
        if (ProjectileClass)
        {


            // 총구 위치
            MuzzleOffset.Set(0.1f, 0.0f, 0.0f);

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
                        Projectile->PlayerRange = AttackRangeLevel;
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
                    //파티클 생성
                    if (WeaponParticle) {
                        SpawnParticle(MuzzleLocation, AdjustedRotation);
                    }
                }

            }
        }
    }
    else {
        if (ProjectileClass)
        {


            // 총구 위치
            MuzzleOffset.Set(0.1f, 0.0f, 0.0f);

            // 총구 방향
            FRotator MuzzleRotation = TraceHitDirection.Rotation();
            MuzzleRotation.Pitch = 0;
            MuzzleRotation.Roll = 0;

            // 총구위치 설정
            MuzzleLocation = CharacterLocation + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
            MuzzleLocation.Z = 90;

            MuzzleRotation.Pitch += 0.0f;

            // 탄환 생성
            UWorld* World = GetWorld();
            if (World)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();
                float LeftRight = 15.0f;
                //탄환숫자만큼 발사반복
                for (int i = 0; i < Numberofprojectile; i++) {
                    if (i % 2 == 1) {
                        MuzzleOffset.Y = -1 * LeftRight;
                        
                    }
                    else if (i!=0 && i % 2 == 0) {
                        MuzzleOffset.Y = 1 * LeftRight;
                        LeftRight += 15.0f;
                    }
                    MuzzleLocation = CharacterLocation + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
                    AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                    if (Projectile)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));
                        FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
                        Projectile->PlayerAttack = Attack;
                        Projectile->PlayerRange = AttackRangeLevel;
                        if (WeaponData)
                        {
                            // 탄환 방향설정
                            FVector LaunchDirection = MuzzleRotation.Vector();
                            Projectile->FireInDirection(LaunchDirection);
                        }

                    }
                    //파티클 생성
                    if (WeaponParticle) {
                        SpawnParticle(MuzzleLocation, MuzzleRotation);
                    }
                }

            }
        }
    }
}

void AAGSDCharacter::Attacked(float Damage)
{
    if (bIsInvincible) // 무적 상태에서는 피해 무시
    {
        return;
    }
    //체력계산
    if ((int32)(Damage * ((100.0f - (float)Defense) / 100.0f)) < 0) //받는 데미지가 음수가 되버리면 0으로 계산
    {
        CurrentHealth -= 0;
    }
    else
    {
        CurrentHealth -= (int32)(Damage * ((100.0f - (float)Defense) / 100.0f));
    }
	UE_LOG(LogTemp, Display, TEXT("HP : %d"), CurrentHealth);
    if (CurrentHealth <= 0)
    {
        UpdateHealthBar();
        OnDeath(); // 사망 처리
    }
    else
    {
        UpdateHealthBar();
    }
}

void AAGSDCharacter::SetHP(float value)
{
    //혹시라도 "체력 회복량 증가" 등의 아이템 추가가 된다면 여기에 수식 추가
    //value *= ??;
    
    //체력계산
    if (CurrentHealth + (int32)(value) <= MaxHealth)
    {
        CurrentHealth += (int32)(value);
    }
    else
    {
        CurrentHealth = MaxHealth;
    }
    UE_LOG(LogTemp, Display, TEXT("HP : %d"), CurrentHealth);
    UpdateHealthBar();
}

void AAGSDCharacter::MagneticEffect(float time)
{
    UE_LOG(LogTemp, Display, TEXT("MagneticEffect Start."));
    MagnetSphere->SetSphereRadius(20000.0f); //자석 범위 증가
    //자석 효과 타이머 시작
    GetWorldTimerManager().SetTimer(
        MagneticEffectTimerHandle, this, &AAGSDCharacter::MagneticEffectOff, time, false);
}

void AAGSDCharacter::MagneticEffectOff()
{
    UE_LOG(LogTemp, Display, TEXT("MagneticEffect End."));
    MagnetSphere->SetSphereRadius(200.0f); //자석 범위 원래대로
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

void AAGSDCharacter::Clear()
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
    UE_LOG(LogTemp, Warning, TEXT("Level Clear"));
}

void AAGSDCharacter::ShowStorageBoxUI()
{
    if (StorageBoxWidgetClass)
    {
        StorageBoxWidget = CreateWidget<UUserWidget>(GetWorld(), StorageBoxWidgetClass);
        if (StorageBoxWidget)
        {
            StorageBoxWidget->AddToViewport();
            DashCooldownWidget->RemoveFromViewport();
            HealthBarWidget->RemoveFromViewport();
            PauseGameForLevelUp();
        }
    }
}

