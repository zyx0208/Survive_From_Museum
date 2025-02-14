// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ManagingGame.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UManagingGame : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // 게임 데이터를 저장하는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void SaveGameData();

    // 저장된 게임 데이터를 불러오는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void LoadGameData();

    // 저장된 게임 데이터를 초기값으로 바꾸는 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void ResetGameData();

    //SavingGame.h에 있는 변수들(저장하기 전까진 임시 변수에 보관)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 Temp_StageProgress;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int64 Temp_TalkingProgress;
};
