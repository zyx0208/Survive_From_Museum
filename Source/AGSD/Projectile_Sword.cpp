// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Sword.h"

AProjectile_Sword::AProjectile_Sword()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 0.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
    
}

void AProjectile_Sword::WeaponHitEffect(AActor* OtherActor)
{
}
void AProjectile_Sword::Tick(float DeltaTime)
{
    Super::Super::Tick(DeltaTime);
    // 현재 위치와 시작 위치의 거리를 계산
    float DistanceTraveled = FVector::Dist(StartLocation, GetActorLocation());
    
}