// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGSDGameMode.h"
#include "AGSDCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "ManagingGame.h"
#include "AGSDGameInstance.h"

AAGSDGameMode::AAGSDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/AGSD/AGSD_Character/Character_BP/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
    if (GEngine)
    {
        GEngine->bEnableOnScreenDebugMessages = false; // 디버그 메시지 끄기
    }
}

void AAGSDGameMode::BeginPlay()
{
    Super::BeginPlay(); // 부모 클래스의 BeginPlay 호출
    UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay Called!"));
    // 게임 인스턴스를 안전하게 캐스팅
    UAGSDGameInstance* GameInstance = Cast<UAGSDGameInstance>(GetGameInstance());
    ResetAccessoryData();

    if (GameInstance)
    {
        // 게임 인스턴스에서 데이터를 로드하는 함수 호출
        GameInstance->LoadGameData();        
        UE_LOG(LogTemp, Warning, TEXT("Data Load Complete."));

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is null!"));
    }
}

void AAGSDGameMode::GameExit()
{
    UAGSDGameInstance* GameInstance = Cast<UAGSDGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->SaveGameData();
    }
}

void AAGSDGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // 게임 종료 시 데이터 저장
    AAGSDGameMode::GameExit();
}

void AAGSDGameMode::ResetAccessoryData()
{
    if (!AccessoryDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("ResetAccessoryData: AccessoryDataTable is NULL"));
        return;
    }
    
    static const FString ContextString(TEXT("ResetAccessoryData"));
    TArray<FName> RowNames = AccessoryDataTable->GetRowNames();

    for (FName RowName : RowNames)
    {
        FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, ContextString, true);
        if (Accessory)
        {
            //데이터 초기화
            Accessory->bIsAcquired = false;

            UE_LOG(LogTemp, Log, TEXT("ResetAccessoryData: Reset %s"), *RowName.ToString());
        }
    }    
}