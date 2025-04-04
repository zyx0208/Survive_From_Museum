// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Hammer_Upgrade.h"
#include "Enemy1AIController.h"

AProjectile_Hammer_Upgrade::AProjectile_Hammer_Upgrade()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 0.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_Hammer_Upgrade::WeaponHitEffect(AActor* OtherActor)
{
    ACharacter* HitEnemy = Cast<ACharacter>(OtherActor);
    if (HitEnemy) {
        AEnemy1AIController* HitEnemyController = Cast<AEnemy1AIController>(HitEnemy->GetController());
        if (HitEnemyController)
        {
            HitEnemyController->Stun(3.0f);
        }
    }
}

void AProjectile_Hammer_Upgrade::Tick(float DeltaTime)
{
}
