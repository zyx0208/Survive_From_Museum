// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "Damage_UI.h"
#include "Engine/StreamableManager.h"
#include "AGSDGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UAGSDGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // 게임 인스턴스 초기화 시 호출되는 함수
    virtual void Init() override;
    virtual void Shutdown() override;

    //액터 로딩 함수
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void FirstLoading();

    //레벨 로딩 함수
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void SecondLoading();

    //레벨 로딩 함수 반복을 위한 카운터 함수
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void OnSingleLevelLoaded();

    //로딩이 끝난 후에 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "Loading")
    void EndLoading();

    // 게임 데이터 슬롯을 생성하는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void CreateGameData();

    // 게임 데이터를 저장하는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void SaveGameData();

    // 저장된 게임 데이터를 불러오는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void LoadGameData();

    // 저장된 게임 데이터를 초기값으로 바꾸는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void ResetGameData();

    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void ResetWeaponDataTable();

    //SavingGame.h에 있는 변수들(저장하기 전까진 임시 변수에 보관)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 Temp_StageProgress;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 Temp_TalkingProgress;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    float Temp_BGMVolume;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    float Temp_SFXVolume;

    //현재 진행 중인 스테이지 정보를 불러오기 위한 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 NowStage;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 ReinforceWeaponIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 ReinforceWeaponIndex2;
    //대미지UI접근
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UUserWidget* DamageUIInstance;

    //캐릭터 무기
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<int> WeaponArray;

    //로딩 화면
    UPROPERTY(EditAnywhere, Category = "Loading")
    TSubclassOf<class UUserWidget> LoadingWidgetClass;

    //레벨 로딩 에셋들
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
    TArray<TSoftObjectPtr<UWorld>> LoadingLevel;

    //액터 로딩 에셋들
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
    TArray<TSoftClassPtr<AActor>> LoadingActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool IsLoadingEnd;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTable;

private:
    //로딩 UI
    UPROPERTY()
    UUserWidget* LoadingWidget;

    FStreamableManager StreamableManager;

    //액터는 배열을 쓰면 여러개의 로딩이 끝난 뒤 한번만 콜백 함수가 작동하지만
    //레벨은 한개한개마다 불러오는 기능을 써서 콜백이 여러번 되기 때문에 이를 처리할 카운터가 필요
    int32 LoadedLevelCount = 0;
};
