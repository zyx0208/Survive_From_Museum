// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Hammer_Upgrade.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Hammer_Upgrade : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Hammer_Upgrade();
    virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void Tick(float DeltaTime) override;
};
