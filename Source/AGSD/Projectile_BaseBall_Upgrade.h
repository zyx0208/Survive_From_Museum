// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_BaseBall_Upgrade.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_BaseBall_Upgrade : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_BaseBall_Upgrade();
    virtual void Tick(float deltaTime) override;
    virtual void WeaponHitEffect(AActor* OtherActor) override;
};
