// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Melee_Two.h"
#include "AIController.h"
#include "Enemy1AIController.h"
#include "GameFramework/Character.h"
AProjectile_Melee_Two::AProjectile_Melee_Two()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 8.0f;
    ProjectileRange = 100.0f;
}

void AProjectile_Melee_Two::Tick(float deltaTime)
{
    Super::Super::Tick(deltaTime);
    // 현재 위치와 시작 위치의 거리를 계산
    float DistanceTraveled = FVector::Dist(StartLocation, GetActorLocation());

    // 특정 거리 이상 이동하면 파괴
    if (DistanceTraveled > ProjectileRange)
    {
        Destroy();
    }
}

void AProjectile_Melee_Two::WeaponHitEffect(AActor* OtherActor)
{
}

void AProjectile_Melee_Two::Damage(AActor* OtherActor)
{
    if (DamagedActor.Contains(OtherActor)) {
        return;
    }
    if (IsValid(OtherActor) && OtherActor != this)
    {
        UE_LOG(LogTemp, Display, TEXT("Projectile: %f Player: %f"), ProjectileDamage, PlayerAttack);
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("OverLap")));
        // 충돌한 오브젝트가 ACharacter 클래스인지 확인(적이 캐릭터 클래스이기 때문)
        if (OtherActor && OtherActor->IsA<ACharacter>())
        {
            // ACharacter로 캐스팅
            ACharacter* HitEnemy = Cast<ACharacter>(OtherActor);
            if (IsValid(HitEnemy))
            {
                //HitEnemy의 가진 AEnemy1AIController를 가져옴(여기에 피격 함수가 있음)
                AEnemy1AIController* HitEnemyController = Cast<AEnemy1AIController>(HitEnemy->GetController());
                if (IsValid(HitEnemyController))
                {
                    HitEnemyController->Attacked(ProjectileDamage + PlayerAttack, 1);
                    
                }
            }
            WeaponHitEffect(OtherActor);
        }
    }
    DamagedActor.Add(OtherActor);

}

