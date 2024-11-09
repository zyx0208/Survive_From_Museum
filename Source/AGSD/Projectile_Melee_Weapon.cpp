// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Melee_Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AProjectile_Melee_Weapon::AProjectile_Melee_Weapon()
{
	// 투사체 속도, 데미지, 사거리
	ProjectileSpeed = 10.0f;
	ProjectileDamage = 4.0f;
	ProjectileRange = 100.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> NewStaticMeshComponet(TEXT("/Script/Engine.StaticMesh'/Game/Projectile/Mesh/Melee.Melee'"));
	if (NewStaticMeshComponet.Succeeded()) {
		ProjectileMeshComponent->SetStaticMesh(NewStaticMeshComponet.Object);
		// ProjectileMeshComponent를 180도 회전 (Yaw 축 기준)
		FRotator NewRotation = FRotator(0.0f, 180.0f, 0.0f);
		ProjectileMeshComponent->SetRelativeRotation(NewRotation);
	}
}

void AProjectile_Melee_Weapon::WeaponHitEffect()
{
	// 추가적인 기능을 구현
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("MeleeHit")));
}
