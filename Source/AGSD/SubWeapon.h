// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile_Sub.h"
#include "SubWeapon.generated.h"

UCLASS()
class AGSD_API ASubWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASubWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
    float Rate;//보조 무기 공격 속도

    int NumberOfProjectile;//보조 무기 탄환 갯수

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    UStaticMeshComponent* WeaponMeshComponent;//보조 무기 메쉬 컴포넌트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* WeaponMesh;//보조 무기 메쉬

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FString WeaponID;//무기 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UDataTable* WeaponDataTable;//무기 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UDataTable* WeaponDataTableRef;//참조할 테이블

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    TSubclassOf<class AProjectile_Beta> ProjectileClass;

    //무기 타이머
    FTimerHandle FireTimerHandle;
    FTimerHandle FireRateTimerHandle;

    //탄환 각도
    float SpreadAngle;

    //무기 발사
    UFUNCTION()
    virtual void Fire();

    UFUNCTION()
    virtual void StartFiring();

    UFUNCTION()
    virtual void CreateProjectile();
};
