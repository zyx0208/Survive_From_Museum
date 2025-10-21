// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccessoryData.h"
#include "Pause_UI.generated.h"

class UWrapBox;
class UTextBlock;
class UDataTable;
class UButton;

/**
 * 
 */
UCLASS()
class AGSD_API UPause_UI : public UUserWidget
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


    /*Drawing UI*/
    UPROPERTY(meta = (BindWidget))
    UButton* DrawingBoxButton;

    // ESC 잡기 (Preview 단계에서 먼저 가로채는 걸 권장)
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    // 만약 Preview 대신 KeyDown을 쓰고 싶다면 아래도 가능
    // virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    UFUNCTION()
    void ClosePause();
    
private:
    void AcquireFocusForKeyboard(); // 포커스/입력모드 세팅 헬퍼

};
