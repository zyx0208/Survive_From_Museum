// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "AccessoryData.h" 
#include "ResultUI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UResultUI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseResultButton;

    UFUNCTION(BlueprintCallable)
    void CloseResult();

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ResultScoreText;

    UPROPERTY(meta = (BindWidget))
    class UWrapBox* AccessoryWrapBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    UDataTable* AccessoryDataTable;

    void PopulateAccessoryIcons();

};
