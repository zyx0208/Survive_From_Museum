// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ManagingGame.h"
#include "AGSDGameInstance.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC1Class.generated.h"


UCLASS()
class AGSD_API ANPC1Class : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC1Class();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> TextLast = NULL;//반복 대화문
    int64 Progress;//어떤 대화창을 출력할지를 위한 변수
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    UAGSDGameInstance* GameInstance;
    void ShowTextUI();
private:
    ACharacter* PlayerCharacter;
    bool IsFirst = true;
    
};
