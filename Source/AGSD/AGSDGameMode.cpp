// Copyright Epic Games, Inc. All Rights Reserved.

#include "AGSDGameMode.h"
#include "AGSDCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "ManagingGame.h"

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

void AAGSDGameMode::BeginPlay()
{
    Super::BeginPlay(); // 부모 클래스의 BeginPlay 호출

    // 게임 인스턴스를 안전하게 캐스팅
    UManagingGame* GameInstance = Cast<UManagingGame>(GetGameInstance());

    if (GameInstance)
    {
        // 게임 인스턴스에서 데이터를 로드하는 함수 호출
        GameInstance->LoadGameData();
        UE_LOG(LogTemp, Warning, TEXT("Data Load Complete."));

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is null!"));
    }
}

void AAGSDGameMode::GameExit()
{
    UManagingGame* GameInstance = Cast<UManagingGame>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->SaveGameData();
    }
}

void AAGSDGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // 게임 종료 시 데이터 저장
    AAGSDGameMode::GameExit();
}