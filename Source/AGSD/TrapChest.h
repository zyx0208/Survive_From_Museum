// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "TrapTimerWidget.h" 
#include "TrapChest.generated.h"


UCLASS()
class AGSD_API ATrapChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapChest();
    void Interact(class AAGSDCharacter* Character);

protected:
    virtual void BeginPlay() override;

    // 맵에 미리 배치한 문 액터를 에디터에서 연결
    UPROPERTY(EditInstanceOnly, Category = "Trap")
    AActor* DoorActor = nullptr;

    // 문을 활성화할 때 콜리전도 같이 켤지
    UPROPERTY(EditAnywhere, Category = "Trap")
    bool bEnableDoorCollisionOnTrap = true;

    // 문이 켜진 뒤 유지 시간(초). 끝나면 문 Destroy()
    UPROPERTY(EditAnywhere, Category = "Trap", meta = (ClampMin = "0.0"))
    float TrapDuration = 20.0f;

    // 레벨업 확률(0~1). 0.3 = 30%
    UPROPERTY(EditAnywhere, Category = "Trap", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LevelUpProbability = 0.3f;

    // 한 번만 사용되도록
    UPROPERTY(VisibleAnywhere, Category = "Trap")
    bool bUsed = false;

    UPROPERTY(VisibleAnywhere, Category = "Trap")
    USphereComponent* InteractionArea = nullptr;

    UPROPERTY(EditAnywhere, Category = "Trap", meta = (ClampMin = "0.0"))
    float InteractionRadius = 350.f;

    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, Category = "Trap|UI")
    TSubclassOf<UTrapTimerWidget> TimerWidgetClass;

    UPROPERTY()
    UTrapTimerWidget* TimerWidgetInstance = nullptr;

private:
    // 문 보이기/숨기기 + 충돌 토글
    void SetActorActiveWithCollision(AActor* Target, bool bActive);

    // 문 삭제 타이머 핸들/포인터
    FTimerHandle DoorDestroyHandle;
    FTimerHandle DoorTickHandle;

    // 문 파괴 예정 시간(월드 초 단위)
    float DoorEndTimeSeconds = 0.f;

    // 문 카운트다운 갱신
    void UpdateDoorCountdown();

    TWeakObjectPtr<AActor> PendingDoor;

};
