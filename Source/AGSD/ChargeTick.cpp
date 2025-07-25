// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargeTick.h"
#include "AGSDCharacter.h"

void UChargeTick::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    AAGSDCharacter* Player = Cast<AAGSDCharacter>(MeshComp->GetOwner());
    if (Player) {
        Player->Charging();
    }
}
