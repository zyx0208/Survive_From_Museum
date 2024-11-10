// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Beta_Blue.h"

AProjectile_Beta_Blue::AProjectile_Beta_Blue()
{
	// 투사체 속도, 데미지, 사거리
	ProjectileSpeed = 7.0f;
	ProjectileDamage = 4.0f;
	ProjectileRange = 500.0f;

	static ConstructorHelpers::FObjectFinder<UMaterial> NewMaterial(TEXT("/Script/Engine.Material'/Game/Projectile/Material/Projectile_Blue.Projectile_Blue'"));
	if (NewMaterial.Succeeded())
	{
		// ProjectileMeshComponent가 부모 클래스에서 생성되었으므로, 여기서 마테리얼을 설정합니다.
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(NewMaterial.Object, ProjectileMeshComponent);
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	}
}

void AProjectile_Beta_Blue::WeaponHitEffect()
{
	// 추가적인 기능을 구현
	UE_LOG(LogTemp, Warning, TEXT("BlueHit"));
	// 발사체를 파괴 (필요시)
	Destroy();
}
