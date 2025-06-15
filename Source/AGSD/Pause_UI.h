// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccessoryData.h"
#include "Pause_UI.generated.h"

class UWrapBox;
class UDataTable;

/**
 * 
 */
UCLASS()
class AGSD_API UPause_UI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    /** 액세서리 목록을 채우는 함수 */
    void PopulateAccessoryIcons();

    /** 캐릭터가 보유한 RowName 리스트 기반 */
    UPROPERTY()
    class AAGSDCharacter* PlayerCharacter;

    /** 액세서리 데이터 테이블 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    UDataTable* AccessoryDataTable;

    /** 액세서리 이미지 블루프린트 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    TSubclassOf<UUserWidget> AccessoryImageClass;

    /** WrapBox 위젯 (바인딩 필요) */
    UPROPERTY(meta = (BindWidget))
    UWrapBox* AccessoryWrapBox;

};
