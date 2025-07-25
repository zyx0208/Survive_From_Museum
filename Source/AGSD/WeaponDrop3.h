// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponDrop.h"
#include "WeaponDrop3.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AWeaponDrop3 : public AWeaponDrop
{
	GENERATED_BODY()
    AWeaponDrop3();
    virtual void BeginPlay() override;
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int MinID;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int MaxID;
};
