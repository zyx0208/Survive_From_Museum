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
        GameInstance = Cast<UManagingGame>(GetGameInstance());
    }

    //대화 진행도 설정
    if (GameInstance)
    {
        Progress = GameInstance->Temp_TalkingProgress;
    }
    else
    {
        Progress = 0;
    }

    //대화 출력
    if (PlayerCharacter)
    {
        if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= 500.0f) && (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::F)))//대화가능 거리 및 상호작용 키[인터렉션으로 교체 예정]
        {
            UE_LOG(LogTemp, Warning, TEXT("Progress : %d"), Progress);
            switch (Progress)
            {
            case 0:
                if (Text1)
                {
                    CreateWidget<UUserWidget>(GetWorld(), Text1)->AddToViewport();
                    GameInstance->Temp_TalkingProgress++;
                    break;
                }
            case 1:
                if (Text2)
                {
                    CreateWidget<UUserWidget>(GetWorld(), Text2)->AddToViewport();
                    GameInstance->Temp_TalkingProgress++;
                    break;
                }
            default:
                if (TextLast)
                {
                    CreateWidget<UUserWidget>(GetWorld(), TextLast)->AddToViewport();
                }
                break;
            }

        }
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Dont Find Player!"));
    }
}

// Called to bind functionality to input
void ANPC1Class::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

