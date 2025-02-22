// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Jelda.h"

AProjectile_Jelda::AProjectile_Jelda()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;

}

void AProjectile_Jelda::WeaponHitEffect(AActor* OtherActor)
{
}
void AProjectile_Jelda::Tick(float DeltaTime)
{
    Super::Super::Tick(DeltaTime);
    // 현재 위치와 시작 위치의 거리를 계산
    float DistanceTraveled = FVector::Dist(StartLocation, GetActorLocation());

    // 특정 거리 이상 이동하면 파괴
    if (DistanceTraveled > ProjectileRange)
    {
        Destroy();
    }
}