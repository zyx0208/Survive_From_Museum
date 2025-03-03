// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "AGSDGameMode.generated.h"

UCLASS(minimalapi)
class AAGSDGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
    // BeginPlay 오버라이드
    virtual void BeginPlay() override;

public:
	AAGSDGameMode();

    //게임 종료 시 저장을 위한 함수
    UFUNCTION(BlueprintCallable, Category = "SaveGame")
    void GameExit();
    void ResetAccessoryData();

protected:
    //자동 저장 함수
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Accessory")
    UDataTable* AccessoryDataTable;
};



