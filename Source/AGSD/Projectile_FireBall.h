// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
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

    virtual void WeaponHitEffect() override;

};
