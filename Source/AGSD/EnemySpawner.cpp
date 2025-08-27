// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "Math/UnrealMathUtility.h"//랜덤 수 추출을 위한 헤더
#include "GameFramework/Character.h"//레벨에 등장하는 액터의 방향이나 이동을 조절하기 위한 함수를 가진 헤더파일
#include "Enemy1Class.h"
#include "Enemy1AIController.h"
#include "Blueprint/UserWidget.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

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
    Patton1Spawn = false;
    Patton2Spawn = false;
    Patton3Spawn = false;
    Patton4Spawn = false;
    Patton5Spawn = false;
    NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
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
            if (TotalTime >= 5.0f)//이 시간을 바꾸면 보스전 진입 시간이 바뀜(기본 300초)
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
                    GetWorld()->SpawnActor<AActor>(Boss, BossSpawnPoint, FRotator::ZeroRotator);
                    PlayerCharacter->SetActorLocation(PlayerSpawnPoint);
                    if (BossTextUI)
                    {
                        CreateWidget<UUserWidget>(GetWorld(), BossTextUI)->AddToViewport();
                    }
                }
            }
            //50초 마다 패턴 변화
            else if (TotalTime >= 250.0f)
            {
                SpawnTime = 10.0f / 13;
                SpawnNum = 1;
            }
            else if (TotalTime >= 200.0f)
            {
                SpawnTime = 10.0f / 11;
                SpawnNum = 1;
            }
            else if (TotalTime >= 150.0f)
            {
                SpawnTime = 10.0f / 9;
                SpawnNum = 1;
            }
            else if (TotalTime >= 100.0f)
            {
                SpawnTime = 10.0f / 7;
                SpawnNum = 1;
            }
            else if (TotalTime >= 50.0f)
            {
                SpawnTime = 10.0f / 5;
                SpawnNum = 1;
            }
            else
            {
                SpawnTime = 10.0f / 3;
                SpawnNum = 1;
            }
        }
        if (TempTime >= SpawnTime and !BossRound)
        {
            TempTime = 0.0f;
            if (Enemys.Num() > 0)
            {
                TempEnemyCounter = 0;
                for (int j = 0; j < 10; j++)
                {
                    FVector EnemySpawnLocation = PlayerCharacter->GetActorLocation()
                        + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal() * FMath::FRandRange(InnerCircleRange, OuterCircleRange);
                    FNavLocation NavEnemySpawnLocation;
                    if (NavSys->ProjectPointToNavigation(EnemySpawnLocation, NavEnemySpawnLocation, FVector(500.f, 500.f, 500.f)))
                    {
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter], NavEnemySpawnLocation.Location + FVector(0, 0, 100.0f), FRotator::ZeroRotator);
                        break;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Display, TEXT("[%d]Can't Find NaviMash for Spawn : %s"), j, *EnemySpawnLocation.ToString());
                    }
                }
            }
        }
        break;

    case 2:
        if (PlayerCharacter)
        {
            TempTime += DeltaTime;
            TotalTime += DeltaTime;

            //보스라운드
            if (TotalTime >= 5.0f)//이 시간을 바꾸면 보스전 진입 시간이 바뀜(기본 300초)
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
                    GetWorld()->SpawnActor<AActor>(Boss, BossSpawnPoint, FRotator::ZeroRotator);
                    PlayerCharacter->SetActorLocation(PlayerSpawnPoint);
                    if (BossTextUI)
                    {
                        CreateWidget<UUserWidget>(GetWorld(), BossTextUI)->AddToViewport();
                    }
                }
            }
            //50초 마다 패턴 변화
            else if (TotalTime >= 250.0f)
            {
                SpawnTime = 10.0f / 13;
                SpawnNum = 1;
                if (!Patton5Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton5Spawn = true;
                    if (Patton5)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton5,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 200.0f)
            {
                SpawnTime = 10.0f / 11;
                SpawnNum = 1;
                if (!Patton4Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton4Spawn = true;
                    if (Patton4)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton4,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 150.0f)
            {
                SpawnTime = 10.0f / 9;
                SpawnNum = 1;
                if (!Patton3Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton3Spawn = true;
                    if (Patton3)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton3,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 100.0f)
            {
                SpawnTime = 10.0f / 7;
                SpawnNum = 1;
                if (!Patton2Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton2Spawn = true;
                    if (Patton2)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton2,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 50.0f)
            {
                SpawnTime = 10.0f / 5;
                SpawnNum = 1;
                if (!Patton1Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton1Spawn = true;
                    if (Patton1)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton1,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else
            {
                SpawnTime = 10.0f / 3;
                SpawnNum = 1;
            }
        }
        if (TempTime >= SpawnTime and !BossRound)
        {
            TempTime = 0.0f;
            if (Enemys.Num() > 0)
            {
                TempEnemyCounter = FMath::RandRange(0, 1);
                for (int j = 0; j < 10; j++)
                {
                    FVector EnemySpawnLocation = PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal() * FMath::FRandRange(InnerCircleRange, OuterCircleRange);
                    FNavLocation NavEnemySpawnLocation;
                    if (NavSys->ProjectPointToNavigation(EnemySpawnLocation, NavEnemySpawnLocation, FVector(500.f, 500.f, 500.f)))
                    {
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter], NavEnemySpawnLocation.Location + FVector(0, 0, 100.0f), FRotator::ZeroRotator);
                        break;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Display, TEXT("[%d]Can't Find NaviMash for Spawn : %s"), j, *EnemySpawnLocation.ToString());
                    }
                }
            }
        }
        break;

    case 3:
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
                    GetWorld()->SpawnActor<AActor>(Boss, BossSpawnPoint, FRotator::ZeroRotator);
                    PlayerCharacter->SetActorLocation(PlayerSpawnPoint);
                    if (BossTextUI)
                    {
                        CreateWidget<UUserWidget>(GetWorld(), BossTextUI)->AddToViewport();
                    }
                }
            }
            //50초 마다 패턴 변화
            else if (TotalTime >= 250.0f)
            {
                SpawnTime = 10.0f / 13;
                SpawnNum = 1;
                if (!Patton5Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton5Spawn = true;
                    if (Patton5)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton5,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 200.0f)
            {
                SpawnTime = 10.0f / 11;
                SpawnNum = 1;
                if (!Patton4Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton4Spawn = true;
                    if (Patton4)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton4,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 150.0f)
            {
                SpawnTime = 10.0f / 9;
                SpawnNum = 1;
                if (!Patton3Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton3Spawn = true;
                    if (Patton3)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton3,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 100.0f)
            {
                SpawnTime = 10.0f / 7;
                SpawnNum = 1;
                if (!Patton2Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton2Spawn = true;
                    if (Patton2)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton2,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 50.0f)
            {
                SpawnTime = 10.0f / 5;
                SpawnNum = 1;
                if (!Patton1Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton1Spawn = true;
                    if (Patton1)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton1,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else
            {
                SpawnTime = 10.0f / 3;
                SpawnNum = 1;
            }
        }
        if (TempTime >= SpawnTime and !BossRound)
        {
            TempTime = 0.0f;
            if (Enemys.Num() > 0)
            {
                TempEnemyCounter = FMath::RandRange(0, 1);
                for (int j = 0; j < 10; j++)
                {
                    FVector EnemySpawnLocation = PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal() * FMath::FRandRange(InnerCircleRange, OuterCircleRange);
                    FNavLocation NavEnemySpawnLocation;
                    if (NavSys->ProjectPointToNavigation(EnemySpawnLocation, NavEnemySpawnLocation, FVector(500.f, 500.f, 500.f)))
                    {
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter], NavEnemySpawnLocation.Location + FVector(0, 0, 100.0f), FRotator::ZeroRotator);
                        break;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Display, TEXT("[%d]Can't Find NaviMash for Spawn : %s"), j, *EnemySpawnLocation.ToString());
                    }
                }
            }
        }
        break;

    case 4:
        if (PlayerCharacter)
        {
            TempTime += DeltaTime;
            TotalTime += DeltaTime;

            //보스라운드
            if (TotalTime >= 5.0f)//이 시간을 바꾸면 보스전 진입 시간이 바뀜(기본 300초)
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
                    GetWorld()->SpawnActor<AActor>(Boss, BossSpawnPoint, FRotator::ZeroRotator);
                    PlayerCharacter->SetActorLocation(PlayerSpawnPoint);
                    if (BossTextUI)
                    {
                        CreateWidget<UUserWidget>(GetWorld(), BossTextUI)->AddToViewport();
                    }
                }
            }
            //50초 마다 패턴 변화
            else if (TotalTime >= 250.0f)
            {
                SpawnTime = 10.0f / 13;
                SpawnNum = 1;
                if (!Patton5Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton5Spawn = true;
                    if (Patton5)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton5,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 200.0f)
            {
                SpawnTime = 10.0f / 11;
                SpawnNum = 1;
                if (!Patton4Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton4Spawn = true;
                    if (Patton4)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton4,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 150.0f)
            {
                SpawnTime = 10.0f / 9;
                SpawnNum = 1;
                if (!Patton3Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton3Spawn = true;
                    if (Patton3)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton3,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 100.0f)
            {
                SpawnTime = 10.0f / 7;
                SpawnNum = 1;
                if (!Patton2Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton2Spawn = true;
                    if (Patton2)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton2,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else if (TotalTime >= 50.0f)
            {
                SpawnTime = 10.0f / 5;
                SpawnNum = 1;
                if (!Patton1Spawn)
                {
                    UE_LOG(LogTemp, Display, TEXT("Patton1 Spawn."));
                    Patton1Spawn = true;
                    if (Patton1)
                    {
                        GetWorld()->SpawnActor<AActor>(Patton1,
                            PlayerCharacter->GetActorLocation(),
                            FRotator::ZeroRotator);
                    }
                }
            }
            else
            {
                SpawnTime = 10.0f / 3;
                SpawnNum = 1;
            }
        }
        if (TempTime >= SpawnTime and !BossRound)
        {
            TempTime = 0.0f;
            if (Enemys.Num() > 0)
            {
                TempEnemyCounter = FMath::RandRange(0, 1);
                for (int j = 0; j < 10; j++)
                {
                    FVector EnemySpawnLocation = PlayerCharacter->GetActorLocation() + FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal() * FMath::FRandRange(InnerCircleRange, OuterCircleRange);
                    FNavLocation NavEnemySpawnLocation;
                    if (NavSys->ProjectPointToNavigation(EnemySpawnLocation, NavEnemySpawnLocation, FVector(500.f, 500.f, 500.f)))
                    {
                        GetWorld()->SpawnActor<AActor>(Enemys[TempEnemyCounter], NavEnemySpawnLocation.Location + FVector(0, 0, 100.0f), FRotator::ZeroRotator);
                        break;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Display, TEXT("[%d]Can't Find NaviMash for Spawn : %s"), j, *EnemySpawnLocation.ToString());
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

