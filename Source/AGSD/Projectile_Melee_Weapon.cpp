// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Melee_Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AProjectile_Melee_Weapon::AProjectile_Melee_Weapon()
{
	// 투사체 속도, 데미지, 사거리
	ProjectileSpeed = 1.0f;
	ProjectileDamage = 8.0f;
	ProjectileRange = 100.0f;
}

void AProjectile_Melee_Weapon::WeaponHitEffect()
{
	// 추가적인 기능을 구현
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("MeleeHit")));
}

void AProjectile_Melee_Weapon::Tick(float DeltaTime)
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
