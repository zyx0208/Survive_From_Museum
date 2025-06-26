// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "AccessoryData.h"
#include "DrawingBook_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UDrawingBook_UI : public UUserWidget
{
	GENERATED_BODY()

public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseDrawingBookButton;

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseDrawingBook();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> AccessoryIconClass;

    int32 GetThemePriority(const FString& Theme);
    int32 GetRarityPriority(EAccessoryRarity Rarity);

    UPROPERTY()
    class UPause_UI* PauseWidgetRef;

protected:
    //악세서리 아이콘 출력 함수
    void PopulateAccessoryIcons();
    


    /** 악세서리 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UDataTable* AccessoryDataTable;

    UPROPERTY(meta = (BindWidget))
    class UWrapBox* AccessoryWrapBox;
};
