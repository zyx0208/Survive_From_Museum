// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubWeapon.h"
#include "SubWeapon_Boomerang.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API ASubWeapon_Boomerang : public ASubWeapon
{
	GENERATED_BODY()

public:
    ASubWeapon_Boomerang();

    virtual void BeginPlay() override
    {
        Super::BeginPlay();

        CreateProjectile();
    }

    virtual void Tick(float DeltaTime) override;

    virtual void StartFiring() override;

    virtual void CreateProjectile() override;

    virtual void Fire() override;

    FRotator RotationSpeed;

    FRotator CurrentRotation;

    bool OnProjectile = false;

    virtual void UpdatePlayerStat() override;
};
