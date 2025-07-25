// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDrop3.h"

AWeaponDrop3::AWeaponDrop3()
{
    
}

void AWeaponDrop3::BeginPlay()
{
    Super::BeginPlay();
    WeaponIDInt = FMath::RandRange(MinID, MaxID);
    WeaponID = FString::FromInt(WeaponIDInt);
    SetMesh();
}
