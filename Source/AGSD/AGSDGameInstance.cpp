// Fill out your copyright notice in the Description page of Project Settings.


#include "AGSDGameInstance.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "SavingGame.h"
#include "Engine/GameInstance.h"
#include "WeaponDataTableBeta.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UAGSDGameInstance::Init()
{
    Super::Init();

    LoadGameData();

    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (UserSettings)
    {
        UserSettings->SetScreenResolution(FIntPoint(1920, 1080));  // 원하는 해상도
        UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
        UserSettings->ApplySettings(false);
    }
    WeaponArray.Add(4);
    WeaponArray.Add(5);

    FirstLoading();
}

void UAGSDGameInstance::FirstLoading()
{
    UE_LOG(LogTemp, Log, TEXT("Loading Start."));
    //레벨 로딩용 카운터 초기화
    IsLoadingEnd = false;

    //로딩 화면 생성
    if (LoadingWidgetClass)
    {
        LoadingWidget = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
        if (LoadingWidget)
        {
            LoadingWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("Loading UI Create."));
        }
    }

    //로딩이 필요한 액터들 배열에 추가
    TArray<FSoftObjectPath> AssetsToLoad;

    for (int32 i = 0; i < LoadingActor.Num(); i++)
    {
        TSoftClassPtr<AActor> SoftClass = LoadingActor[i];
        AssetsToLoad.Add(SoftClass.ToSoftObjectPath());
    }
    UE_LOG(LogTemp, Log, TEXT("Asset Loading Ready."));

    //로딩 시작
    if (AssetsToLoad.Num() > 0)
    {
        //로딩 종료 후 레벨 로딩 함수 호출
        StreamableManager.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UAGSDGameInstance::EndLoading));
    }
    else
    {
        //로딩할 게 없으면 레벨 로딩 함수 호출
        EndLoading();
    }
}

void UAGSDGameInstance::EndLoading()
{
    UE_LOG(LogTemp, Log, TEXT("Loading Complete!"));

    // 로딩 화면 제거
    if (LoadingWidget)
    {
        LoadingWidget->RemoveFromParent();
        LoadingWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("Loading UI Deleted."));
    }
    IsLoadingEnd = true;
}

void UAGSDGameInstance::LevelLoadingStart()
{
    IsLevelLoading = true;
    UWorld* World = GetWorld();
    if (World && GEngine && GEngine->GameViewport)
    {
        LoadingWidget = CreateWidget<UUserWidget>(World, LoadingWidgetClass);
        if (LoadingWidget)
        {
            GEngine->GameViewport->AddViewportWidgetContent(LoadingWidget->TakeWidget(), 9999);
            UE_LOG(LogTemp, Log, TEXT("Loading UI Added to GameViewport directly."));
        }
    }
}

void UAGSDGameInstance::LevelLoadingEnd()
{
    IsLevelLoading = false;
    if (LoadingWidget)
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(LoadingWidget->TakeWidget());
        LoadingWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("Loading UI Deleted."));
    }
}

void UAGSDGameInstance::Shutdown()
{
    Super::Shutdown();
    SaveGameData(); // 자동 저장
}

void UAGSDGameInstance::CreateGameData()
{
    // SaveGame 객체 생성
    USavingGame* SaveGameInstance = NewObject<USavingGame>();

    // 데이터를 디폴트값으로 지정
    SaveGameInstance->StageProgress = -1;
    SaveGameInstance->TalkingProgress = 0;
    SaveGameInstance->BGMVolume = 1.0f;
    SaveGameInstance->SFXVolume = 1.0f;

    // 저장할 슬롯 이름
    FString SaveSlotName = TEXT("SaveSlot1");

    // 유저 인덱스 (기본값은 0으로 설정)
    int32 UserIndex = 0;

    // 게임 데이터를 저장
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
}

void UAGSDGameInstance::SaveGameData()
{
    // SaveGame 객체 생성
    USavingGame* SaveGameInstance = NewObject<USavingGame>();

    // 데이터를 저장
    SaveGameInstance->StageProgress = Temp_StageProgress;
    SaveGameInstance->TalkingProgress = Temp_TalkingProgress;
    SaveGameInstance->BGMVolume = Temp_BGMVolume;
    SaveGameInstance->SFXVolume = Temp_SFXVolume;

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
        Temp_BGMVolume = LoadedGame->BGMVolume;
        Temp_SFXVolume = LoadedGame->SFXVolume;

        // 예시: 불러온 데이터를 출력 (디버그용)
        UE_LOG(LogTemp, Warning, TEXT("Loaded StageProgress: %d"), Temp_StageProgress);
        UE_LOG(LogTemp, Warning, TEXT("Loaded TalkingProgress: %d"), Temp_TalkingProgress);
        UE_LOG(LogTemp, Warning, TEXT("Loaded StageProgress: %f"), Temp_BGMVolume);
        UE_LOG(LogTemp, Warning, TEXT("Loaded TalkingProgress: %f"), Temp_SFXVolume);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No save file found."));
        //저장된 세이브가 없을 경우 새로운 세이브 파일 생성
        CreateGameData();
    }
}

void UAGSDGameInstance::ResetGameData()
{
    // SaveGame 객체 생성
    USavingGame* SaveGameInstance = NewObject<USavingGame>();

    // 데이터를 초기값으로 저장
    SaveGameInstance->StageProgress = -1;
    SaveGameInstance->TalkingProgress = 0;
    
    // 임시 변수 초기화
    Temp_StageProgress = -1;
    Temp_TalkingProgress = 0;
    ResetWeaponDataTable(); 

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

void UAGSDGameInstance::ResetWeaponDataTable()
{
    if (!WeaponDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("ResetWeaponData: WeaponDataTable is NULL"));
        return;
    }
    
    static const FString ContextString(TEXT("ResetWeaponData"));
    TArray<FName> RowNames = WeaponDataTable->GetRowNames();

    for (FName RowName : RowNames)
    {
        FWeaponDataTableBetaStruct* Weapon = WeaponDataTable->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
        if (Weapon)
        {
            Weapon->bIsReinforce = false;
            if (Weapon->Sname == TEXT("Racket") || Weapon->Sname == TEXT("BaseBall"))
            {
                UE_LOG(LogTemp, Log, TEXT("ResetWeaponData: Skipped %s (Weapon: %s)"), *RowName.ToString(), *Weapon->Sname);
                continue;
            }
            //데이터 초기화
            Weapon->bIsAcquired = false;

            UE_LOG(LogTemp, Log, TEXT("ResetWeaponData: Reset %s"), *RowName.ToString());
        }
    }
    
}