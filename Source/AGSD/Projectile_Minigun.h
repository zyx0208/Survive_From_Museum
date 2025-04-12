// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Minigun.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Minigun : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Minigun();
    virtual void Tick(float deltaTime) override;
    virtual void WeaponHitEffect(AActor* OtherActor) override;
};
