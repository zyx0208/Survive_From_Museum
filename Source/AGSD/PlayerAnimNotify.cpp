// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimNotify.h"
#include "AGSDCharacter.h"

void UPlayerAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AAGSDCharacter* Player = Cast<AAGSDCharacter>(MeshComp->GetOwner());
	if (Player) {
		Player->FireByType();
	}
}
