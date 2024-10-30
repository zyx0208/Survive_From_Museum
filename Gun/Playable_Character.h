// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Projectile_A.h"
#include "WeaponDataTable.h"
#include "Playable_Character.generated.h"


UCLASS()
class GUN_API APlayable_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayable_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectile_A> ProjectileClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	//무기 발사
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void StartFiring();

	UFUNCTION()
	void StopFiring();

	//무기 공격속도
	float FireRate;

	//무기 타이머
	FTimerHandle FireTimerHandle;

	FTimerHandle FireRateTimerHandle;

	//무기 탄환 숫자
	int Numberofprojectile;

	//탄환 각도
	float SpreadAngle;

	//무기 교체
	UFUNCTION()
	void WeaponSwap();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	//들고 있는 무기 데이터
	UPROPERTY()
	FString WeaponID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* WeaponDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* WeaponDataTableRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UStaticMesh* CurrentProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UMaterialInterface* CurrentProjectileMaterial;

	float CurrentWeaponDamage;

	float CurrentWeaponRange;

	float CurrentWeaponProjectileSpeed;

	float CurrentFireRateTime;
};
