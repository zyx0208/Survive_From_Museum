// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Beta_Blue.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Beta_Blue : public AProjectile_Beta
{
	GENERATED_BODY()
public:
	// 생성자
	AProjectile_Beta_Blue();

public:
	virtual void WeaponHitEffect(AActor* OtherActor) override;
};
