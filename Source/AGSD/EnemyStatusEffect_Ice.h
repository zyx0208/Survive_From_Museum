// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyStatusEffect.h"
#include "Sound/SoundCue.h"
#include "EnemyStatusEffect_Ice.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AEnemyStatusEffect_Ice : public AEnemyStatusEffect
{
	GENERATED_BODY()
public:

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //초당 효과
    virtual void TimePerEffect() override;

    AEnemyStatusEffect_Ice();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float IceDamage = 1.0f;

    //무기 공격 사운드
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundCue* EffectSoundCue;
};
