// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Sword_Upgrade.h"

AProjectile_Sword_Upgrade::AProjectile_Sword_Upgrade()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_Sword_Upgrade::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AProjectile_Sword_Upgrade::WeaponHitEffect(AActor* OtherActor)
{

}
