// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "Math/UnrealMathUtility.h"//랜덤 수 추출을 위한 헤더
#include "GameFramework/Character.h"//레벨에 등장하는 액터의 방향이나 이동을 조절하기 위한 함수를 가진 헤더파일
#include "Enemy1Class.h"
#include "Enemy1AIController.h"
#include "Blueprint/UserWidget.h"

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
	InnerCircleRange = 1000.0f;
	OuterCircleRange = 1500.0f;
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
    case 1: //1 스테이지(300초 / 10초 당 1웨이브 / 5웨이브마다 등장 패턴 변화)
        if (PlayerCharacter)
        {
            TempTime += DeltaTime;
            TotalTime += DeltaTime;

            //보스라운드
            if (TotalTime >= 300.0f)//이 시간을 바꾸면 보스전 진입 시간이 바뀜(기본 300초)
            {
                if (!BossRound)
                {
                    BossRound = true;
                    //모든 적 캐릭터 강제 삭제
                    TArray<AActor*> AllEnemys;
                    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy1Class::StaticClass(), AllEnemys);
                    for (int i = 0; i < AllEnemys.Num(); i++)
                    {
                        AEnemy1Class* Enemy = Cast<AEnemy1Class>(AllEnemys[i]);
                        if (Enemy)
                        {
                            AEnemy1AIController* AIC = Cast<AEnemy1AIController>(Enemy->GetController());
                            if (AIC)
                            {
                                AIC->Died(-1);
                            }
                        }
                    }
                    //보스 몹 소환 및 플레이어 이동
                    GetWorld()->SpawnActor<AActor>(Boss, FVector(-4800.0f, 28850.0f, 400.0f), FRotator::ZeroRotator);
                    PlayerCharacter->SetActorLocation(FVector(-6150.0f, 28850.0f, 400.0f));
                    if (BossTextUI)
                    {
                        CreateWidget<UUserWidget>(GetWorld(), BossTextUI)->AddToViewport();
                    }
                }
                SpawnNum = -3;//일반 몹 생성안하도록 설정
            }
            //50초 마다 패턴 변화
            else if (TotalTime >= 250.0f)
            {
                SpawnNum = 13;
            }
            else if (TotalTime >= 200.0f)
            {
                SpawnNum = 11;
            }
            else if (TotalTime >= 150.0f)
            {
                SpawnNum = 9;
            }
            else if (TotalTime >= 100.0f)
            {
                SpawnNum = 7;
            }
            else if (TotalTime >= 50.0f)
            {
                SpawnNum = 5;
            }
            else
            {
                SpawnTime = 10.0f;
                SpawnNum = 3;
            }
        }
        if (TempTime >= SpawnTime)
        {
            TempTime = 0.0f;
            if (Enemys.Num() > 0)
            {
                UE_LOG(LogTemp, Display, TEXT("Nomal Spawn : %d"), (SpawnNum + 3)/2);
                for (int i = 0; i < (SpawnNum + 3)/2; i++) //일반몹 소환
                {
                    TempEnemyCounter = FMath::RandRange(0, Enemys.Num() - 2);
                    GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter],
                        PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
                        FRotator::ZeroRotator);
                }
                UE_LOG(LogTemp, Display, TEXT("Elite Spawn : %d"), (SpawnNum - 3) / 2);
                for (int i = 0; i < (SpawnNum - 3)/2; i++) //정예몹 소환(정예몹 인덱스 : 마지막 인덱스)
                {
                    GetWorld()->SpawnActor<AActor>(Enemys[Enemys.Num() - 1],
                        PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
                        FRotator::ZeroRotator); //마지막 인덱스(정예몹)을 확정으로 소환
                }
            }
        }
        break;

    default: //스테이지가 설정되지 않았을 경우
        UE_LOG(LogTemp, Display, TEXT("Setting Stage Number."));
        break;
    }
}

