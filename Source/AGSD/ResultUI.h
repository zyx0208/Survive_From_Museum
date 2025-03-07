// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultUI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UResultUI : public UUserWidget
{
	GENERATED_BODY()

public:
    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseResultButton;
};
