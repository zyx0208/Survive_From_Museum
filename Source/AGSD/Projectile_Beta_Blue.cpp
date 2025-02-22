// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Beta_Blue.h"

AProjectile_Beta_Blue::AProjectile_Beta_Blue()
{
	// 투사체 속도, 데미지, 사거리
	ProjectileSpeed = 7.0f;
	ProjectileDamage = 4.0f;
    ProjectileRange = 500.0f;
}

void AProjectile_Beta_Blue::WeaponHitEffect(AActor* OtherActor)
{
	// 추가적인 기능을 구현
	UE_LOG(LogTemp, Warning, TEXT("BlueHit"));
	// 발사체를 파괴 (필요시)
	Destroy();
}
