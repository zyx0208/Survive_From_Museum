// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Beta_Red.h"

AProjectile_Beta_Red::AProjectile_Beta_Red()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 5.0f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;
}

void AProjectile_Beta_Red::WeaponHitEffect()
{
    // 추가적인 기능을 구현
    UE_LOG(LogTemp, Warning, TEXT("RedHit"));
}
