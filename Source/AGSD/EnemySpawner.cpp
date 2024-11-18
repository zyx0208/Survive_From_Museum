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
	SpawnTime = 3.0f;
	PlayerCharacter = NULL;
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter)
	{
		TempTimer += DeltaTime;
		if (TempTimer >= SpawnTime)
		{
			TempTimer = 0.0f;
			if (Enemys.Num() > 0)
			{
				TempEnemyCounter = FMath::RandRange(0, Enemys.Num() - 1); //0부터 Enemys의 [변수 개수 - 1]까지의 인덱스

				GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter],
					PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f) * FMath::FRandRange(InnerCircleRange, OuterCircleRange),
					FRotator::ZeroRotator);
			}
		}
	}
}

