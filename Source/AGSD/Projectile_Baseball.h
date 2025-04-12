// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Baseball.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Baseball : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Baseball();
    virtual void Tick(float delta) override;
    virtual void WeaponHitEffect(AActor* OtherActor) override;
};
