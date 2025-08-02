// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainLoby_UI.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class AGSD_API UMainLoby_UI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void OpenDrawingBook();
    UPROPERTY()
    class UDrawingBook_UI* DrawingBookWidget;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UDrawingBook_UI> DrawingBookClass;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* Button_temp_collection;
};
