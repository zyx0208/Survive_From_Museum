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

    // 초기화
    bIsConsumed = false;

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
}

// Called when the game starts or when spawned
void AXPOrb::BeginPlay()
{
	Super::BeginPlay();

    // OrbMesh의 Dynamic Material Instance 생성
    UMaterialInstanceDynamic* DynamicMaterial = OrbMesh->CreateDynamicMaterialInstance(0);
    if (DynamicMaterial)
    {
        OrbMesh->SetMaterial(0, DynamicMaterial);
        MaterialInstance = DynamicMaterial; // 멤버 변수에 저장
    }

    // XP 값에 따라 크기와 색상 설정
    AdjustScaleBasedOnXP();
}

// Called every frame
void AXPOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AXPOrb::SetRandomXP()
{
    MagnetStrength = 500.0f;
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
    FLinearColor NewColor;

    switch (XPValue)
    {
    case 2:
        NewColor = FLinearColor::Green;
        break;
    case 4:
        NewColor = FLinearColor::Blue;
        break;
    case 6:
        NewColor = FLinearColor::Red;
        break;
    default:
        NewColor = FLinearColor::White;
        break;
    }

    // 색상 변경 (Dynamic Material Instance가 유효한 경우)
    if (MaterialInstance)
    {
        MaterialInstance->SetVectorParameterValue(TEXT("BaseColor"), NewColor);
    }
}

void AXPOrb::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsConsumed) return;

    AAGSDCharacter* Character = Cast<AAGSDCharacter>(OtherActor);
    if (Character)
    {
        bIsConsumed = true;

        // 충돌 비활성화
        SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // 캐릭터에 등록하여 끌어당기기 상태로 설정
        Character->AddToMagnetField(this);
    }
}

