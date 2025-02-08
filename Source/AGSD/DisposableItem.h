// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DisposableItem.generated.h"

UCLASS()
class AGSD_API ADisposableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADisposableItem();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")int Item = 0; //아이템 숫자 [1: 폭탄] [2: HP 절반회복] [3: HP 풀 회복] [4: 자석]
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")float BombRange = 0.0f; //폭탄 범위 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")TSubclassOf<AActor> PickEffect = NULL;

    // 충돌을 감지할 콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* CollisionComponent;

    //충돌이벤트
    UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
