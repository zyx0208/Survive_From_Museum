// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Minigun.h"

AProjectile_Minigun::AProjectile_Minigun()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 2000.0f;
}

void AProjectile_Minigun::Tick(float deltaTime)
{
}

void AProjectile_Minigun::WeaponHitEffect(AActor* OtherActor)
{
    Destroy();
}
