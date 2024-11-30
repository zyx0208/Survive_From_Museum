// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_FireBall.h"

AProjectile_FireBall::AProjectile_FireBall()
{
    ProjectileSpeed = 0.2f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;

    static ConstructorHelpers::FObjectFinder<UMaterial> NewMaterial(TEXT("/Script/Engine.Material'/Game/Projectile/Material/Projectile_FireBall.Projectile_FireBall'"));
    if (NewMaterial.Succeeded())
    {
        // ProjectileMeshComponent가 부모 클래스에서 생성되었으므로, 여기서 마테리얼을 설정합니다.
        ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(NewMaterial.Object, ProjectileMeshComponent);
        ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
    }
}

void AProjectile_FireBall::WeaponHitEffect()
{
    Destroy();
}
