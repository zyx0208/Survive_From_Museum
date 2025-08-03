// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatusEffect_Ice.h"
#include "Enemy1Class.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy1AIController.h"

void AEnemyStatusEffect_Ice::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyStatusEffect_Ice::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyStatusEffect_Ice::TimePerEffect()
{
    AActor* ParentActor = GetAttachParentActor();
    //AActor* ParentActor = GetOwner();
    if (IsValid(ParentActor)) {
        if (ParentActor->IsA(AEnemy1AIController::StaticClass())) {
            AEnemy1AIController* Enemy = Cast<AEnemy1AIController>(ParentActor);
            if (IsValid(Enemy)) {
                Enemy->Attacked(IceDamage);
                Enemy->Stun(1.0);
                if (EffectSoundCue) {
                    UGameplayStatics::PlaySound2D(GetWorld(), EffectSoundCue);
                }
            }
        }
        UE_LOG(LogTemp, Display, TEXT("Fire Damage!"));
    }
}

AEnemyStatusEffect_Ice::AEnemyStatusEffect_Ice()
{
}
