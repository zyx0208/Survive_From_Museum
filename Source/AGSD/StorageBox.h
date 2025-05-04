// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "StorageBox.generated.h"

UCLASS()
class AGSD_API AStorageBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStorageBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // 박스의 충돌을 감지하는 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Collision")
    class USphereComponent* SphereComponent;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    

};
