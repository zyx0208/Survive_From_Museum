// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Baseball.h"

AProjectile_Baseball::AProjectile_Baseball() {
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 0.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_Baseball::Tick(float delta)
{

}

void AProjectile_Baseball::WeaponHitEffect(AActor* OtherActor)
{
    Destroy();
}
