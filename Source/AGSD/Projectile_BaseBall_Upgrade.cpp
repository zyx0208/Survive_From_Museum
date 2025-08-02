// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_BaseBall_Upgrade.h"

AProjectile_BaseBall_Upgrade::AProjectile_BaseBall_Upgrade()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 6.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_BaseBall_Upgrade::Tick(float deltaTime)
{
}

void AProjectile_BaseBall_Upgrade::WeaponHitEffect(AActor* OtherActor)
{
}
