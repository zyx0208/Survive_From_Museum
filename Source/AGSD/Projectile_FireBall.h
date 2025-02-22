// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "EnemyStatusEffect.h"
#include "EnemyStatusEffect_Fire.h"
#include "Projectile_FireBall.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_FireBall : public AProjectile_Beta
{
	GENERATED_BODY()
public:
    AProjectile_FireBall();
    virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
    TSubclassOf<AEnemyStatusEffect_Fire> FireEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireDamage = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 5.0f;
};
