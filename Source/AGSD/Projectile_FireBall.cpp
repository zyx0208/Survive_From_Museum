// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_FireBall.h"
#include "Enemy1AIController.h"
#include "EnemyStatusEffect_Fire.h"
#include "NiagaraComponent.h"
#include "EnemyStatusEffect.h"
AProjectile_FireBall::AProjectile_FireBall()
{
    ProjectileSpeed = 0.2f;
    ProjectileDamage = 5.0f;
    ProjectileRange = 2000.0f;
}



void AProjectile_FireBall::WeaponHitEffect(AActor* OtherActor)
{
    if (OtherActor && OtherActor->IsA<ACharacter>())
    {
        // ACharacter로 캐스팅
        ACharacter* HitEnemy = Cast<ACharacter>(OtherActor);
        if (HitEnemy)
        {
            //HitEnemy의 가진 AEnemy1AIController를 가져옴(여기에 피격 함수가 있음)
            AEnemy1AIController* HitEnemyController = Cast<AEnemy1AIController>(HitEnemy->GetController());
            if (HitEnemyController)
            {
                AEnemyStatusEffect* TestEffect = GetWorld()->SpawnActor<AEnemyStatusEffect_Fire>(FireEffect);
                if (TestEffect) {
                    TestEffect->AttachToActor(HitEnemyController, FAttachmentTransformRules::KeepRelativeTransform);
                    //TestEffect->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
                    //TestEffect->SetOwner(HitEnemyController);
                }
                if (FireVFX) {
                    UNiagaraComponent* FireVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
                        FireVFX,
                        HitEnemy->GetRootComponent(),
                        NAME_None,
                        FVector::ZeroVector,
                        FRotator::ZeroRotator,
                        EAttachLocation::KeepRelativeOffset,
                        true
                    );
                }
            }
        }
    }
    Destroy();
}

void AProjectile_FireBall::BeginPlay()
{
    Super::BeginPlay();
}
