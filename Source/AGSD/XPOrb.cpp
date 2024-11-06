// Fill out your copyright notice in the Description page of Project Settings.


#include "XPOrb.h"
#include "Components/SphereComponent.h"
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

    // 충돌 이벤트 바인딩
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AXPOrb::OnOverlapBegin);
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

void AXPOrb::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 캐릭터와 충돌했는지 확인
    AAGSDCharacter* Character = Cast<AAGSDCharacter>(OtherActor);
    if (Character)
    {
        // 캐릭터에 XP 추가
        Character->AddXP(10);

        // 구슬을 파괴 (원한다면)
        Destroy();
    }
}

