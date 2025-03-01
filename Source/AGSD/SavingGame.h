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
    //무기 데이터테이블 정보(초기값: 야구공, 테니스 라켓 획득)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveData")
    UDataTable* SavingWeaponData;
};
