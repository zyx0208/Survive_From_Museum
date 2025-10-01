//해당 코드는 게임에서 저장이 필요한 변수만을 가지고 있는 코드임
//저장작업 및 불러오기는 다른 코드에 존재

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SavingGame.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API USavingGame : public USaveGame
{
	GENERATED_BODY()

public:
    //스테이지 진행 정보(초기값 : 0)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 StageProgress;
    //로비 NPC의 대화정보(초기값 : 0)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 TalkingProgress;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    float BGMVolume;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    float SFXVolume;
    //무기 데이터테이블 정보(초기값: 야구공, 테니스 라켓 획득)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveData")
    UDataTable* SavingWeaponData;

    //각 무기 마다 저장하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TMap<FName, int>SWeapon_Ascension;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TMap<FName, bool>SWeapon_Acquired;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TMap<FName, bool>SWeapon_Reinforced;

    //들고 있는 무기
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<int> SWeapon_Array;

    //악세서리 저장 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
    TArray<FName> SaveKnowAccessory;
};
