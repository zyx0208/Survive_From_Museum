// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "DrawingBook.generated.h"

UCLASS()
class AGSD_API ADrawingBook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrawingBook();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    // 박스의 충돌을 감지하는 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Collision")
    class USphereComponent* SphereComponent;

    // 박스 오브젝트 
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    class UStaticMeshComponent* BoxMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // 말풍선 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> InteractionWidgetClass;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* InteractionWidget;
};
