// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class AGSD_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TArray<TSubclassOf<AActor>> Enemys; //몬스터의 정보를 담는 배열
	ACharacter* PlayerCharacter; //플레이어 위치 및 방향 정보를 담는 액터 배열
	//안쪽 범위 밖 부분, 바깥쪽 범위의 안 부분에서 적이 등장(도넛 모양)
	float InnerCircleRange; 
	float OuterCircleRange; 
	float SpawnTime;//몇초마다 생성할지
private:
	float TempTimer;//생성 시간을 제어하기 위한 임의 변수
	float TempEnemyCounter;//적 생성을 제어하기 위한 임의 변수
};
