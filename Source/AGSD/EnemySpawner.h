// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
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
    //몬스터의 정보를 담는 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TArray<TSubclassOf<AActor>> Enemys; 
    //스테이지 숫자 입력(1 ~ ...)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")int Stage = 0; 
    //웨이브 단계(색깔놀이)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")int WaveLevel = 0; 
    //몬스터 소환 횟수(웨이브마다 초기화)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")int NumOfSpawn = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> Boss = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> Patton1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> Patton2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> Patton3 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> Patton4 = NULL;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> BossTextUI = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")FVector BossSpawnPoint;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")FVector PlayerSpawnPoint;

    //플레이어 위치 및 방향 정보를 담는 액터 배열
    ACharacter* PlayerCharacter; 
	//적이 등장하는 최소 거리(도넛 모양)
	float InnerCircleRange = 0.0f;
    //적이 등장하는 최대 거리(도넛 모양)
	float OuterCircleRange = 0.0f;
    //몇초마다 생성할지
	float SpawnTime = 0.0f;
    //몇마리 생성할지
    int SpawnNum = 0;
    //보스전이 시작됐는지 확인하기 위한 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")bool BossRound = false; 

private:
    //스테이지 시작 후 총 걸린 시간
    float TotalTime = 0.0f;
    //생성 시간을 제어하기 위한 임의 변수
	float TempTime = 0.0f;
    //적 생성을 제어하기 위한 임의 변수
	int64 TempEnemyCounter = 0;
    //개발자가 몇분 지났는지 알기 위해 설정한 임의 변수
    float LogTime = 0.0f; 
    //패턴 소환을 위한 변수
    bool Patton1Spawn;
    //패턴 소환을 위한 변수
    bool Patton2Spawn;
    //패턴 소환을 위한 변수
    bool Patton3Spawn;
    //패턴 소환을 위한 변수
    bool Patton4Spawn;
    UNavigationSystemV1* NavSys;
};
