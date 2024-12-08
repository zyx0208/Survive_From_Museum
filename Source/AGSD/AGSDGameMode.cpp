// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGSDGameMode.h"
#include "AGSDCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAGSDGameMode::AAGSDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
    if (GEngine)
    {
        GEngine->bEnableOnScreenDebugMessages = false; // 디버그 메시지 끄기
    }
}
