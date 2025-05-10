// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC1Class.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "AGSDCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/InputComponent.h"
#include "ManagingGame.h"
#include "Engine/GameInstance.h"
#include "AGSDGameInstance.h"


// Sets default values
ANPC1Class::ANPC1Class()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANPC1Class::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC1Class::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    //처음 실행 과정
    if (IsFirst)
    {
        PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        IsFirst = false;
        GameInstance = Cast<UAGSDGameInstance>(GetGameInstance());
    }

    //대화 진행도 설정
    if (GameInstance)
    {
        TalkProgress = GameInstance->Temp_TalkingProgress;
        StageProgress = GameInstance->Temp_StageProgress;
    }
    else
    {
        TalkProgress = 0;
    }
}

// Called to bind functionality to input
void ANPC1Class::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANPC1Class::ShowTextUI()
{
    UE_LOG(LogTemp, Warning, TEXT("Stage : %d, Talk : %d"), StageProgress, TalkProgress);
    switch (StageProgress)
    {
    case 0:
        switch (TalkProgress)
        {
        case -1:
        case 0:
            if (Text0_1)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text0_1)->AddToViewport();
                GameInstance->Temp_TalkingProgress = 1;
                break;
            }
        case 1:
            if (Text0_2)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text0_2)->AddToViewport();
                GameInstance->Temp_TalkingProgress++;
                break;
            }
        case 2:
            if (Text0_3)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text0_3)->AddToViewport();
                break;
            }
        case 3:
            if (Text0_4)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text0_4)->AddToViewport();
                break;
            }
        default:
            if (TextError)
            {
                CreateWidget<UUserWidget>(GetWorld(), TextError)->AddToViewport();
            }
            break;
        }
        break;
    case 1:
        switch (TalkProgress)
        {
        case -1:
        case 0:
            if (Text1_1)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text1_1)->AddToViewport();
                GameInstance->Temp_TalkingProgress = 1;
                break;
            }
        case 1:
            if (Text1_2)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text1_2)->AddToViewport();
                GameInstance->Temp_TalkingProgress++;
                break;
            }
        case 2:
            if (Text1_3)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text1_3)->AddToViewport();
                break;
            }
        case 3:
            if (Text1_4)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text1_4)->AddToViewport();
                break;
            }
        default:
            if (TextError)
            {
                CreateWidget<UUserWidget>(GetWorld(), TextError)->AddToViewport();
            }
            break;
        }
        break;
    case 2:
        switch (TalkProgress)
        {
        case -1:
        case 0:
            if (Text2_1)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text2_1)->AddToViewport();
                GameInstance->Temp_TalkingProgress = 1;
                break;
            }
        case 1:
            if (Text2_2)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text2_2)->AddToViewport();
                GameInstance->Temp_TalkingProgress++;
                break;
            }
        case 2:
            if (Text2_3)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text2_3)->AddToViewport();
                break;
            }
        case 3:
            if (Text2_3)
            {
                CreateWidget<UUserWidget>(GetWorld(), Text2_4)->AddToViewport();
                break;
            }
        default:
            if (TextError)
            {
                CreateWidget<UUserWidget>(GetWorld(), TextError)->AddToViewport();
            }
            break;
        }
        break;
    default:
        if (TextError)
        {
            CreateWidget<UUserWidget>(GetWorld(), TextError)->AddToViewport();
        }
        break;
    }

    
}