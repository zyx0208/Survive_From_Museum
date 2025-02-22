// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameTimer.h"
#include "EnemyStatusEffect.generated.h"

UCLASS()
class AGSD_API AEnemyStatusEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyStatusEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    
    //초당 효과
    virtual void TimePerEffect();

    //상시적용 효과
    virtual void AlwaysEffect();

    //끝날 때 발동되는 효과
    virtual void EndEffect();

    //지속시간 타이머
    FTimerHandle EffectTimer;

    //지속시간
    float Duration=5.0f;

    //남은시간
    float RemainTime=999.9f;
    
    //액터가 파괴될 때 호출되는 이벤트
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void TimeUpdate();
};
