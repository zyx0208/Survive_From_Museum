// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccessoryData.h"
#include "PauseUI_AccessoryDetail.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AGSD_API UPauseUI_AccessoryDetail : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** 텍스트 초기화 함수 */
    UFUNCTION(BlueprintCallable, Category = "Accessory")
    void SetAccessoryDetail(const FAccessoryData& InData);

protected:
    /** 바인딩된 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EffectText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DescriptionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    FVector2D ViewportAlignment = FVector2D(0.5f, 0.5f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    FVector2D FollowOffset = FVector2D(12.f, 12.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    bool bFollowMouse = true;
    /** 화면 가장자리와의 최소 여백(픽셀) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    float EdgePadding = 8.f;


};
