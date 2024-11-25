// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Beta.h"
#include "Projectile_Sub.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AProjectile_Sub : public AProjectile_Beta
{
	GENERATED_BODY()
	
    AProjectile_Sub();
    virtual void BeginPlay() override;
    };
