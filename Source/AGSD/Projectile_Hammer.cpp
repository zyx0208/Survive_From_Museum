// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Hammer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AProjectile_Hammer::AProjectile_Hammer()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 0.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_Hammer::WeaponHitEffect(AActor* OtherActor)
{
    UE_LOG(LogTemp, Display, TEXT("KnockBack"));
    FVector KnockBackDirection = GetActorForwardVector();
    ACharacter* HitEnemy = Cast<ACharacter>(OtherActor);

    if (HitEnemy) {
        FVector KnockBackVector = KnockBackDirection * 3000.0f;
        KnockBackVector.Z = 10.0f;
        UE_LOG(LogTemp, Log, TEXT("KnockBackX: %f KnockBackY: %f KnockBackZ: %f"), KnockBackVector.X, KnockBackVector.Y, KnockBackVector.Z);
        UCharacterMovementComponent* EnemyMovement = HitEnemy->GetCharacterMovement();
        EnemyMovement->StopMovementImmediately();
        HitEnemy->LaunchCharacter(KnockBackVector, true, true);
        UCapsuleComponent* EnemyComp = HitEnemy->GetCapsuleComponent();
        float Mass = HitEnemy->GetMesh()->CalculateMass();
        UE_LOG(LogTemp, Log, TEXT("EnemyMass: %f"), Mass);
        if (EnemyComp) {
            // 전체 충돌은 활성화 상태 유지
            EnemyComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

            // 기본값을 Overlap으로 설정
            EnemyComp->SetCollisionResponseToAllChannels(ECR_Overlap);

            // 벽(StaticObject, WorldStatic)은 Block으로 설정
            EnemyComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
            EnemyComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block); // 필요 시

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()
                {
                    EnemyComp->SetCollisionProfileName(TEXT("Enemy"));
                }, 0.2f, false);
        }
        
    }
}

void AProjectile_Hammer::Tick(float DeltaTime)
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
