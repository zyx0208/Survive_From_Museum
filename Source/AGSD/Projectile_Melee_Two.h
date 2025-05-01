// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Melee_Two.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Melee_Two : public AProjectile_Beta
{
	GENERATED_BODY()
    AProjectile_Melee_Two();
    TArray<AActor*> DamagedActor;
    virtual void Tick(float deltaTime) override;
    virtual void WeaponHitEffect(AActor* OtherActor) override;
    virtual void Damage(AActor* OtherActor) override;
    
};
