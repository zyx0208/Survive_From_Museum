// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseIndicatorActor.generated.h"

UCLASS()
class AGSD_API AMouseIndicatorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMouseIndicatorActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UDecalComponent* DecalComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UBoxComponent* CollisionBox;

    void SetDecalCollision(ECollisionEnabled::Type Mouse);

};
