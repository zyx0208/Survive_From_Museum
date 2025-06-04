// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HPDashUIActor.generated.h"

class UWidgetComponent;
class UHPandDashUI;

UCLASS()
class AGSD_API AHPDashUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHPDashUIActor();
    virtual void Tick(float DeltaTime) override;
    void Initialize(AActor* FollowTarget); // 따라다닐 캐릭터 설정

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    UWidgetComponent* HPandDashWidget;

    UPROPERTY()
    AActor* TargetActor;

};
