// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Bow_Upgrade.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Bow_Upgrade : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Bow_Upgrade();
    virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void Tick(float timedelta) override;
};
