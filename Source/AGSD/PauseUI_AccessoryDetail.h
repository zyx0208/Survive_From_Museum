// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccessoryData.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PauseUI_AccessoryDetail.generated.h"

class UTextBlock;
class SizeBox;
struct FGeometry;

/**
 * 
 */
UCLASS()
class AGSD_API UPauseUI_AccessoryDetail : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** 텍스트 초기화 함수 */
    UFUNCTION(BlueprintCallable, Category = "Accessory")
    void SetAccessoryDetail(const FAccessoryData& InData);

    void UpdatePositionToMouse(bool bClampToViewport, const FGeometry* GeometryPtr=nullptr);
    FVector2D ComputeWidgetSize(const FGeometry* GeometryPtr) const;

    // Pause UI가 닫힐 때 일괄 정리용
    UFUNCTION(BlueprintCallable)
    static void CloseAllInWorld(UWorld* World);

protected:
    /** 바인딩된 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EffectText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DescriptionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    FVector2D ViewportAlignment = FVector2D::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    FVector2D FollowOffset = FVector2D::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    bool bFollowMouse = true;
    /** 화면 가장자리와의 최소 여백(픽셀) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow")
    float EdgePadding = 4.f;

    UPROPERTY(meta = (BindWidget)) // BP의 SizeBox(ContentRoot)에 Bind
    class USizeBox* ContentRoot = nullptr;

    void HandleViewportResized(class FViewport* Viewport, uint32);
    void ReapplyLayoutNow();
    void ReapplyLayoutNextTick();

private:
    FDelegateHandle ViewportResizedHandle;
    FTimerHandle ReapplyNextTickHandle;
    bool GetMouseViewportPositionPx(FVector2D& OutViewportPos) const;

};
