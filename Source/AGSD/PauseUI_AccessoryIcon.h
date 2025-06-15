// PauseUI_AccessoryIcon.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccessoryData.h"
#include "PauseUI_AccessoryIcon.generated.h"

class UImage;
class UPauseUI_AccessoryDetail;

UCLASS()
class AGSD_API UPauseUI_AccessoryIcon : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 외부에서 데이터 전달 시 호출 */
    UFUNCTION(BlueprintCallable, Category = "Accessory")
    void InitializeAccessoryIcon(const FAccessoryData& InData);

protected:
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    /** 아이콘 이미지 위젯 (BP에서 바인딩) */
    UPROPERTY(meta = (BindWidget))
    UImage* AccessoryIcon;

    /** 저장된 액세서리 데이터 */
    UPROPERTY()
    FAccessoryData AccessoryData;

    /** 상세 UI 호출 (기본 구현 + BP 오버라이드 허용) */
    UFUNCTION(BlueprintNativeEvent, Category = "Accessory")
    void ShowDetailUI(const FGeometry& InGeometry);
    void ShowDetailUI_Implementation(const FGeometry& InGeometry);

    UFUNCTION(BlueprintNativeEvent, Category = "Accessory")
    void HideDetailUI();
    void HideDetailUI_Implementation();

    UPROPERTY(EditAnywhere, Category = "Accessory")
    TSubclassOf<UUserWidget> DetailBalloonClass;

    UPROPERTY()
    UPauseUI_AccessoryDetail* SpawnedBalloon;
};
