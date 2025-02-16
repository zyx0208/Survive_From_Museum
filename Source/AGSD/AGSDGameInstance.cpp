// Fill out your copyright notice in the Description page of Project Settings.


#include "AGSDGameInstance.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"

void UAGSDGameInstance::Init()
{
    Super::Init();

    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (UserSettings)
    {
        UserSettings->SetScreenResolution(FIntPoint(1920, 1080));  // 원하는 해상도
        UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
        UserSettings->ApplySettings(false);
    }
}

