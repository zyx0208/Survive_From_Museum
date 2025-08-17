#include "TrapChest.h"
#include "AGSDCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"      
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"   

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

        GetWorldTimerManager().SetTimer(
            DoorDestroyHandle,
            FTimerDelegate::CreateLambda([this]()
                {
                    if (PendingDoor.IsValid())
                    {
                        UE_LOG(LogTemp, Log, TEXT("[TrapChest] DoorActor DESTROY: %s"),
                            *PendingDoor->GetName());
                        PendingDoor->Destroy();
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[TrapChest] DoorActor destroy skipped (no longer valid)."));
                    }
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
