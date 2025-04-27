// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Jelda_Upgrade.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Jelda_Upgrade : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Jelda_Upgrade();
    virtual void Tick(float deltaTime) override;
    virtual void WeaponHitEffect(AActor* OtherActor) override;
};
