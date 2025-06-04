// Fill out your copyright notice in the Description page of Project Settings.


#include "HPDashUIActor.h"
#include "Components/WidgetComponent.h"
#include "HPandDashUI.h"

// Sets default values
AHPDashUIActor::AHPDashUIActor()
{
    PrimaryActorTick.bCanEverTick = true;

    HPandDashWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPandDashWidget"));
    RootComponent = HPandDashWidget;

    HPandDashWidget->SetWidgetSpace(EWidgetSpace::World);
    HPandDashWidget->SetDrawAtDesiredSize(true);
    HPandDashWidget->SetDrawSize(FVector2D(64.f, 64.f));
}

// Called when the game starts or when spawned
void AHPDashUIActor::BeginPlay()
{
	Super::BeginPlay();

    // 고정된 회전 방향
    HPandDashWidget->SetWorldRotation(FRotator(70.f, 180.f, 0.f)); // 탑뷰일 경우 보통 90도
}

// Called every frame
void AHPDashUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (TargetActor)
    {
        FVector TargetLocation = TargetActor->GetActorLocation() + FVector(-50.f, 0.f, -80.f); // 캐릭터 아래
        SetActorLocation(TargetLocation);

        // 회전은 고정
        SetActorRotation(FRotator(70.f, 180.f, 0.f));
    }
}

void AHPDashUIActor::Initialize(AActor* FollowTarget)
{
    TargetActor = FollowTarget;
}
