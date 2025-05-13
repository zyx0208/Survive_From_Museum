// Fill out your copyright notice in the Description page of Project Settings.


#include "Damage_UI.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UDamage_UI::NativeConstruct()
{
    DamageText->SetVisibility(ESlateVisibility::Hidden);
    Construct();
    UpdateCanTick();
}
void UDamage_UI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    for (int i = DamageTextArray.Num()-1; i >=0 ; i--)
    {
        if (DamageTextArray[i]) {
            FLinearColor CurrentColor = DamageTextArray[i]->GetColorAndOpacity().GetSpecifiedColor();
            CurrentColor.A -= InDeltaTime/FadeDuration;
            DamageTextArray[i]->SetColorAndOpacity(FSlateColor(CurrentColor));
            if (CurrentColor.A <= 0.0f)
            {
                DamageTextArray[i]->RemoveFromParent();
                DamageTextArray.RemoveAt(i);
            }
        }
        
    }
}
void UDamage_UI::DamageTextCreate(float damage, FVector2D screenPosition)
{
    
    UTextBlock* NewDamage = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    if (RootCanvas && NewDamage) {
        UCanvasPanelSlot* canvasSlot = RootCanvas->AddChildToCanvas(NewDamage);
        if (canvasSlot) {
            NewDamage->SetText(FText::AsNumber(damage));
            NewDamage->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            NewDamage->SetVisibility(ESlateVisibility::HitTestInvisible);
            DamageTextArray.Add(NewDamage);
            float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
            canvasSlot->SetPosition(screenPosition/Scale);
            canvasSlot->SetSize(FVector2D(150.0f, 30.0f));
            canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
            canvasSlot->SetAutoSize(true);
            
        }
    }    
    else {
        UE_LOG(LogTemp, Log, TEXT("DamageNOTCreated"));
    }
}
