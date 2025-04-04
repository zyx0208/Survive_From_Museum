// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Bow.h"

AProjectile_Bow::AProjectile_Bow()
{
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;
}

void AProjectile_Bow::WeaponHitEffect(AActor* OtherActor)
{
}

void AProjectile_Bow::BeginPlay()
{
    Super::BeginPlay();
}
