// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_FireBall.h"

AProjectile_FireBall::AProjectile_FireBall()
{
    ProjectileSpeed = 0.2f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;
}

void AProjectile_FireBall::WeaponHitEffect()
{
    Destroy();
}
