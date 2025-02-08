// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC1Class.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "AGSDCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/InputComponent.h"


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
    if (IsFirst)
    {
        PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        IsFirst = false;
        Progress = 1;
    }
    if (PlayerCharacter)
    {
        if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= 500.0f) && (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::F)))//대화가능 거리 및 상호작용 키[인터렉션으로 교체 예정]
        {
            switch (Progress)
            {
            case 1:
                if (Text1)
                {
                    CreateWidget<UUserWidget>(GetWorld(), Text1)->AddToViewport();
                    Progress++;
                    break;
                }
            case 2:
                if (Text2)
                {
                    CreateWidget<UUserWidget>(GetWorld(), Text2)->AddToViewport();
                    Progress++;
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

