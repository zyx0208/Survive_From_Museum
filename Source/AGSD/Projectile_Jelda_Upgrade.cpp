// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Jelda_Upgrade.h"

AProjectile_Jelda_Upgrade::AProjectile_Jelda_Upgrade()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_Jelda_Upgrade::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AProjectile_Jelda_Upgrade::WeaponHitEffect(AActor* OtherActor)
{

}
