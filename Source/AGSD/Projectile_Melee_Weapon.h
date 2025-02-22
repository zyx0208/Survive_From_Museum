// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Melee_Weapon.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Melee_Weapon : public AProjectile_Beta
{
	GENERATED_BODY()
public:
	AProjectile_Melee_Weapon();
	virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void Tick(float DeltaTime) override;
};
