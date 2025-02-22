// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatusEffect.h"

// Sets default values
AEnemyStatusEffect::AEnemyStatusEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyStatusEffect::BeginPlay()
{
	Super::BeginPlay();

    SetLifeSpan(Duration);
    GetWorldTimerManager().SetTimer(EffectTimer, this, &AEnemyStatusEffect::TimePerEffect ,1.0f, true);
}

void AEnemyStatusEffect::TimePerEffect()
{
    //GetWorldTimerManager().SetTimer()
    UE_LOG(LogTemp, Display, TEXT("초당 효과 적용중"));
}

void AEnemyStatusEffect::AlwaysEffect()
{
}

void AEnemyStatusEffect::EndEffect()
{
    UE_LOG(LogTemp, Display, TEXT("종료시 효과 적용"));
}

void AEnemyStatusEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    EndEffect();
    GetWorldTimerManager().ClearTimer(EffectTimer);
}

// Called every frame
void AEnemyStatusEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    RemainTime = GetLifeSpan();
}

void AEnemyStatusEffect::TimeUpdate()
{
    if (this) {
        SetLifeSpan(Duration);
        UE_LOG(LogTemp, Display, TEXT("시간 업데이트 성공"));
    }
}


