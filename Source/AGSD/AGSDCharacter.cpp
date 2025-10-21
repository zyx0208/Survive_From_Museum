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
#include "Enemy1Class.h"
#include "Enemy1AIController.h"
#include "TrapChest.h"




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
	XPToNextLevel = 10;       // 첫 번째 레벨 업까지 필요한 경험치
	BounsXPLevel = 1.0f;		//획득 경험치 증가
    XPRangeLevel = 200.0f;        //획득 자석 범위

    AttackSpeedLevel = 1.0f;
    AttackRangeLevel = 1.0f;        //3단계로 진행
    IsWalking = false;
    DashCooldown = 3.0f;

	PrimaryActorTick.bCanEverTick = true; // Tick 함수 활성화

    AcquiredAccessories.Empty();    //획득 악세서리 초기화

    InitializeAccessoryList();  //획득한 악세서리 초기화

    IsResurrection = false;     //부활
    bIs_Attacked_Invincible = false;    //피격시 무적
    bGuard = false;//악세서리 가드횟수 초기화
    Guard = 0;     

    //5스테이지 전용 세트옵션 bool값 초기화
    steel1, steel2, steel3 = false;
    steelp1, steelp2, steelp3 = false;
    dna, nuclear = false;

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
    FollowCamera->PrimaryComponentTick.bTickEvenWhenPaused = true;

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

    if (!WeaponSkeletalMeshComponent) {
        WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMeshComponent"));
        WeaponSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponSkeletalMeshComponent->SetupAttachment(WeaponMeshComponent);
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
        PlayerController->bShowMouseCursor = true;
        
        ApplyGameAndUICursorMode();

        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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
        UE_LOG(LogTemp, Log, TEXT("%d %d 게임시작"), WeaponArray[0], WeaponArray[1]);
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
            DashCooldownWidget->UpdatePrimeZCooldown(0.0f);
            DashCooldownWidget->UpdatePrimeXCooldown(0.0f);
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
    
   
    AnimInstance = GetMesh()->GetAnimInstance();
    
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

			//라인트레이스 위치와 방향 저장
			TraceHitLocation = HitResult.Location;
			TraceHitDirection = (HitResult.Location - CharacterLocation).GetSafeNormal();
            FRotator NewActorRotation = GetActorRotation();
            NewActorRotation.Yaw = TraceHitDirection.Rotation().Yaw;
            SetActorRotation(NewActorRotation);
			
		}
	}

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

        // PrimeSet "z"key Action
        EnhancedInputComponent->BindAction(PrimeZAction, ETriggerEvent::Triggered, this, &AAGSDCharacter::PrimeZ);
        // PrimeSet "x"key Action
        EnhancedInputComponent->BindAction(PrimeXAction, ETriggerEvent::Triggered, this, &AAGSDCharacter::PrimeX);
		
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

        //스탯 토글 버튼
        PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &AAGSDCharacter::ToggleStatPanelBox);
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
    //DrawDebugLine(GetWorld(), TempCharacterLocation, DesiredCameraLocation, FColor::Blue, false, 0.1f, 0, 1.0f);

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
void AAGSDCharacter::Talking(bool dialogueInput)
{
    if (dialogueInput) {
        bIsInvincible = true;
    }
    else if (!dialogueInput) {
        bIsInvincible = false;
    }
}
//일시정시 관리 함수
void AAGSDCharacter::HandleEscape()
{
    UE_LOG(LogTemp, Warning, TEXT("ESC pressed, Game Pause"));
}

// 스탯 토글 관리 함수
void AAGSDCharacter::ToggleStatPanelBox()
{
    if (StatPanelWidget)
    {
        StatPanelWidget->ToggleStatBox();
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
    if (WeaponSwapCheck()) {
        return;
    }

    if (OverlapDropWeapon) 
    {
        ShowWeaponExchangeUI();
    }
    else if (OverlapBox)
    {
        ShowStorageBoxUI();
        //UE_LOG(LogTemp, Log, TEXT("Box here"));
    }
    /*else if (OverlapDrawingBook)
    {
        ShowDrawingBookUI();
    }*/
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

    //5stage 전용 상자 상호작용
    if (ATrapChest* Chest = FindNearbyTrapChest())
    {
        UE_LOG(LogTemp, Log, TEXT("TrapChest here"));
        Chest->Interact(this);   // 확률/문 처리 전부 Chest가 담당
        return;
    }
    else UE_LOG(LogTemp, Log, TEXT("TrapChest missing"));
}

bool AAGSDCharacter::CheckReinforce()
{
    FWeaponDataTableBetaStruct* WeaponData1 = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(FString::FromInt(WeaponArray[0])), TEXT("Weapon Lookup"));
    FWeaponDataTableBetaStruct* WeaponData2 = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(FString::FromInt(WeaponArray[1])), TEXT("Weapon Lookup"));
    if (WeaponData1 && WeaponData2) {
        FString ReinforceStageName = GetLevel()->GetOuter()->GetName();
        if (WeaponData1->ReinforceStage == ReinforceStageName || WeaponData2->ReinforceStage == ReinforceStageName) {
            return true;
        }
        else if (ReinforceStageName == "TutorialStage4_BossMonster") return true; // 튜토리얼용 찬란한 강화
    }
    return false;
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
    while (CurrentXP >= XPToNextLevel)
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
	XPToNextLevel = XPToNextLevel * 1.15; // 다음 레벨로 가기 위한 경험치 15% 증가

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
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        PlayerController->SetPause(true);
        ApplyGameAndUICursorMode(); // 추가
    }
}
//레벨업 선택 이후 게임 이어하기
void AAGSDCharacter::ResumeGameAfterLevelUp()
{
    StopFiring();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
        PlayerController->SetPause(false);
        ApplyGameAndUICursorMode();
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
    /*else if (IsValid(DrawingBookWidget))
    {
        DrawingBookWidget->RemoveFromViewport();
        DashCooldownWidget->AddToViewport();
        HealthBarWidget->AddToViewport();
        DrawingBookWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("DrawingBookWidget removed successfully."));
    }*/
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
    else if (IsValid(ReinforceUIWidget)) {
        ReinforceUIWidget->RemoveFromViewport();
        ReinforceUIWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("ReinforceUIWidget1 removed successfully."));
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
    if (WeaponFireSequence && WeaponSkeletalMeshComponent->GetSkeletalMeshAsset()) {
        WeaponSkeletalMeshComponent->PlayAnimation(WeaponFireSequence, false);
        WeaponSkeletalMeshComponent->SetPlayRate(1.0f * AttackSpeedLevel);
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
        return;
    }
    
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
    
}

void AAGSDCharacter::UpdateSwapWeaponIcon()
{
    if (!DashCooldownWidget) return;
    DashCooldownWidget->UpdateSwapWeapon();
}


void AAGSDCharacter::SpawnParticle(FVector Location, FRotator Rotation, FVector Scale)
{
    //FVector AddVector = FVector(100.0f, 0, 0);
    //AddVector = Rotation.RotateVector(AddVector);
    Location = Location;
    UE_LOG(LogTemp, Log, TEXT("%.2f %.2f %.2f"), Location.X, Location.Y,Location.Z);
    UNiagaraComponent* WeaponParticleComponent;
    WeaponParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponParticle, Location, Rotation, Scale);
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
    CurrentSkeletalMesh = WeaponData->WeaponSkeletalMesh;
    RepeatFire = WeaponData->RepeatFire;
    WeaponMeshComponent->SetStaticMesh(CurrentWeaponMesh);
    if (CurrentSkeletalMesh) {
        WeaponMeshComponent->SetVisibility(false);
    }
    else
    {
        WeaponMeshComponent->SetVisibility(true);
    }
    WeaponSkeletalMeshComponent->SetSkeletalMesh(CurrentSkeletalMesh);
    WeaponFireSequence = WeaponData->WeaponSkeletalAnimSequence;
	WeaponType = WeaponData->WeaponType;
    RangeType = WeaponData->WeaponRangeType;
    UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
    if (GI) {
        int32* pointerValue = GI->Temp_Ascension.Find(*WeaponID);
        Ascension = *pointerValue;
    }
    else {
        Ascension = 0;
    }

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
                OverlapWeaponDropArray.Add(OverlapWeaponDrop);
                OverlapID = *OverlapWeaponDrop->WeaponID;
            }
            SortOverlapWeaponDropArray();
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
        /*if (ADrawingBook* Book = Cast<ADrawingBook>(OtherActor))
        {
            OverlapDrawingBook = true;
            if (Book->InteractionWidget) Book->InteractionWidget->SetVisibility(true);
            if (OverlapDrawingBook) UE_LOG(LogTemp, Log, TEXT("OverLap DrawingBook"));
        }*/

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
            

            // null 체크 및 배열에 존재 여부 확인 후 RemoveSingle 호출
            if (IsValid(WeaponDrop))
            {
                if (WeaponDrop->InteractionWidget) WeaponDrop->InteractionWidget->SetVisibility(false);
                OverlapWeaponDropArray.RemoveSingle(WeaponDrop);
                SortOverlapWeaponDropArray();
            }

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
        /*if (ADrawingBook* Book = Cast<ADrawingBook>(OtherActor))
        {
            OverlapDrawingBook = false;
            if (Book->InteractionWidget) Book->InteractionWidget->SetVisibility(false);
        }*/
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
        UpdateSwapWeaponIcon();
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Not Ovelap WeaponDrop")));
    }
}
void AAGSDCharacter::SortOverlapWeaponDropArray()
{
    const FVector MyLocation = GetActorLocation();

        OverlapWeaponDropArray.Sort([MyLocation](const AActor& A, const AActor& B)
        {
            return FVector::DistSquared(A.GetActorLocation(), MyLocation) < FVector::DistSquared(B.GetActorLocation(), MyLocation);
        });

        if (OverlapWeaponDropArray.Num() > 0) {
            OverlapWeaponDrop = OverlapWeaponDropArray[0];
            OverlapID = OverlapWeaponDropArray[0]->WeaponID;
        }
}
void AAGSDCharacter::StartFiring()
{
    ApplyGameAndUICursorMode();
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
    UE_LOG(LogTemp, Log, TEXT("StopFiring"));
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
    if (RangeType == ERangeType::ChargeBeam) {
        StopCharging();
    }
}

void AAGSDCharacter::FireByType()
{

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
    case ERangeType::ChargeBeam:
        ChargeFire();
        break;
    case ERangeType::ComplexFire:
        ComplexFire();
        break;
    default:
        break;
    }

}

void AAGSDCharacter::LockCharge()
{
    UE_LOG(LogTemp, Log, TEXT("LockCharge"));
}

void AAGSDCharacter::RapidFire()
{
    FireCount = 0;
    if (WeaponSoundCue != nullptr) {
        UGameplayStatics::PlaySound2D(GetWorld(), WeaponSoundCue);
    }
    GetWorldTimerManager().SetTimer(RapidFireTimerHandle, this, &AAGSDCharacter::RapidFireCount, 0.1f, true);

}

void AAGSDCharacter::RapidFireCount()
{
    CreateProjectile(0.0f,false);
    FireCount++;
    if (FireCount > Numberofprojectile-1) {
        GetWorldTimerManager().ClearTimer(RapidFireTimerHandle);
    }
}

void AAGSDCharacter::SprayFire()
{
    if (WeaponSoundCue != nullptr) {
        UGameplayStatics::PlaySound2D(GetWorld(), WeaponSoundCue);
    }
    for (int i = 0; i < Numberofprojectile ; i++) {
        float AdjustYaw = (i - (Numberofprojectile - 1) / 2.0f) * SpreadAngle;
        CreateProjectile(AdjustYaw,false);
    }
}

void AAGSDCharacter::BiggerProjectile()
{
    if (WeaponSoundCue != nullptr) {
        UGameplayStatics::PlaySound2D(GetWorld(), WeaponSoundCue);
    }
    CreateProjectile(0.0f,false);
}

void AAGSDCharacter::ChargeFire()
{
    if (Charge < 0.5f) {
        CancelCharge();
        return;
    }
    if (WeaponSoundCue != nullptr) {
        UGameplayStatics::PlaySound2D(GetWorld(), WeaponSoundCue);
    }
    CreateProjectile(0.0f, true);
}

void AAGSDCharacter::ComplexFire()
{
    FireCount = 0;
    if (WeaponSoundCue != nullptr) {
        UGameplayStatics::PlaySound2D(GetWorld(), WeaponSoundCue);
    }
    GetWorldTimerManager().SetTimer(RapidFireTimerHandle, this, &AAGSDCharacter::ComplexFireCount, 0.1f, true);
}

void AAGSDCharacter::ComplexFireCount()
{
    FireCount++;
    SprayFire();
    if (FireCount > Numberofprojectile) {
        
        GetWorldTimerManager().ClearTimer(RapidFireTimerHandle);
    }
}

void AAGSDCharacter::Charging()
{
    Charge += ChargePerTime;
    if (Charge >= 2.0f) {
        Charge = 2.0f;
    }
}

void AAGSDCharacter::StopCharging()
{
    UAnimInstance* ChargeAnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
    if (ChargeAnimInstance && FireMontage)
    {
        ChargeAnimInstance->Montage_JumpToSection(FName("FireCharge"), FireMontage);
    }
    if (WeaponFireSequence && WeaponSkeletalMeshComponent->GetSkeletalMeshAsset()) {
        WeaponSkeletalMeshComponent->PlayAnimation(WeaponFireSequence, false);
        WeaponSkeletalMeshComponent->Stop();
    }
}

void AAGSDCharacter::CancelCharge()
{
    UAnimInstance* ChargeAnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
    if (ChargeAnimInstance && FireMontage)
    {
        ChargeAnimInstance->Montage_Stop(0.2f, FireMontage);
    }
    if (WeaponFireSequence && WeaponSkeletalMeshComponent->GetSkeletalMeshAsset()) {
        WeaponSkeletalMeshComponent->Stop();
    }
    Charge = 0;
}

void AAGSDCharacter::CreateProjectile(float AdjustedYaw, bool ChargeFire)
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
                Projectile->SetActorScale3D(FVector(AttackRangeLevel));
                Projectile->SetPlayerState(Attack, AttackRangeLevel, Ascension);
                if (ChargeFire) {
                    Projectile->SetPlayerState(Attack+Charge, AttackRangeLevel, Ascension);
                    Projectile->SetActorScale3D(FVector(Charge));
                }
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
                FVector ParticleScale = FVector(AttackRangeLevel);

                SpawnParticle(MuzzleLocation, AdjustedRotation, ParticleScale);
            }
        }
    }
    Charge = 0;
}


void AAGSDCharacter::Attacked(float Damage)
{
    if (bIsInvincible) // 무적 상태에서는 피해 무시
    {
        return;
    }
    if (Guard > 0)      //가드가 있는경우 1회 공격무시
    {
        Guard--;
        if (Guard <= 0) {
            TArray<UActorComponent*> TaggedComps = GetComponentsByTag(UNiagaraComponent::StaticClass(), FName("Seven"));

            for (UActorComponent* Comp : TaggedComps)
            {
                if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp))
                {
                    NiagaraComp->DestroyComponent();
                }
            }

        }
        else {
            TArray<UActorComponent*> TaggedComps = GetComponentsByTag(UNiagaraComponent::StaticClass(), FName("Seven"));

            for (UActorComponent* Comp : TaggedComps)
            {
                if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp))
                {
                    NiagaraComp->SetVariableInt("User.VisibleCount", Guard-1);
                }
            }
        }
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
        if (AttackedWidgetClass)
        {
            // 위젯이 아직 생성되지 않았거나 Viewport에 없다면 새로 추가
            if (!AttackedWidgetInstance || !AttackedWidgetInstance->IsInViewport())
            {
                AttackedWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), AttackedWidgetClass);
                if (AttackedWidgetInstance)
                {
                    AttackedWidgetInstance->AddToViewport();

                    // 일정 시간 후 제거 (예: 0.4초 뒤에 Viewport에서 제거)
                    FTimerHandle RemoveWidgetHandle;
                    GetWorldTimerManager().SetTimer(RemoveWidgetHandle, FTimerDelegate::CreateLambda([this]()
                        {
                            if (AttackedWidgetInstance && AttackedWidgetInstance->IsInViewport())
                            {
                                AttackedWidgetInstance->RemoveFromParent();
                                AttackedWidgetInstance = nullptr; // 다음 피격 시 다시 생성되도록 초기화
                            }
                        }), 0.4f, false);
                }
            }
        }
    }
    else
    {
        CurrentHealth -= 0;
        if (AttackedWidgetClass)
        {
            // 위젯이 아직 생성되지 않았거나 Viewport에 없다면 새로 추가
            if (!AttackedWidgetInstance || !AttackedWidgetInstance->IsInViewport())
            {
                AttackedWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), AttackedWidgetClass);
                if (AttackedWidgetInstance)
                {
                    AttackedWidgetInstance->AddToViewport();

                    // 일정 시간 후 제거 (예: 0.4초 뒤에 Viewport에서 제거)
                    FTimerHandle RemoveWidgetHandle;
                    GetWorldTimerManager().SetTimer(RemoveWidgetHandle, FTimerDelegate::CreateLambda([this]()
                        {
                            if (AttackedWidgetInstance && AttackedWidgetInstance->IsInViewport())
                            {
                                AttackedWidgetInstance->RemoveFromParent();
                                AttackedWidgetInstance = nullptr; // 다음 피격 시 다시 생성되도록 초기화
                            }
                        }), 0.4f, false);
                }
            }
        }
    }
	UE_LOG(LogTemp, Display, TEXT("HP : %d"), CurrentHealth);
    //피격시 무적(데미지는 받고 이후 5초간 무적 / 60초 쿨타임)
    if (Invincibility_Cooldown && bIs_Attacked_Invincible)
    {
        SpawnBuffVFX(EBuffType::Star, 5.0f);
        float tempcooldown = 5.0f;
        bIsInvincible = true;
        GetWorldTimerManager().SetTimer(
            InvincibilityTimerHandle2, this, &AAGSDCharacter::ResetInvincibility2, tempcooldown, false);
    }
    if (CurrentHealth <= 0) //캐릭터 사망 및 부활
    {        
        if (IsResurrection) //부활
        {
            TArray<UActorComponent*> TaggedComps = GetComponentsByTag(UNiagaraComponent::StaticClass(), FName("Revive"));

            for (UActorComponent* Comp : TaggedComps)
            {
                if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(Comp))
                {
                    NiagaraComp->DestroyComponent();
                }
            }
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
        UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());

        if (CheckReinforce()) {
            ReinforceUIWidget = CreateWidget<UUserWidget>(GetWorld(), ReinforceUIWidgetClass);
            if (ReinforceUIWidget)
            {
                ReinforceUIWidget->AddToViewport();
            }
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
/*void AAGSDCharacter::ShowDrawingBookUI()
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
}*/

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
/*디버프 적용 함수 모음*/
void AAGSDCharacter::UpdateControlLock()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        const bool bShouldLock = bIsStunned || bIsKnockback; // 필요 시 다른 상태 추가
        if (bShouldLock)
        {
            DisableInput(PC);
        }
        else
        {
            EnableInput(PC);
        }
    }
}

// 슬로우 적용
void AAGSDCharacter::SlowApply(float Duration)
{
    if (bIsSlowed || !CharacterMovementComponent) return;

    bIsSlowed = true;
    OriginalWalkSpeed = CharacterMovementComponent->MaxWalkSpeed;
    CharacterMovementComponent->MaxWalkSpeed *= 0.5f;

    if (SlowSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SlowSound, GetActorLocation());
    }

    GetWorldTimerManager().ClearTimer(SlowTimerHandle);
    GetWorldTimerManager().SetTimer(SlowTimerHandle, FTimerDelegate::CreateLambda([this]()
        {
            CharacterMovementComponent->MaxWalkSpeed = OriginalWalkSpeed;
            bIsSlowed = false;
        }), Duration, false);
}

// 스턴 적용
void AAGSDCharacter::StunApply(float Duration)
{
    if (bIsStunned) { GetWorldTimerManager().ClearTimer(StunTimerHandle); }

    bIsStunned = true;
    UpdateControlLock();

    DisableInput(Cast<APlayerController>(GetController()));

    if (StunSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, StunSound, GetActorLocation());
    }

    if (StunMontage) {
        UE_LOG(LogTemp, Warning, TEXT("stunmontage"));
        PlayAnimMontage(StunMontage, Duration/StunMontage->GetPlayLength());
    }

    GetWorldTimerManager().ClearTimer(StunTimerHandle);
    GetWorldTimerManager().SetTimer(StunTimerHandle, FTimerDelegate::CreateLambda([this]()
        {
            bIsStunned = false;
            UpdateControlLock();
        }), Duration, false);
}

// 넉백 적용
void AAGSDCharacter::KnockbackApply(FVector Direction, float Power)
{
    if (bIsKnockback || !CharacterMovementComponent) return;

    bIsKnockback = true;
    UpdateControlLock();

    Direction.Normalize();
    FVector KnockbackForce = Direction * Power;   //넉백량 조절
    LaunchCharacter(KnockbackForce, true, true);

    if (KnockbackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, KnockbackSound, GetActorLocation());
    }

    // 넉백 제어 복구 시간은 넉백 거리, 세기 등에 따라 조절
    GetWorldTimerManager().ClearTimer(KnockbackControlHandle);
    GetWorldTimerManager().SetTimer(KnockbackControlHandle, FTimerDelegate::CreateLambda([this]()
        {
            bIsKnockback = false;
            UpdateControlLock();            
        }), 0.6f, false);
}

//5스테이지 전용 악세서리
void AAGSDCharacter::SteelSet()
{
    if (steel1 && steel2 && steel3)
    {
        //세트 달성
        Attack += 1.0f;
        Defense += 5.0f;
        SpeedLevel += 50.0f;
        AttackSpeedLevel += 0.1f;
        MaxHealth += 5;
        CurrentHealth += 5;
    }
    else
    {
        //세트옵션 미달성
        UE_LOG(LogTemp, Warning, TEXT("SteelSet 미달성 - 1: %s, 2: %s, 3: %s"),
            steel1 ? TEXT("true") : TEXT("false"),
            steel2 ? TEXT("true") : TEXT("false"),
            steel3 ? TEXT("true") : TEXT("false"));
        return;
    }
}

void AAGSDCharacter::SteelPrimeSet()
{
    if (steelp1 && steelp2 && steelp3)
    {
        //세트 달성
        Attack += 2.0f;
        Defense += 10.0f;
        SpeedLevel += 100.0f;
        AttackSpeedLevel += 0.2f;
        MaxHealth += 10;
        CurrentHealth += 10;
    }
    else
    {
        //세트옵션 미달성
        UE_LOG(LogTemp, Warning, TEXT("SteelPrimeSet 미달성 - 1: %s, 2: %s, 3: %s"),
            steelp1 ? TEXT("true") : TEXT("false"),
            steelp2 ? TEXT("true") : TEXT("false"),
            steelp3 ? TEXT("true") : TEXT("false"));
        return;
    }
}

void AAGSDCharacter::NuclearSet()
{
    if (dna && nuclear)
    {
        //세트 완성
        FName RowName(TEXT("NewRow_83"));

        PlayingGetAccessoryRowName(RowName);
        AcquiredAccessories.Add(RowName);

        if (UAGSDGameInstance* GameInstance = Cast<UAGSDGameInstance>(GetWorld()->GetGameInstance()))
        {
            if (!GameInstance->TempAccessory.Contains(RowName))
                GameInstance->TempAccessory.Add(RowName);
        }
        AttackRangeLevel += 3.0f;
        Attack += 2.0f;
        SpeedLevel += 50.0f;
    }
    else
    {
        //세트 미달성
        UE_LOG(LogTemp, Warning, TEXT("NuclearSet 미달성 - dna: %s, nuclear: %s"),
            dna ? TEXT("true") : TEXT("false"),
            nuclear ? TEXT("true") : TEXT("false"));
        return;
    }
}

void AAGSDCharacter::PrimeZ()
{
    // 세트 미완성 시 실행 안 함
    if (!(steelp1 && steelp2 && steelp3)) return;

    // 쿨타임 중이면 실행 안 함
    if (GetWorldTimerManager().IsTimerActive(PrimeZCooldownHandle))
    {
        UE_LOG(LogTemp, Warning, TEXT("PrimeZ 사용 불가: 쿨타임 진행 중"));
        return;
    }

    // 무적 시작
    bIsInvincible = true;
    UE_LOG(LogTemp, Log, TEXT("PrimeZ 발동 - 무적 시작"));

    // 3초 뒤 무적 해제
    GetWorldTimerManager().SetTimer(PrimeZDurationHandle, this, &AAGSDCharacter::EndPrimeZ, 3.0f, false);

    // 30초 쿨타임 시작
    GetWorldTimerManager().SetTimer(PrimeZCooldownHandle, this, &AAGSDCharacter::ResetPrimeZCooldown, PrimeZCooldownTotal, false);

    // UI 즉시 1으로(막 썼음)
    if (DashCooldownWidget) DashCooldownWidget->UpdatePrimeZCooldown(1.0f);
    GetWorldTimerManager().SetTimer(PrimeZTickHandle, this, &AAGSDCharacter::TickPrimeZCooldownUI, 0.05f, true);

}
void AAGSDCharacter::EndPrimeZ()
{
    bIsInvincible = false;
    UE_LOG(LogTemp, Log, TEXT("PrimeZ 종료 - 무적 해제"));
}

void AAGSDCharacter::ResetPrimeZCooldown()
{
    UE_LOG(LogTemp, Log, TEXT("PrimeZ 쿨타임 종료 - 다시 사용 가능"));
    // 마지막으로 0.0 보정 & 틱 타이머 정리
    if (DashCooldownWidget) DashCooldownWidget->UpdatePrimeZCooldown(0.0f);
    GetWorldTimerManager().ClearTimer(PrimeZTickHandle);
}

void AAGSDCharacter::PrimeX()
{
    // 세트 미완성 시 미실행
    if (!(steelp1 && steelp2 && steelp3)) {
        UE_LOG(LogTemp, Warning, TEXT("[PrimeX] 조건 미달 - steelp1=%d, steelp2=%d, steelp3=%d"),
            steelp1, steelp2, steelp3);
        return;
    }

    // 쿨타임 중이면 미실행
    if (GetWorldTimerManager().IsTimerActive(PrimeXCooldownHandle))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PrimeX] 쿨타임 중 - 남은시간: %.2fs"),
            GetWorldTimerManager().GetTimerRemaining(PrimeXCooldownHandle));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[PrimeX] 즉시 폭발 발동"));

    // 즉시 폭발 로직 (기존Disposableitem.cpp OnOverlapBegin에서 복사)
    TArray<AActor*> AllEnemys;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy1Class::StaticClass(), AllEnemys);

    if (AllEnemys.Num() > 0)
    {
        for (int32 i = 0; i < AllEnemys.Num(); i++)
        {
            AEnemy1Class* Enemy = Cast<AEnemy1Class>(AllEnemys[i]);
            if (Enemy)
            {
                AEnemy1AIController* AIC = Cast<AEnemy1AIController>(Enemy->GetController());
                if (AIC)
                {
                    // 캐릭터 위치 기준 거리 체크(폭발범위 3000.0f)
                    if (FVector::Dist(GetActorLocation(), Enemy->GetActorLocation()) <= 3000.0f)
                    {
                        AIC->Attacked(Attack * 3); // 3배 공격력
                    }
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Enemy do not exist."));
    }
    const FVector ExplosionCenter = GetActorLocation();
    SpawnExplosionFX(ExplosionCenter, /*bSnapToGround=*/true);

    // 20초 쿨타임 시작
    GetWorldTimerManager().SetTimer(PrimeXCooldownHandle, this, &AAGSDCharacter::ResetPrimeXCooldown, PrimeXCooldownTotal, false);

    // UI 갱신
    if (DashCooldownWidget) DashCooldownWidget->UpdatePrimeXCooldown(1.0f);
    GetWorldTimerManager().SetTimer(PrimeXTickHandle, this, &AAGSDCharacter::TickPrimeXCooldownUI, 0.05f, true);
}


void AAGSDCharacter::ResetPrimeXCooldown()
{
    UE_LOG(LogTemp, Log, TEXT("PrimeX 쿨타임 종료 - 다시 사용 가능"));
    if (DashCooldownWidget) DashCooldownWidget->UpdatePrimeXCooldown(0.0f);
    GetWorldTimerManager().ClearTimer(PrimeXTickHandle);
}

float AAGSDCharacter::GetPrimeZCooldownPercent() const
{
    // 0 = 준비완료, 1 = 막 사용
    if (!GetWorldTimerManager().IsTimerActive(PrimeZCooldownHandle)) return 0.0f;

    const float Remaining = GetWorldTimerManager().GetTimerRemaining(PrimeZCooldownHandle);
    const float Total = FMath::Max(0.01f, PrimeZCooldownTotal);
    return 0.0f + (Remaining / Total);
}

float AAGSDCharacter::GetPrimeXCooldownPercent() const
{
    if (!GetWorldTimerManager().IsTimerActive(PrimeXCooldownHandle)) return 0.0f;

    const float Remaining = GetWorldTimerManager().GetTimerRemaining(PrimeXCooldownHandle);
    const float Total = FMath::Max(0.01f, PrimeXCooldownTotal);
    return 0.0f + (Remaining / Total);
}

void AAGSDCharacter::TickPrimeZCooldownUI()
{
    if (DashCooldownWidget)
    {
        DashCooldownWidget->UpdatePrimeZCooldown(GetPrimeZCooldownPercent());
    }
}

void AAGSDCharacter::TickPrimeXCooldownUI()
{
    if (DashCooldownWidget)
    {
        DashCooldownWidget->UpdatePrimeXCooldown(GetPrimeXCooldownPercent());
    }
}

void AAGSDCharacter::SpawnExplosionFX(const FVector& Center, bool bSnapToGround)
{
    if (!BombEffect)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PrimeX] 이펙트 미지정: BombEffect를 에디터에서 설정하세요."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    // 스폰 위치: 기본은 Center. 필요 시 지면에 스냅
    FVector SpawnLoc = Center;
    if (bSnapToGround)
    {
        FHitResult Hit;
        FVector Start = Center + FVector(0, 0, 150);
        FVector End = Center - FVector(0, 0, 1000);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(PrimeX_EffectTrace), false, this);
        if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            SpawnLoc = Hit.ImpactPoint;
        }
    }

    FActorSpawnParameters SParams;
    SParams.Owner = this;
    SParams.Instigator = this;
    SParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 회전은 정면이 필요 없으면 ZeroRotator, 아니면 캐릭터 기준
    const FRotator SpawnRot = FRotator::ZeroRotator;

    AActor* FX = World->SpawnActor<AActor>(BombEffect, SpawnLoc, SpawnRot, SParams);
    if (FX)
    {
        UE_LOG(LogTemp, Log, TEXT("[PrimeX] 폭발 이펙트 스폰: %s @ %s"), *GetNameSafe(FX), *SpawnLoc.ToCompactString());
        if (BombEffectLifeSpan > 0.f)
        {
            FX->SetLifeSpan(BombEffectLifeSpan);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[PrimeX] 폭발 이펙트 스폰 실패 (클래스/콜리전 확인)"));
    }
}

void AAGSDCharacter::SpawnBuffVFX(EBuffType BuffType, float Duration)
{
    
    switch (BuffType)
    {
    case EBuffType::Revive:
        if (ReviveParticle) {
            ApplyVFX = ReviveParticle;
        }
        break;
    case EBuffType::Seven:
        if (SevenParticle) {
            ApplyVFX = SevenParticle;
            SevenParticleID = 7;
        }
        break;
    case EBuffType::Star:
        if (StarParticle) {
            ApplyVFX = StarParticle;
        }
        break;
    default:
        break;
    }

    if (ApplyVFX) {
        UNiagaraComponent* VFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ApplyVFX,GetMesh(),NAME_None,FVector::ZeroVector,FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset,
            true);
        VFXComp->RegisterComponent();
        
        VFXComp->AttachToComponent(RootComponent, FAttachmentTransformRules(
            EAttachmentRule::KeepRelative,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepRelative,
            false));
        switch (BuffType)
        {
        case EBuffType::Revive:
            VFXComp->ComponentTags.Add("Revive");
            break;
        case EBuffType::Seven:
            VFXComp->ComponentTags.Add("Seven");
            break;
        case EBuffType::Star:
            VFXComp->ComponentTags.Add("Star");
            break;
        default:
            break;
        }
        
        VFXComp->SetAsset(ApplyVFX);
        VFXComp->SetAbsolute(false, true, false);
        VFXComp->Activate();
    }
}

ATrapChest* AAGSDCharacter::FindNearbyTrapChest() const
{
    TArray<AActor*> OverlappingActors;
    GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, ATrapChest::StaticClass());

    ATrapChest* Best = nullptr;
    float BestDistSq = MAX_flt;

    for (AActor* A : OverlappingActors)
    {
        if (ATrapChest* Chest = Cast<ATrapChest>(A))
        {
            const float DistSq = FVector::DistSquared(Chest->GetActorLocation(), GetActorLocation());
            if (DistSq < BestDistSq)
            {
                Best = Chest;
                BestDistSq = DistSq;
            }
        }
    }
    return Best;
}

void AAGSDCharacter::TestCameraMove(AActor* MoveToCamera)
{

}

void AAGSDCharacter::ApplyGameAndUICursorMode()
{
    if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
    {
        // 커서를 캡처 중에도 숨기지 않음 + 뷰포트 잠금 해제
        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        // 포커스 줄 위젯이 있으면 여기서 지정 가능: Mode.SetWidgetToFocus(SomeWidget->TakeWidget());

        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }
}
