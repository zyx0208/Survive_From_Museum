// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XPOrb.generated.h"

UCLASS()
class AGSD_API AXPOrb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXPOrb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 구슬의 충돌을 감지하는 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class USphereComponent* SphereComponent;
	// 충돌 이벤트 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
