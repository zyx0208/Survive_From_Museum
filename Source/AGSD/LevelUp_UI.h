// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "AccessoryData.h" // 장신구 데이터 구조체 헤더
#include "LevelUp_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API ULevelUp_UI : public UUserWidget
{
	GENERATED_BODY()

protected:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
    UDataTable* AccessoryDataTable;

    UPROPERTY(BlueprintReadWrite, Category = "LevelUp")
    TArray<FAccessoryData> SelectedAccessories;


    void SelectRandomAccessories();
    void UpdateAccessoryVFX(int index, EAccessoryRarity rarity);

public:
    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* Option1;

    UPROPERTY(meta = (BindWidget))
    class UButton* Option2;

    UPROPERTY(meta = (BindWidget))
    class UButton* Option3;

    // 텍스트 블록 변수
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option1_Name;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option1_Effect;
    UPROPERTY(meta=(BindWidget))
    class UTextBlock* Option1_Description;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option1_Img;


    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option2_Name;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option2_Effect;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option2_Description;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option2_Img;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option3_Name;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option3_Effect;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Option3_Description;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option3_Img;

    // 인덱스와 텍스트 데이터를 관리할 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
    TArray<int32> OptionIndices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
    TArray<FText> OptionTexts;

    // 텍스트를 업데이트하는 함수
    UFUNCTION(BlueprintCallable, Category = "LevelUp")
    void UpdateOptionTexts();

    // 버튼 클릭 이벤트 처리
    UFUNCTION()
    void OnOption1Clicked();

    UFUNCTION()
    void OnOption2Clicked();

    UFUNCTION()
    void OnOption3Clicked();


    TArray<UImage*> AccessoryRareEffect;
    TArray<UImage*> AccessoryLegendaryEffect;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option1RareEffect;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option2RareEffect;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option3RareEffect;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option1LegendaryEffect;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option2LegendaryEffect;
    UPROPERTY(meta = (BindWidget))
    class UImage* Option3LegendaryEffect;

    //클릭 미스 방지
    UFUNCTION()
    void EnableButtons();
    UFUNCTION()
    void UnableButtons();

};
