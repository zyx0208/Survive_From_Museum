// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Jelda.h"

AProjectile_Jelda::AProjectile_Jelda()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> NewStaticMeshComponet(TEXT("/Script/Engine.StaticMesh'/Game/Projectile/Mesh/Melee.Melee'"));
    if (NewStaticMeshComponet.Succeeded()) {
        ProjectileMeshComponent->SetStaticMesh(NewStaticMeshComponet.Object);
        // ProjectileMeshComponent를 180도 회전 (Yaw 축 기준)
        FRotator NewRotation = FRotator(0.0f, 180.0f, 0.0f);
        ProjectileMeshComponent->SetRelativeRotation(NewRotation);
        ProjectileMeshComponent->SetHiddenInGame(true);
    }
}

void AProjectile_Jelda::WeaponHitEffect()
{
}
