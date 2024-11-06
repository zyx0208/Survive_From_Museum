// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Beta_Red.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Beta_Red : public AProjectile_Beta
{
	GENERATED_BODY()
public:
	// »ý¼ºÀÚ
	AProjectile_Beta_Red();

public:
	virtual void WeaponHitEffect() override;
};
