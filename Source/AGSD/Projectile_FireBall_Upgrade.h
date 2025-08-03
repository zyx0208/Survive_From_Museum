// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "EnemyStatusEffect.h"
#include "EnemyStatusEffect_Ice.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Projectile_FireBall_Upgrade.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_FireBall_Upgrade : public AProjectile_Beta
{
	GENERATED_BODY()
public:
    AProjectile_FireBall_Upgrade();
    virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatusEffect")
    TSubclassOf<AEnemyStatusEffect_Ice> IceEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireDamage = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraSystem* IceVFX;
};
