// Fill out your copyright notice in the Description page of Project Settings.


#include "XPOrb.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AGSDCharacter.h"  // 캐릭터 클래스 헤더 포함

// Sets default values
AXPOrb::AXPOrb()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // 구슬에 충돌 컴포넌트 추가
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->InitSphereRadius(50.0f);  // 충돌 범위 설정
    RootComponent = SphereComponent;

    // 구체 오브젝트 초기화
    OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
    OrbMesh->SetupAttachment(SphereComponent); // SphereComponent의 자식으로 설정

    // 충돌 이벤트 바인딩
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AXPOrb::OnOverlapBegin);

    // 경험치와 크기를 설정
    SetRandomXP();
    AdjustScaleBasedOnXP();
}

// Called when the game starts or when spawned
void AXPOrb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AXPOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AXPOrb::SetRandomXP()
{
    // 랜덤 값에 따라 경험치 설정 (60%: 2, 30%: 4, 10%: 6)
    float RandomValue = FMath::FRand(); // 0.0f ~ 1.0f 사이의 랜덤 값
    if (RandomValue <= 0.6f)
    {
        XPValue = 2;
    }
    else if (RandomValue <= 0.9f) // 0.6f < RandomValue <= 0.9f
    {
        XPValue = 4;
    }
    else
    {
        XPValue = 6; // 0.9f < RandomValue <= 1.0f
    }
}

void AXPOrb::AdjustScaleBasedOnXP()
{
    FVector NewScale;

    // 경험치량에 따른 크기 조절
    if (XPValue == 2)
    {
        NewScale = FVector(0.5f);
    }
    else if (XPValue == 4)
    {
        NewScale = FVector(1.0f);
    }
    else if (XPValue == 6)
    {
        NewScale = FVector(5.0f);
    }

    // OrbMesh에만 크기 적용
    OrbMesh->SetWorldScale3D(NewScale);
}

void AXPOrb::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 캐릭터와 충돌했는지 확인
    AAGSDCharacter* Character = Cast<AAGSDCharacter>(OtherActor);
    if (Character)
    {
        // 캐릭터에 XP 추가
        Character->AddXP(XPValue);

        // 구슬을 파괴 (원한다면)
        Destroy();
    }
}

