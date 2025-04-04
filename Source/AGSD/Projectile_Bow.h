// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Bow.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Bow : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Bow();
    virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void BeginPlay() override;
};
