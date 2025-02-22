// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatusEffect_Fire.h"
#include "Enemy1AIController.h"

AEnemyStatusEffect_Fire::AEnemyStatusEffect_Fire()
{

}
void AEnemyStatusEffect_Fire::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyStatusEffect_Fire::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyStatusEffect_Fire::TimePerEffect()
{
    //AActor* ParentActor = GetAttachParentActor();
    AActor* ParentActor = GetOwner();
    if (ParentActor && ParentActor->IsA(AEnemy1AIController::StaticClass())) {
        AEnemy1AIController* Enemy = Cast<AEnemy1AIController>(ParentActor);
        Enemy->Attacked(FireDamage);
        UE_LOG(LogTemp, Display, TEXT("Fire Damage!"));
    }
}


