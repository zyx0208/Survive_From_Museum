// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ManagingGame.h"
#include "AGSDGameInstance.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> TextError = NULL; //텍스트가 지정되지 않았을 경우
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text0_1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text0_2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text0_3 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text0_4 = NULL;//반복 대화문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text1_1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text1_2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text1_3 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text1_4 = NULL;//반복 대화문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text2_1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text2_2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text2_3 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text2_4 = NULL;//반복 대화문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text3_1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text3_2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text3_3 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text3_4 = NULL;//반복 대화문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text4_1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text4_2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text4_3 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text4_4 = NULL;//반복 대화문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<UUserWidget> Text5_1 = NULL;//엔딩 대사

    int64 StageProgress;//스태이지 진행도
    int64 TalkProgress;//대화 진행도
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    UAGSDGameInstance* GameInstance;
    void ShowTextUI();

    // 말풍선 위젯 클래스
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> InteractionWidgetClass;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* InteractionWidget;

private:
    ACharacter* PlayerCharacter;
    bool IsFirst = true;
    
};
