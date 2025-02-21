// Fill out your copyright notice in the Description page of Project Settings.


#include "AGSDGameInstance.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "SavingGame.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UAGSDGameInstance::Init()
{
    Super::Init();

    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (UserSettings)
    {
        UserSettings->SetScreenResolution(FIntPoint(1920, 1080));  // 원하는 해상도
        UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
        UserSettings->ApplySettings(false);
    }
}

void UAGSDGameInstance::SaveGameData()
{
    // SaveGame 객체 생성
    USavingGame* SaveGameInstance = NewObject<USavingGame>();

    // 데이터를 저장
    SaveGameInstance->StageProgress = Temp_StageProgress;
    SaveGameInstance->TalkingProgress = Temp_TalkingProgress;

    // 저장할 슬롯 이름
    FString SaveSlotName = TEXT("SaveSlot1");

    // 유저 인덱스 (기본값은 0으로 설정)
    int32 UserIndex = 0;

    // 게임 데이터를 저장
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
}

void UAGSDGameInstance::LoadGameData()
{
    // 저장된 슬롯 이름
    FString SaveSlotName = TEXT("SaveSlot1");

    // 유저 인덱스 (기본값은 0으로 설정)
    int32 UserIndex = 0;

    // 저장된 게임 데이터 불러오기
    USavingGame* LoadedGame = Cast<USavingGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));

    // 불러온 데이터가 있을 경우, 데이터를 적용
    if (LoadedGame)
    {
        Temp_StageProgress = LoadedGame->StageProgress;
        Temp_TalkingProgress = LoadedGame->TalkingProgress;

        // 예시: 불러온 데이터를 출력 (디버그용)
        UE_LOG(LogTemp, Warning, TEXT("Loaded StageProgress: %d"), Temp_StageProgress);
        UE_LOG(LogTemp, Warning, TEXT("Loaded TalkingProgress: %d"), Temp_TalkingProgress);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No save file found."));
    }
}

void UAGSDGameInstance::ResetGameData()
{
    // SaveGame 객체 생성
    USavingGame* SaveGameInstance = NewObject<USavingGame>();

    // 데이터를 초기값으로 저장
    SaveGameInstance->StageProgress = 0;
    SaveGameInstance->TalkingProgress = 0;

    // 임시 변수 초기화
    Temp_StageProgress = 0;
    Temp_TalkingProgress = 0;

    // 저장할 슬롯 이름
    FString SaveSlotName = TEXT("SaveSlot1");

    // 유저 인덱스 (기본값은 0으로 설정)
    int32 UserIndex = 0;

    // 게임 데이터를 저장
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
    UE_LOG(LogTemp, Warning, TEXT("Reset Complete."));
    UE_LOG(LogTemp, Warning, TEXT("Temp_StageProgress : %d"), Temp_StageProgress);
    UE_LOG(LogTemp, Warning, TEXT("Temp_TalkingProgress : %d"), Temp_TalkingProgress);
}