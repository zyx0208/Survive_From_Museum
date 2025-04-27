// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Bow_Upgrade.h"

AProjectile_Bow_Upgrade::AProjectile_Bow_Upgrade()
{
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;
}

void AProjectile_Bow_Upgrade::WeaponHitEffect(AActor* OtherActor)
{
    Destroy();
}

void AProjectile_Bow_Upgrade::Tick(float timedelta)
{
}