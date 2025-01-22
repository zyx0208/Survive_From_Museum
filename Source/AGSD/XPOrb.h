// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "XPOrb.generated.h"

UCLASS()
class AGSD_API AXPOrb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXPOrb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 구슬의 충돌을 감지하는 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class USphereComponent* SphereComponent;

	// 구체 오브젝트 (XPOrb)
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	class UStaticMeshComponent* OrbMesh;

    UPROPERTY()
    class UMaterialInstanceDynamic* MaterialInstance; // Dynamic Material Instance


	// 충돌 이벤트 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 경험치량 설정 함수
	void SetRandomXP();
	// 크기 조절 함수
	void AdjustScaleBasedOnXP();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP", meta = (AllowPrivateAccess = "true"))
    int32 XPValue;

private:
    bool bIsConsumed = false; // 이미 흡수되었는지 확인

};
