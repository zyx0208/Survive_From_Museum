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
#include "AGSDGameInstance.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundWave.h"

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
#include "Components/TextBlock.h"

#include "DashCooldown_UI.h"
#include "HPDashUIActor.h"
#include "WeaponDataTable.h"
#include "WeaponDataTableBeta.h"
#include "UObject/ConstructorHelpers.h"

#include "WeaponDrop.h"
#include "StorageBox.h"
#include "DrawingBook.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "NPC1Class.h"




DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAGSDCharacter

AAGSDCharacter::AAGSDCharacter()
{
	MaxHealth = 20;
	CurrentHealth = 20;
	Defense = 100.0f;
    SpeedLevel = 500.f;

    Attack = 1.0f; //초기 공격력 수정해도 상관없음


	CharacterLevel = 1;        // 캐릭터 초기 레벨
	CurrentXP = 0;             // 초기 경험치
	XPToNextLevel = 8;       // 첫 번째 레벨 업까지 필요한 경험치
	BounsXPLevel = 1.0f;		//획득 경험치 증가
    XPRangeLevel = 200.0f;        //획득 자석 범위

    AttackSpeedLevel = 1.0f;
    AttackRangeLevel = 1.0f;        //3단계로 진행
    IsWalking = false;
    DashCooldown = 3.0f;

	PrimaryActorTick.bCanEverTick = true; // Tick 함수 활성화

    InitializeAccessoryList();  //획득한 악세서리 초기화

    IsResurrection = false;     //부활
    bIs_Attacked_Invincible = false;    //피격시 무적

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
	CameraBoom->bDoCollisionTest = true; // 카메라 붐 충돌 체크 비활성화 (필요 시 활성화 가능)
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

    WalkingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WalkingAudioComponent"));
    WalkingAudioComponent->SetupAttachment(RootComponent);

    DashAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DashAudioComponent"));
    DashAudioComponent->SetupAttachment(RootComponent);

    SubWeaponAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SubWeaponAttachPoint"));
    SubWeaponAttachPoint->SetupAttachment(RootComponent);

    XPAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("XPAudioComponent"));
    XPAudioComponent->SetupAttachment(RootComponent);

	FireRate = 1.0f;
	Numberofprojectile = 1;
	SpreadAngle = 25.0f;

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
		//PlayerController->SetInputMode(FInputModeGameAndUI());  // FInputModeGameOnly()로 변경하여 순수 게임 모드로 설정 가능
		PlayerController->bShowMouseCursor = true;  // 마우스 커서 표시

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using Playable_Character."));

    UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());

    if (GI && GI->WeaponArray[1] && GI->WeaponArray[0]) {
        WeaponArray[0] = GI->WeaponArray[0];
        WeaponArray[1] = GI->WeaponArray[1];
    }

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

	if (HealthBarUIBPClass && StatPanelWidgetClass) //체력바 생성
	{
		HealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarUIBPClass);
        StatPanelWidget = CreateWidget<UStatPanelWidget>(GetWorld(), StatPanelWidgetClass);
        if (StatPanelWidget)
        {
            StatPanelWidget->AddToViewport();
            StatPanelWidget->SetupStatBars(this); // 최초 초기화
        }
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
			UpdateHealthBar();
			UpdateXPBar();
            UpdateStat();
		}
	}
    if (DashCooldownUIClass) //대쉬 아이콘 생성
    {
        DashCooldownWidget = CreateWidget<UDashCooldown_UI>(GetWorld(), DashCooldownUIClass);
        if (DashCooldownWidget)
        {
            DashCooldownWidget->AddToViewport();
            UpdateDashCooldownUI();
            UpdateSwapWeaponIcon();
        }
    }
    if (DamageTextWidgetClass)
    {
        DamageTextWidget = CreateWidget<UUserWidget>(GetWorld(), DamageTextWidgetClass);
        if (DamageTextWidget) {
            DamageTextWidget->AddToViewport();
            if (GI) {
                GI->DamageUIInstance = DamageTextWidget;
            }
        }
    }

	if (WeaponMeshComponent)//무기 손에 붙히기
	{
		WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
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
    /*
    if (AnimInstance) {
        UE_LOG(LogTemp, Warning, TEXT("No Anim"));
    }
    */
    // 레벨업 처리 핸들러 생성
    LevelUpHandler = NewObject<AAGSDCharacter_LevelUP>(this);

    //마우스 포인터
    if (MouseIndicatorClass)
    {
        MouseIndicator = GetWorld()->SpawnActor<AMouseIndicatorActor>(MouseIndicatorClass, FVector::ZeroVector, FRotator::ZeroRotator);
    }

    if (HPDashUIActorClass)
    {
        HPDashUIActorInstance = GetWorld()->SpawnActor<AHPDashUIActor>(HPDashUIActorClass, GetActorLocation(), FRotator::ZeroRotator);
        if (HPDashUIActorInstance)
        {
            HPDashUIActorInstance->Initialize(this); // 캐릭터를 따라다님

            // 위젯 연결
            if (HPandDashWidgetClass)
            {
                UWidgetComponent* WidgetComp = HPDashUIActorInstance->FindComponentByClass<UWidgetComponent>();
                if (WidgetComp)
                {
                    UUserWidget* UIWidget = CreateWidget<UUserWidget>(GetWorld(), HPandDashWidgetClass);
                    WidgetComp->SetWidget(UIWidget);
                    UpdateHealthBar();
                    UpdateDashCooldownUI();
                }
            }
        }
    }
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

    //걷는 소리 출력 처리
    PlayWalkingSound();

    //카메라 위치 디버깅
    UpdateCameraObstruction();

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
			//DrawDebugLine(GetWorld(), CharacterLocation, AdjustedMouseLocation, FColor::Green, false, -1.0f, 0, 2.0f);
            /*if (MouseIndicator)
            {
                FVector DecalLocation = AdjustedMouseLocation + FVector(0.f, 0.f, 5.f);
                MouseIndicator->SetActorLocation(DecalLocation);
            }*/
			//라인트레이스 위치와 방향 저장
			TraceHitLocation = HitResult.Location;
			TraceHitDirection = (HitResult.Location - CharacterLocation).GetSafeNormal();
            FRotator NewActorRotation = GetActorRotation();
            NewActorRotation.Yaw = TraceHitDirection.Rotation().Yaw;
            SetActorRotation(NewActorRotation);
            //PlayerController->SetControlRotation(NewActorRotation);
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

            // 대쉬 사운드 재생
            if (XPSound)
            {
                // AudioComponent에 사운드를 설정하고 재생
                XPAudioComponent->SetSound(XPSound);
                XPAudioComponent->Play();
            }

            // XPOrb 제거
            XPOrb->Destroy();
            //MagnetStrength = 250.0f; //초기 자석 값
            // MagnetField에서 제거
            MagnetField.RemoveAt(i);
        }
        else
        {
            // 끌어당기기
            XPOrb->MagnetStrength += MagnetAcceleration * DeltaTime;
            XPOrb->force = Direction * XPOrb->MagnetStrength * DeltaTime;
            XPOrb->AddActorWorldOffset(XPOrb->force, true);
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
    Super::SetupPlayerInputComponent(PlayerInputComponent);
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
        //일시정지 버튼 
        PlayerInputComponent->BindAction("GamePauseButton", IE_Pressed, this, &AAGSDCharacter::HandleEscape);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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
void AAGSDCharacter::UpdateCameraObstruction()
{
    if (!CameraBoom || !FollowCamera) return;

    FVector DesiredCameraLocation = FollowCamera->GetComponentLocation();
    FVector TempCharacterLocation = GetActorLocation() + FVector(0.0f, 0.0f, 900.0f);
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(CameraObstruction), true, this);

    FHitResult HitResult;
    float DeltaTime = GetWorld()->GetDeltaSeconds();
    float InterpSpeed = 10.0f; // 보간 속도
    bool bBlocked = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TempCharacterLocation,
        DesiredCameraLocation,
        ECC_Visibility,
        TraceParams
    );
    //디버그 라인트레이스
    DrawDebugLine(GetWorld(), TempCharacterLocation, DesiredCameraLocation, FColor::Blue, false, 0.1f, 0, 1.0f);

    if (bBlocked)
    {
        float OffsetDistance = 10.0f;
        FVector AdjustedLocation = HitResult.ImpactPoint + HitResult.ImpactNormal * OffsetDistance;

        // 선택사항: 기존 카메라 위치와 새 위치를 부드럽게 보간
        FVector NewCameraLocation = FMath::VInterpTo(FollowCamera->GetComponentLocation(), AdjustedLocation, GetWorld()->GetDeltaSeconds(), InterpSpeed);

        FollowCamera->SetWorldLocation(NewCameraLocation);
    }
    else
    {
        FVector UnobstructedLocation = CameraBoom->GetSocketLocation(CameraBoom->SocketName);
        FVector NewCameraLocation = FMath::VInterpTo(FollowCamera->GetComponentLocation(), UnobstructedLocation, DeltaTime, InterpSpeed);
        FollowCamera->SetWorldLocation(NewCameraLocation);
    }
}
//일시정시 관리 함수
void AAGSDCharacter::HandleEscape()
{
    UE_LOG(LogTemp, Warning, TEXT("ESC pressed, Game Pause"));
}

void AAGSDCharacter::Dash()
{
    if (!bCanDash) // 쿨타임 중이면 실행 X
    {
        return;
    }

	if (CharacterMovementComponent && Controller)
	{
        // 대쉬 사운드 재생
        if (DashSound)
        {
            // AudioComponent에 사운드를 설정하고 재생
            DashAudioComponent->SetSound(DashSound);
            DashAudioComponent->Play();
        }
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
void AAGSDCharacter::ResetInvincibility2()
{
    bIsInvincible = false; // 무적 해제(피격 무적용)
    Invincibility_Cooldown = false;
    GetWorldTimerManager().SetTimer(
        InvincibilityTimerHandle2, this, &AAGSDCharacter::ResetUnInvincibility, 60.0f, false);
}
void AAGSDCharacter::ResetUnInvincibility()
{
    Invincibility_Cooldown = true;  // 피격 무적 쿨타임 확인용
}
//대쉬 쿨타임 갱신 함수
void AAGSDCharacter::UpdateDashCooldownUI()
{
    /*if (!DashCooldownWidget)
    {
        //UE_LOG(LogTemp, Error, TEXT("DashCooldownWidget is NULL! Check if UI was created in BeginPlay."));
        return;
    }*/
    if (DashCooldownTimer > 0)
    {
        DashCooldownTimer -= 0.1f;
        float CooldownPercentage = DashCooldownTimer / DashCooldown;

       // UE_LOG(LogTemp, Log, TEXT("Cooldown Percentage: %f"), CooldownPercentage);

        //DashCooldownWidget->UpdateDashCooldown(CooldownPercentage);
        if (HPDashUIActorInstance)
        {
            UHPandDashUI* UIWidget = Cast<UHPandDashUI>(HPDashUIActorInstance->FindComponentByClass<UWidgetComponent>()->GetWidget());
            if (UIWidget)
            {
                UIWidget->UpdateSimpleDashBar(CooldownPercentage);
            }
        }
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
    else if (OverlapDrawingBook)
    {
        ShowDrawingBookUI();
    }
    else
    {
        TArray<AActor*> FoundNPC;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC1Class::StaticClass(), FoundNPC);

        for (AActor* Actor : FoundNPC)
        {
            ANPC1Class* NPC = Cast<ANPC1Class>(Actor);
            if (NPC && FVector::Dist(GetActorLocation(), NPC->GetActorLocation()) <= 500.0f)
            {
                NPC->ShowTextUI();
                break;
            }
        }
    }
}

void AAGSDCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    FString ReasonText;
    UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
    if (IsValid(GI)) {
        if (GI->WeaponArray.Num() > 1&&WeaponArray.Num() > 1) {
            GI->WeaponArray[0] = WeaponArray[0];
            GI->WeaponArray[1] = WeaponArray[1];
        }      
    }

}

//체력바 갱신 함수
void AAGSDCharacter::UpdateHealthBar()
{
	if (HealthBarWidget)
	{
		float HealthPercentage = static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
        float BarrierPercentage = static_cast<float>(Barrier) / static_cast<float>(MaxHealth);

		UProgressBar* HealthProgressBar = Cast<UProgressBar>(HealthBarWidget->GetWidgetFromName(TEXT("HealthBar")));
        UProgressBar* BarrierProgressBar = Cast<UProgressBar>(HealthBarWidget->GetWidgetFromName(TEXT("BarrierBar")));
		if (HealthProgressBar && BarrierProgressBar)
		{
			HealthProgressBar->SetPercent(HealthPercentage);
            BarrierProgressBar->SetPercent(BarrierPercentage);
		}
        if (HPDashUIActorInstance)
        {
            UHPandDashUI* UIWidget = Cast<UHPandDashUI>(HPDashUIActorInstance->FindComponentByClass<UWidgetComponent>()->GetWidget());
            if (UIWidget)
            {
                UIWidget->UpdateSimpleHPBar(HealthPercentage);
            }
        }
        UpdateStat();
	}
    
}
void AAGSDCharacter::UpdateXPBar()
{
    if (!HealthBarWidget) return;

    // XP Progress Bar 가져오기
    UProgressBar* XPProgressBar = Cast<UProgressBar>(HealthBarWidget->GetWidgetFromName(TEXT("XPBar")));

    // 레벨 표시 TextBlock 가져오기
    UTextBlock* LevelText = Cast<UTextBlock>(HealthBarWidget->GetWidgetFromName(TEXT("LevelText")));

    // XP 바 업데이트
    if (XPProgressBar)
    {
        float XPPercentage = (XPToNextLevel > 0) ? static_cast<float>(CurrentXP) / XPToNextLevel : 0.0f;
        XPProgressBar->SetPercent(XPPercentage);
    }

    // 레벨 텍스트 업데이트
    if (LevelText)
    {
        LevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CharacterLevel)));
    }
}
void AAGSDCharacter::UpdateStat()
{
    if (!HealthBarWidget) return;

    UTextBlock* HPText = Cast<UTextBlock>(HealthBarWidget->GetWidgetFromName(TEXT("HPText")));
    if (HPText)
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentHealth, MaxHealth)));
    }

    if (StatPanelWidget)
    {
        StatPanelWidget->SetupStatBars(this);  // 전체 스탯 갱신
    }
}

void AAGSDCharacter::AddXP(int32 XPAmount)
{
    XPAmount *= BounsXPLevel;
	CurrentXP += XPAmount ; // 주어진 XP를 현재 경험치에 더함
	float XPPercentage = static_cast<float>(CurrentXP) / static_cast<float>(XPToNextLevel);
	//UE_LOG(LogTemp, Log, TEXT("Increases XP: %d / %d"), CurrentXP, XPToNextLevel);
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
	//UE_LOG(LogTemp, Log, TEXT("레벨 업! 새로운 레벨: %d"), CharacterLevel);

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
		//FInputModeUIOnly InputMode;
		//PlayerController->SetInputMode(InputMode);
		//PlayerController->bShowMouseCursor = true;
	}
}
//레벨업 선택 이후 게임 이어하기
void AAGSDCharacter::ResumeGameAfterLevelUp()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		//FInputModeGameOnly InputMode;
		//PlayerController->SetInputMode(InputMode);
		//PlayerController->bShowMouseCursor = true;
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
    else if (IsValid(DrawingBookWidget))
    {
        DrawingBookWidget->RemoveFromViewport();
        DashCooldownWidget->AddToViewport();
        HealthBarWidget->AddToViewport();
        DrawingBookWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("DrawingBookWidget removed successfully."));
    }
    else if (IsValid(WeaponExchangeWidget)) {
        WeaponExchangeWidget->RemoveFromViewport();
        WeaponExchangeWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("WeaponExchangeWidget removed successfully."));
    }
    else if (IsValid(WeaponAscensionWidget)) {
        WeaponAscensionWidget->RemoveFromViewport();
        WeaponAscensionWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("WeaponAscensionWidget removed successfully."));
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
        float CooldownTime = FireRate / AttackSpeedLevel;
        // 공격 시 ProgressBar를 100%로 초기화합니다.
        if (DashCooldownWidget)  // DashCooldownUI는 UDashCooldown_UI의 인스턴스라고 가정합니다.
        {
            DashCooldownWidget->UpdateAttackCooldown(1.0f);
        }
        // 메인 쿨다운 타이머 설정 (쿨다운이 끝나면 OnCooldownFinished 함수 호출)
        GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AAGSDCharacter::OnAttackCooldownFinished, CooldownTime, false);

        // 쿨다운 시작 시간과 전체 쿨다운 시간 저장
        AttackCooldownStartTime = GetWorld()->GetTimeSeconds();
        AttackTotalCooldownTime = CooldownTime;

        // 진행 바 업데이트를 위한 반복 타이머 설정 (예: 0.05초 간격)
        GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AAGSDCharacter::UpdateAttackCooldownProgress, 0.05f, true);
	}
	if (FireMontage && GetMesh())
	{
        if (!AnimInstance) {
            //UE_LOG(LogTemp, Log, TEXT("NO Anim"));
            return;
        }
        PlayFireMontage(FireMontage);
	}	
}
void AAGSDCharacter::UpdateAttackCooldownProgress()
{
    // 현재 시간과 시작 시간의 차이를 계산하여 경과 시간을 구합니다.
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - AttackCooldownStartTime;

    // 남은 시간을 계산하고 비율로 변환합니다.
    float RemainingTime = AttackTotalCooldownTime - ElapsedTime;
    float Percentage = FMath::Clamp(RemainingTime / AttackTotalCooldownTime, 0.0f, 1.0f);

    // ProgressBar를 업데이트합니다.
    if (DashCooldownWidget)
    {
        DashCooldownWidget->UpdateAttackCooldown(Percentage);
    }

    // 남은 시간이 0 이하이면 타이머를 중지하고 ProgressBar를 0%로 설정합니다.
    if (RemainingTime <= 0.0f)
    {
        GetWorldTimerManager().ClearTimer(AttackCooldownTimerHandle);
        if (DashCooldownWidget)
        {
            DashCooldownWidget->UpdateAttackCooldown(0.0f);
        }
    }
}
void AAGSDCharacter::OnAttackCooldownFinished()
{
    // 쿨다운 완료 시 추가로 실행할 로직이 있다면 여기에 작성합니다.
}

void AAGSDCharacter::PlayFireMontage(UAnimMontage* Montage)
{
    if (!Montage) return;

    // 변수 초기화
    CurrentMontage = Montage;
    // Delegate 설정
    //FOnMontageEnded MontageEndedDelegate;
    //MontageEndedDelegate.BindUObject(this, &AAGSDCharacter::OnMontageEnded);

    // 몽타주 재생 및 Delegate 바인딩
    AnimInstance->Montage_Play(Montage,1.0f*RepeatFire*AttackSpeedLevel);
    //AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
}

/*
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
        OnMontageEnded(Montage, false);
    }
    else
    {
        // 반복 종료
        CurrentCount = 0;
        UE_LOG(LogTemp, Display, TEXT("Montage playback completed %d times."), RepeatCount);
    }
}
*/

void AAGSDCharacter::ShowWeaponExchangeUI()
{
    int Overlap = FCString::Atoi(*OverlapID);
    if ((WeaponArray[0] == Overlap) || (WeaponArray[1] == Overlap)) {
        if (WeaponAscensionClass) {
            WeaponAscensionWidget = CreateWidget<UUserWidget>(GetWorld(), WeaponAscensionClass);
        }
        if (WeaponAscensionWidget) {
            WeaponAscensionWidget->AddToViewport();
            PauseGameForLevelUp();
            
        }
    }
    return;
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
    NoSwap=WeaponSwapCheck();
    if (NoSwap) {
        return;
    }
    
	if (CurrentWeaponSlot) {
        WeaponID = FString::FromInt(WeaponArray[1]);
        WeaponTake();
        UpdateSwapWeaponIcon();
        CurrentWeaponSlot = !CurrentWeaponSlot;
	}
	else
	{
        WeaponID = FString::FromInt(WeaponArray[0]);
        WeaponTake();
        UpdateSwapWeaponIcon();
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

void AAGSDCharacter::UpdateSwapWeaponIcon()
{
    if (!DashCooldownWidget) return;
    DashCooldownWidget->UpdateSwapWeapon();
}


void AAGSDCharacter::SpawnParticle(FVector Location, FRotator Rotation)
{
    //FVector AddVector = FVector(100.0f, 0, 0);
    //AddVector = Rotation.RotateVector(AddVector);
    Location = Location;
    UE_LOG(LogTemp, Log, TEXT("%.2f %.2f %.2f"), Location.X, Location.Y,Location.Z);
    UNiagaraComponent* WeaponParticleComponent;
    WeaponParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponParticle, Location, Rotation);
    if(IsValid(WeaponParticleComponent)) {
        WeaponParticleComponent->Activate();
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
        if (SubWeaponAttachPoint) {
            NewSubWeapon->AttachToComponent(SubWeaponAttachPoint, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("SubWeaponSocket")));
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
    RangeType = WeaponData->WeaponRangeType;
    Ascension = WeaponData->Ascension;

    WeaponAnimType = WeaponData->WeaponAnimType;
    UAnimInstance* CurrentAnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
    CurrentAnimInstance->LinkAnimClassLayers(WeaponAnimType);
    if (WeaponData->WeaponParticle != nullptr) {
        WeaponParticle = WeaponData->WeaponParticle;
    }
    else {
        WeaponParticle = nullptr;
    }
    if (WeaponData->WeaponSound != nullptr) {
        WeaponSoundCue = WeaponData->WeaponSound;
    }
    else {
        WeaponSoundCue = nullptr;
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
                //if(OverlapWeaponDrop->InteractionWidget) OverlapWeaponDrop->InteractionWidget->SetVisibility(true);
                OverlapID = *OverlapWeaponDrop->WeaponID;
            }
        }
        // 충돌한 오브젝트가 Box임을 확인
        if (AStorageBox* Box = Cast<AStorageBox>(OtherActor))
        {
            OverlapBox = true;
            //UE_LOG(LogTemp, Log, TEXT("OverLap Box"));
            if (Box->InteractionWidget)
            {
                Box->InteractionWidget->SetVisibility(true);
                //UE_LOG(LogTemp, Log, TEXT("Widget SetVisible"));
            }
            else
            {
                //UE_LOG(LogTemp, Log, TEXT("Error InteractionUI"));
            }
        }
        // 충돌한 오브젝트가 DrawingBook임을 확인
        if (ADrawingBook* Book = Cast<ADrawingBook>(OtherActor))
        {
            OverlapDrawingBook = true;
            if (Book->InteractionWidget) Book->InteractionWidget->SetVisibility(true);
            if (OverlapDrawingBook) UE_LOG(LogTemp, Log, TEXT("OverLap DrawingBook"));
        }
        if (ANPC1Class* npc = Cast<ANPC1Class>(OtherActor))
            if (npc->InteractionWidget) npc->InteractionWidget->SetVisibility(true);
    }
}

void AAGSDCharacter::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
    {
        // 충돌한 오브젝트가 WeaponDrop임을 확인
        if (OtherActor && OtherActor->IsA<AWeaponDrop>())
        {
            AWeaponDrop* WeaponDrop = Cast<AWeaponDrop>(OtherActor);
            if (WeaponDrop->InteractionWidget) WeaponDrop->InteractionWidget->SetVisibility(false);
            OverlapDropWeapon = false;
        }
        // 충돌한 오브젝트가 Box임을 확인
        if (AStorageBox* Box = Cast<AStorageBox>(OtherActor))
        {
            OverlapBox = false;
            //UE_LOG(LogTemp, Log, TEXT("OverLap out Box"));
            if (Box->InteractionWidget)
            {
                Box->InteractionWidget->SetVisibility(false);
            }
        }
        // 충돌한 오브젝트가 DrawingBook임을 확인
        if (ADrawingBook* Book = Cast<ADrawingBook>(OtherActor))
        {
            OverlapDrawingBook = false;
            if (Book->InteractionWidget) Book->InteractionWidget->SetVisibility(false);
        }
        if (ANPC1Class* npc = Cast<ANPC1Class>(OtherActor))
            if (npc->InteractionWidget) npc->InteractionWidget->SetVisibility(false);
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
        WeaponSwap();
        WeaponSwap();
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

void AAGSDCharacter::FireByType()
{
    if (WeaponSoundCue != nullptr) {
        UGameplayStatics::PlaySound2D(GetWorld(), WeaponSoundCue);
    }

    switch (RangeType)
    {
    case ERangeType::RapidFire:
        RapidFire();
        break;
    case ERangeType::SprayFire:
        SprayFire();
        break;
    case ERangeType::BiggerProjectile:
        BiggerProjectile();
        break;
    default:
        break;
    }
}

void AAGSDCharacter::RapidFire()
{
    FireCount = 0;
    GetWorldTimerManager().SetTimer(RapidFireTimerHandle, this, &AAGSDCharacter::RapidFireCount, 0.25f, true);
}

void AAGSDCharacter::RapidFireCount()
{
    CreateProjectile(0.0f,false);
    FireCount++;
    int MoreProjectile = AttackRangeLevel;
    if (FireCount > MoreProjectile + Numberofprojectile -2) {
        GetWorldTimerManager().ClearTimer(RapidFireTimerHandle);
    }
}

void AAGSDCharacter::SprayFire()
{
    int MoreProjectile = AttackRangeLevel;
    for (int i = 0; i < Numberofprojectile + MoreProjectile - 1; i++) {
        float AdjustYaw = (i - (Numberofprojectile - 1) / 2.0f) * SpreadAngle;
        CreateProjectile(AdjustYaw,false);
    }
}

void AAGSDCharacter::BiggerProjectile()
{
    CreateProjectile(0.0f,true);
}

void AAGSDCharacter::CreateProjectile(float AdjustedYaw, bool Bigger)
{
    if (ProjectileClass) {
        // 총구 방향
        FRotator MuzzleRotation = TraceHitDirection.Rotation();
        MuzzleRotation.Pitch = 0;
        MuzzleRotation.Roll = 0;

        // 총구위치 설정
        MuzzleLocation = CharacterLocation + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
        MuzzleLocation.Z = CharacterLocation.Z;

        // 탄환 생성
        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();
           
            AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
            FRotator AdjustedRotation = FRotator(MuzzleRotation.Pitch, MuzzleRotation.Yaw + AdjustedYaw, MuzzleRotation.Roll);
            if (Projectile)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));
                FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
                if (Bigger) {
                    Projectile->SetActorScale3D(FVector(AttackRangeLevel));
                }
                Projectile->SetPlayerState(Attack, AttackRangeLevel, Ascension);
                Projectile->SetActorEnableCollision(true);
                Projectile->SetActorTickEnabled(true);
                if (WeaponData)
                {
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

/*
void AAGSDCharacter::CreateProjectile()
{

    // 발사
    if (!WeaponType) {
        if (ProjectileClass)
        {
            // 총구 방향
            FRotator MuzzleRotation = TraceHitDirection.Rotation();
            MuzzleRotation.Pitch = 0;
            MuzzleRotation.Roll = 0;

            // 총구위치 설정
            MuzzleLocation = CharacterLocation + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
            //MuzzleLocation.Z = CharacterLocation.Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
            MuzzleLocation.Z = CharacterLocation.Z;

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
                    AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                    float AdjustedYaw = MuzzleRotation.Yaw + (i - (Numberofprojectile - 1) / 2.0f) * SpreadAngle;
                    FRotator AdjustedRotation = FRotator(MuzzleRotation.Pitch, AdjustedYaw, MuzzleRotation.Roll);
                    if (Projectile)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));
                        FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
                        Projectile->SetPlayerState(Attack, AttackRangeLevel);
                        Projectile->SetActorEnableCollision(true);
                        Projectile->SetActorTickEnabled(true);
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
            MuzzleLocation.Z = CharacterLocation.Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();


            MuzzleRotation.Pitch += 0.0f;

            // 탄환 생성
            UWorld* World = GetWorld();
            if (World)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();
                float LeftRight = 5.0f;
                //탄환숫자만큼 발사반복
                for (int i = 0; i < Numberofprojectile; i++) {
                    if (i % 2 == 1) {
                        MuzzleOffset.Y = -1 * LeftRight;

                    }
                    else if (i != 0 && i % 2 == 0) {
                        MuzzleOffset.Y = 1 * LeftRight;
                        LeftRight += 5.0f;
                    }
                    MuzzleLocation = CharacterLocation + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
                    AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                    if (Projectile)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));
                        FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
                        Projectile->SetPlayerState(Attack, AttackRangeLevel);
                        Projectile->SetActorEnableCollision(true);
                        Projectile->SetActorTickEnabled(true);
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
*/
void AAGSDCharacter::Attacked(float Damage)
{
    if (bIsInvincible) // 무적 상태에서는 피해 무시
    {
        return;
    }
    int32 fixed_Damge = (int32)(Damage) - (int32)(Damage * ((Defense - 100.0f) / 100.0f));
    //체력계산
    if (fixed_Damge > 0) //받는 데미지가 음수가 되버리면 0으로 계산
    {
        if (Barrier > 0 && fixed_Damge >= Barrier)
        {
            CurrentHealth -= fixed_Damge - Barrier;
            Barrier = 0;
        }
        else if (Barrier > 0 && fixed_Damge < Barrier)
        {
            Barrier -= fixed_Damge;
        }
        else
        {
            CurrentHealth -= fixed_Damge;
        }
    }
    else
    {
        CurrentHealth -= 0;
    }
	UE_LOG(LogTemp, Display, TEXT("HP : %d"), CurrentHealth);
    //피격시 무적(데미지는 받고 이후 5초간 무적 / 60초 쿨타임)
    if (Invincibility_Cooldown && bIs_Attacked_Invincible)
    {
        float tempcooldown = 5.0f;
        bIsInvincible = true;
        GetWorldTimerManager().SetTimer(
            InvincibilityTimerHandle2, this, &AAGSDCharacter::ResetInvincibility2, tempcooldown, false);
    }
    if (CurrentHealth <= 0) //캐릭터 사망 및 부활
    {        
        if (IsResurrection) //부활
        {
            IsResurrection = false;
            CurrentHealth = 10;
            bIsInvincible = true;       // 무적 상태 설정
            // 무적 해제 타이머 시작
            GetWorldTimerManager().SetTimer(
                InvincibilityTimerHandle, this, &AAGSDCharacter::ResetInvincibility, 1.0f, false);
            UE_LOG(LogTemp, Display, TEXT("Character Resurrection"));
            UpdateHealthBar();                   
        }            
        else
        {
            UpdateHealthBar();
            OnDeath(); // 사망 처리               
        }                
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
        //FInputModeUIOnly InputMode;
        //PlayerController->SetInputMode(InputMode);
        //PlayerController->bShowMouseCursor = true;
    }

    // Restart UI 생성
    if (GameoverUIClass)
    {
        GameoverWidget = CreateWidget<UUserWidget>(GetWorld(), GameoverUIClass);
        if (GameoverWidget)
        {
            GameoverWidget->AddToViewport();
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
        //FInputModeUIOnly InputMode;
        //PlayerController->SetInputMode(InputMode);
        //PlayerController->bShowMouseCursor = true;
    }
    // Restart UI 생성
    if (ClearUIClass)
    {
        ClearWidget = CreateWidget<UUserWidget>(GetWorld(), ClearUIClass);
        if (ClearWidget)
        {
            ClearWidget->AddToViewport();
        }
    }

    // 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("Level Clear"));
}
//창고 UI 출력
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
//도감 UI 출력
void AAGSDCharacter::ShowDrawingBookUI()
{
    if (DrawingBookWidgetClass)
    {
        DrawingBookWidget = CreateWidget<UUserWidget>(GetWorld(), DrawingBookWidgetClass);
        if (DrawingBookWidget)
        {
            DrawingBookWidget->AddToViewport();
            DashCooldownWidget->RemoveFromViewport();
            HealthBarWidget->RemoveFromViewport();
            PauseGameForLevelUp();
        }
    }
}

bool AAGSDCharacter::WeaponSwapCheck()
{
    if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle)) {
        return true;
    }
    else
    {
        return false;
    }
}

void AAGSDCharacter::PlayWalkingSound()
{
    // 캐릭터의 현재 속도 체크
    FVector Velocity = GetVelocity();
    float Speed = Velocity.Size();

    // 걷기 속도 기준 (최소 속도보다 빠르면 걷는 중)
    float WalkingSpeedThreshold = 10.0f;

    if (Speed > WalkingSpeedThreshold)
    {
        // 이동 중이면 걷는 소리 재생
        if (!IsWalking)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Walking"));
            IsWalking = true;
            if (WalkingAudioComponent && WalkingSound)
            {
                WalkingAudioComponent->SetSound(WalkingSound);
                WalkingAudioComponent->Play();
            }
            if (!WalkingAudioComponent)
            {
                UE_LOG(LogTemp, Warning, TEXT("WalkingAudioComponent"));
            }
            if (!WalkingSound)
            {
                UE_LOG(LogTemp, Warning, TEXT("WalkingSound"));
            }
        }
    }
    else
    {
        // 이동하지 않으면 걷는 소리 중지
        if (IsWalking)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Not Walking"));
            IsWalking = false;
            if (WalkingAudioComponent && WalkingSound)
            {
                WalkingAudioComponent->Stop();
            }
        }
    }
}

void AAGSDCharacter::InitializeAccessoryList()
{
    GetAccessory.Empty(); // 게임 시작 시 리스트 초기화
}

void AAGSDCharacter::PlayingGetAccessoryRowName(FName RowName)
{
    GetAccessory.Add(RowName);
}