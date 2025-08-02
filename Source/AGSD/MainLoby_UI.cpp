// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLoby_UI.h"
#include "Components/Button.h"
#include "DrawingBook_UI.h"

void UMainLoby_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_temp_collection)
    {
        Button_temp_collection->OnClicked.RemoveDynamic(this, &UMainLoby_UI::OpenDrawingBook);
        Button_temp_collection->OnClicked.AddDynamic(this, &UMainLoby_UI::OpenDrawingBook);
    }
}

void UMainLoby_UI::OpenDrawingBook()
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
        DrawingBookWidget->MainWidgetRef = this; // 닫기 후 돌아올 수 있도록 참조 전달
        DrawingBookWidget->AddToViewport();
    }
}