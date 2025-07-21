// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_ChargeBeam.h"

AProjectile_ChargeBeam::AProjectile_ChargeBeam()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 10.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_ChargeBeam::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AProjectile_ChargeBeam::WeaponHitEffect(AActor* OtherActor)
{
}
