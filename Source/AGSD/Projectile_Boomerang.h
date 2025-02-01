// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Boomerang.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Boomerang : public AProjectile_Beta
{
	GENERATED_BODY()
public:
    AProjectile_Boomerang();
    virtual void WeaponHitEffect() override;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    float Radius;
    float CurrentAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ACharacter* TargetCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator RotationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator CurrentRotation;

    virtual void UpdatePlayerStat() override;
};
