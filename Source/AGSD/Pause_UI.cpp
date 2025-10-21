// Fill out your copyright notice in the Description page of Project Settings.

#include "Pause_UI.h"
#include "AGSDCharacter.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "AccessoryData.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "DrawingBook_UI.h"
#include "InputCoreTypes.h"  
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPause_UI::NativeConstruct()
{
    Super::NativeConstruct();

    // 위젯이 키보드 포커스를 받을 수 있도록 설정 (매우 중요)
    SetIsFocusable(true);
    // 이 위젯이 열릴 때 키 입력을 받을 수 있도록 포커스/입력 모드 확보
    AcquireFocusForKeyboard();

    PlayerCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (AccessoryWrapBox && PlayerCharacter && AccessoryDataTable && AccessoryImageClass)
    {
        PopulateAccessoryIcons();
    }
    if (DrawingBoxButton)
    {
        DrawingBoxButton->OnClicked.RemoveDynamic(this, &UPause_UI::OpenDrawingBook);
        DrawingBoxButton->OnClicked.AddDynamic(this, &UPause_UI::OpenDrawingBook);
    }

}

void UPause_UI::PopulateAccessoryIcons()
{
    AccessoryWrapBox->ClearChildren();

    for (const FName& RowName : PlayerCharacter->GetAccessory)
    {
        const FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("PauseUI"));

        if (Accessory && Accessory->AccessoryIcon)
        {
            UUserWidget* IconWidget = CreateWidget<UUserWidget>(GetWorld(), AccessoryImageClass);
            if (IconWidget)
            {
                UFunction* InitFunc = IconWidget->FindFunction(FName("InitializeAccessoryIcon"));
                if (InitFunc)
                {
                    struct FDynamicAccessoryParam
                    {
                        FAccessoryData AccessoryData;
                    };

                    FDynamicAccessoryParam Param;
                    Param.AccessoryData = *Accessory;
                    IconWidget->ProcessEvent(InitFunc, &Param);
                }
                AccessoryWrapBox->AddChildToWrapBox(IconWidget);
            }
        }
    }
}

void UPause_UI::OpenDrawingBook()
{
    if (!DrawingBookClass) return;

    if (DrawingBookWidget && DrawingBookWidget->IsInViewport())
    {
        // 이미 열려있으면 중복 생성 방지
        return;
    }

    // 기존 Pause UI 제거
    RemoveFromParent();

    // DrawingBook UI 생성 및 추가
    DrawingBookWidget = CreateWidget<UDrawingBook_UI>(GetWorld(), DrawingBookClass);
    if (DrawingBookWidget)
    {
        DrawingBookWidget->PauseWidgetRef = this; // 닫기 후 돌아올 수 있도록 참조 전달
        DrawingBookWidget->AddToViewport();
    }
}

void UPause_UI::AcquireFocusForKeyboard()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        // Game & UI 모드로 전환하고, 이 위젯에 키보드 포커스 주기
        FInputModeGameAndUI InputMode;
        // UE5에서는 SetWidgetToFocus / SetLockMouseToViewportBehavior 사용
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);

        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(true);

        // 추가로 확실하게 포커스 부여 (위젯에 키보드 포커스)
        UWidgetBlueprintLibrary::SetFocusToGameViewport(); // (선택) 뷰포트 포커스 초기화
        FSlateApplication::Get().SetKeyboardFocus(TakeWidget(), EFocusCause::SetDirectly);
    }
}

FReply UPause_UI::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // 이 위젯이 화면에 떠 있고, ESC가 눌렸다면 닫기
    if (IsInViewport() && InKeyEvent.GetKey() == EKeys::Escape)
    {
        ClosePause();
        return FReply::Handled();
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UPause_UI::ClosePause()
{
    // 위젯 닫기
    RemoveFromParent();

    if (APlayerController* PC = GetOwningPlayer())
    {
        // 게임 재개
        UGameplayStatics::SetGamePaused(this, false);

        // 입력 모드 복귀 (게임 전용)
        FInputModeGameOnly GameOnlyMode;
        PC->SetInputMode(GameOnlyMode);
        PC->SetShowMouseCursor(false);
    }
}
