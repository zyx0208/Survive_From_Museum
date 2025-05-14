// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseIndicatorActor.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AMouseIndicatorActor::AMouseIndicatorActor()
{
    PrimaryActorTick.bCanEverTick = false;

    DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
    RootComponent = DecalComponent;

    // 디폴트 Decal 세팅
    /*static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterial(TEXT("/Game/DecalMaterials/M_MouseCursor")); // 너가 만든 머티리얼 경로로 교체
    if (DecalMaterial.Succeeded())
    {
        DecalComponent->SetDecalMaterial(DecalMaterial.Object);
    }

    DecalComponent->DecalSize = FVector(64.f, 128.f, 128.f); // (Thickness, X Size, Y Size)
    DecalComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // 바닥에 똑바로 누워있게

    // 충돌은 Box로 대체
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(DecalComponent);
    CollisionBox->SetBoxExtent(FVector(50.f, 50.f, 10.f)); // 커서 범위 크기 조정
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본은 꺼짐
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore); // 기본 무시
    CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // 예: 커서만 체크하는 채널*/
}


void AMouseIndicatorActor::SetDecalCollision(ECollisionEnabled::Type Mouse)
{
    if (CollisionBox)
    {
        CollisionBox->SetCollisionEnabled(Mouse);
    }
}
