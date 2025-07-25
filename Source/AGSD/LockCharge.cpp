// Fill out your copyright notice in the Description page of Project Settings.


#include "LockCharge.h"
#include "AGSDCharacter.h"

void ULockCharge::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    AAGSDCharacter* Player = Cast<AAGSDCharacter>(MeshComp->GetOwner());
    if (Player) {
        Player->LockCharge();
    }
}
