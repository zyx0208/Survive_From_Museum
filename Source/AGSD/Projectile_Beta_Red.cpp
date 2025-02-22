// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Beta_Red.h"

AProjectile_Beta_Red::AProjectile_Beta_Red()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;
}

void AProjectile_Beta_Red::WeaponHitEffect(AActor* OtherActor)
{
    // 추가적인 기능을 구현
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("RedHit")));
    // 발사체를 파괴 (필요시)
    Destroy();
}
