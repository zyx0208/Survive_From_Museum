// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDataTable.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "WeaponDrop.generated.h"

UCLASS()
class AGSD_API AWeaponDrop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDrop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //무기 메쉬 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WeaponMeshComponent;

    //무기 메쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMesh* CurrentWeaponMesh;

    //무기 ID
    UPROPERTY(BlueprintReadWrite,EditAnywhere)
    FString WeaponID = "0";

    INT32 WeaponIDInt = 0;

    //참조할 무기 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UDataTable* WeaponDataTableRef;

    UFUNCTION()
    void SetMesh();

    //오버랩 함수
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    //콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* CollisionComponent;

    //파괴
    UFUNCTION()
    void DestroySelf();

    // 말풍선 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> InteractionWidgetClass;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* InteractionWidget;
};
