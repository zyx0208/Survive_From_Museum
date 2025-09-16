#include "TrapChest.h"
#include "AGSDCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"      
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"   
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

// Sets default values
ATrapChest::ATrapChest()
{
    PrimaryActorTick.bCanEverTick = false;   

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    InteractionArea = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionArea"));
    InteractionArea->SetupAttachment(Root);
    InteractionArea->InitSphereRadius(InteractionRadius);
    InteractionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionArea->SetCollisionObjectType(ECC_WorldDynamic);
    InteractionArea->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    InteractionArea->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ATrapChest::BeginPlay()
{
    Super::BeginPlay();

    if (DoorActor)
    {
        SetActorActiveWithCollision(DoorActor, false);  // 시작은 비활성화
        UE_LOG(LogTemp, Log, TEXT("[TrapChest] BeginPlay: DoorActor set to inactive: %s"),
            *DoorActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[TrapChest] BeginPlay: DoorActor is NULL. (에디터에서 연결 필요)"));
    }
}

void ATrapChest::SetActorActiveWithCollision(AActor* Target, bool bActive)
{
    if (!Target) return;

    Target->SetActorHiddenInGame(!bActive);
    Target->SetActorEnableCollision(bActive);

    TInlineComponentArray<UPrimitiveComponent*> PrimComps(Target);
    for (UPrimitiveComponent* Prim : PrimComps)
    {
        Prim->SetHiddenInGame(!bActive);
        Prim->SetVisibility(bActive, true);

        if (bEnableDoorCollisionOnTrap)
        {
            Prim->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics
                : ECollisionEnabled::NoCollision);
        }
    }
}

void ATrapChest::Interact(AAGSDCharacter* Character)
{
    if (!Character) return;

    // 이미 사용되었으면 더 이상 반응하지 않음 (상자는 화면에 그대로)
    if (bUsed)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[TrapChest] Interact ignored: already used."));
        return;
    }

    bUsed = true;

    // 상자는 ‘보이게 유지’, 대신 InteractionArea만 끄고 재사용 차단
    if (InteractionArea)
    {
        InteractionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        InteractionArea->SetGenerateOverlapEvents(false);
    }

    const float Roll = FMath::FRand();            // 0.0 ~ 1.0
    const bool bLevelUp = (Roll <= LevelUpProbability); // 30% 기본

    UE_LOG(LogTemp, Log, TEXT("[TrapChest] Interact: Roll=%.3f (<=%.2f -> LevelUp:%s)"),
        Roll, LevelUpProbability, bLevelUp ? TEXT("YES") : TEXT("NO"));

    if (bLevelUp)
    {
        // 현재 XP 유지 + 정확히 1레벨만 상승
        Character->AddXP(Character->XPToNextLevel);

        return;
    }

    // 70%: 문 활성화 → TrapDuration(기본 20초) 후 Destroy()
    if (DoorActor)
    {
        SetActorActiveWithCollision(DoorActor, true);
        UE_LOG(LogTemp, Log, TEXT("[TrapChest] DoorActor ACTIVATED: %s"), *DoorActor->GetName());

        PendingDoor = DoorActor;
        DoorEndTimeSeconds = GetWorld()->GetTimeSeconds() + TrapDuration;

        if (EmergencySound)
        {
            // 이전 재생분이 남아있다면 정리
            if (EmergencyAudioComp && EmergencyAudioComp->IsPlaying())
            {
                EmergencyAudioComp->Stop();
            }

            // 루트에 붙여 3D로 재생
            EmergencyAudioComp = UGameplayStatics::SpawnSoundAttached(
                EmergencySound,
                GetRootComponent(),          // AttachComponent
                NAME_None,                   // AttachPointName
                FVector::ZeroVector,         // Location
                EAttachLocation::KeepRelativeOffset,
                false,                       // bStopWhenAttachedToDestroyed (수동으로 stop)
                1.f,                         // VolumeMultiplier
                1.f                          // PitchMultiplier
            );

            if (EmergencyAudioComp)
            {
                EmergencyAudioComp->bAutoDestroy = false; // Trap 끝에서 수동 정리
                // 필요하면 부드럽게 시작
                EmergencyAudioComp->FadeIn(0.15f);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[TrapChest] Failed to spawn EmergencyAudioComp."));
            }
        }

        if (!TimerWidgetInstance && TimerWidgetClass)
        {
            if (Character)
            {
                if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
                {
                    // 멀티플레이/리플리케이션 환경에서 "해당 로컬 클라이언트"에서만 UI 생성
                    if (PC->IsLocalController())
                    {
                        TimerWidgetInstance = CreateWidget<UTrapTimerWidget>(PC, TimerWidgetClass);
                        if (TimerWidgetInstance)
                        {
                            // 화면에 추가
                            TimerWidgetInstance->AddToViewport();
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[TrapChest] Failed to create TimerWidgetInstance (PC=%s)"),
                                *PC->GetName());
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Verbose, TEXT("[TrapChest] PC is not local, skip creating UI."));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[TrapChest] Character has no PlayerController."));
                }
            }
        }
        UpdateDoorCountdown();

        TrapSpawnMonster();

        GetWorldTimerManager().SetTimer(
            DoorTickHandle,
            this,
            &ATrapChest::UpdateDoorCountdown,
            0.1f,
            true
        );

        GetWorldTimerManager().SetTimer(
            DoorDestroyHandle,
            FTimerDelegate::CreateLambda([this]()
                {
                    // 파괴 직전, UI 제거
                    if (TimerWidgetInstance)
                    {
                        TimerWidgetInstance->RemoveFromParent();
                        TimerWidgetInstance = nullptr;
                    }
                    if (EmergencyAudioComp)
                    {
                        // 부드럽게 페이드아웃 후 파괴
                        EmergencyAudioComp->FadeOut(0.2f, 0.f);
                        EmergencyAudioComp = nullptr;
                    }
                    if (PendingDoor.IsValid())
                    {
                        UE_LOG(LogTemp, Log, TEXT("[TrapChest] DoorActor DESTROY: %s"), *PendingDoor->GetName());
                        PendingDoor->Destroy();
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[TrapChest] DoorActor destroy skipped (no longer valid)."));
                    }

                    // 갱신 타이머도 정리
                    GetWorldTimerManager().ClearTimer(DoorTickHandle);
                    PendingDoor.Reset();
                }),
            TrapDuration,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[TrapChest] Interact: DoorActor is NULL — 에디터에서 문을 연결하세요."));
    }
}

void ATrapChest::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (InteractionArea)
    {
        InteractionArea->SetSphereRadius(InteractionRadius, true);
    }
}

void ATrapChest::UpdateDoorCountdown()
{
    if (!TimerWidgetInstance)
    {
        // 위젯이 없다면 갱신 중단
        GetWorldTimerManager().ClearTimer(DoorTickHandle);
        return;
    }

    const float Now = GetWorld()->GetTimeSeconds();
    float Remaining = DoorEndTimeSeconds - Now;

    // 올림하여 "3.9 -> 4초" 식으로 표기
    int32 Seconds = FMath::Max(0, FMath::CeilToInt(Remaining));

    TimerWidgetInstance->SetSeconds(Seconds);

    if (Remaining <= 0.f)
    {
        // 안전장치: 파괴 타이머 콜백에서 위젯 제거가 이뤄지지만,
        // 여기서도 제거 시도하여 UI 잔존 방지
        TimerWidgetInstance->RemoveFromParent();
        TimerWidgetInstance = nullptr;
        GetWorldTimerManager().ClearTimer(DoorTickHandle);
    }
}
void ATrapChest::TrapSpawnMonster()
{
    if (TrapMonster)
    {
        GetWorld()->SpawnActor<AActor>(TrapMonster,
            GetActorLocation(),
            FRotator::ZeroRotator);
    }
}
