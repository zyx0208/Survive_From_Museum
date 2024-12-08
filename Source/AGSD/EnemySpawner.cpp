// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "Math/UnrealMathUtility.h"//랜덤 수 추출을 위한 헤더
#include "GameFramework/Character.h"//레벨에 등장하는 액터의 방향이나 이동을 조절하기 위한 함수를 가진 헤더파일

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	InnerCircleRange = 500.0f;
	OuterCircleRange = 1000.0f;
	SpawnTime = 5.0f;
	PlayerCharacter = NULL;
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    switch (Stage)
    {
    case 1: //1 스테이지(15분 = 900초)
        if (PlayerCharacter)
        {
            TempTime += DeltaTime;
            TotalTime += DeltaTime;

            //시간 확인을 위한 로그 함수 부분(후에 타이머 기능이 생기면 삭제)
            LogTime += DeltaTime;
            if (LogTime >= 60.0f)
            {
                LogTime = 0.0f;
                UE_LOG(LogTemp, Display, TEXT("TotalTime : %f"), TotalTime);
            }

            //60초 마다 패턴 변화
            if (TotalTime >= 840.0f)
            {
                SpawnTime = 0.5f;
                SpawnNum = 20;
            }
            else if (TotalTime >= 780.0f)
            {
                SpawnTime = 0.5f;
                SpawnNum = 15;
            }
            else if (TotalTime >= 720.0f)
            {
                SpawnTime = 0.5f;
                SpawnNum = 10;
            }
            else if (TotalTime >= 660.0f)
            {
                SpawnTime = 1.0f;
                SpawnNum = 10;
            }
            else if (TotalTime >= 600.0f) //정예 몹 다수 등장
            {
                SpawnTime = 1.0f;
                SpawnNum = 8;
            }
            else if (TotalTime >= 540.0f)
            {
                SpawnTime = 1.0f;
                SpawnNum = 7;
            }
            else if (TotalTime >= 480.0f)
            {
                SpawnTime = 1.0f;
                SpawnNum = 6;
            }
            else if (TotalTime >= 420.0f)
            {
                SpawnTime = 1.0f;
                SpawnNum = 5;
            }
            else if (TotalTime >= 360.0f)
            {
                SpawnTime = 1.0f;
                SpawnNum = 4;
            }
            else if (TotalTime >= 300.0f) //정예 몹 등장 시기
            {
                SpawnTime = 1.0f;
                SpawnNum = 3;
            }
            else if (TotalTime >= 240.0f)
            {
                SpawnTime = 1.0f;
                SpawnNum = 2;
            }
            else if (TotalTime >= 180.0f)
            {
                SpawnTime = 2.0f;
                SpawnNum = 2;
            }
            else if (TotalTime >= 120.0f)
            {
                SpawnTime = 3.0f;
                SpawnNum = 2;
            }
            else if (TotalTime >= 60.0f)
            {
                SpawnTime = 3.0f;
                SpawnNum = 1;
            }
            else
            {
                SpawnTime = 5.0f;
                SpawnNum = 1;
            }
        }
        if (TempTime >= SpawnTime)
        {
            TempTime = 0.0f;
            if (Enemys.Num() > 0)
            {
                if (TotalTime >= 110.0f) //600초 ~ 900초 : 정예몹이 최소 한마리는 포함
                {
                    for (int i = 0; i < SpawnNum - 1; i++)
                    {
                        TempEnemyCounter = FMath::RandRange(0, Enemys.Num() - 1);
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter],
                            PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
                            FRotator::ZeroRotator);
                    }
                    GetWorld()->SpawnActor<AActor>(Enemys[Enemys.Num() - 1],
                        PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
                        FRotator::ZeroRotator); //마지막 인덱스(정예몹)을 확정으로 소환
                }/*
                else if(TotalTime >= 300.0f) //300초 ~ 600초 : 정예몹 확률적으로 등장
                {
                    for (int i = 0; i < SpawnNum; i++)
                    {
                        TempEnemyCounter = FMath::RandRange(0, Enemys.Num() - 1);
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter],
                            PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
                            FRotator::ZeroRotator);
                    }
                }*/
                else //0초 ~ 300초 : 정예몹 등장 X
                {
                    for (int i = 0; i < SpawnNum; i++)
                    {
                        TempEnemyCounter = FMath::RandRange(0, Enemys.Num() - 2);
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter],
                            PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
                            FRotator::ZeroRotator);
                    }
                }
            }
        }
        break;

    default: //스테이지가 설정되지 않았을 경우
        UE_LOG(LogTemp, Display, TEXT("Setting Stage Number."));
        break;
    }
}

