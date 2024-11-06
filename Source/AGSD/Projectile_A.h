// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WeaponDataTable.h"
#include "Enemy.h"
#include "Projectile_A.generated.h"

UCLASS()
class AGSD_API AProjectile_A : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile_A();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 탄환 출발지점 저장
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	FVector StartLocation;

	// 탄환 속도, 사거리, 데미지 변수
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	float ProjectileSpeed;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	float ProjectileRange;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	float ProjectileDamage;

	// 탄환 충돌
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// 탄환 이동
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// 탄환 방향 함수
	void FireInDirection(const FVector& ShootDirection);

	// 탄환 매쉬
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;

	// 탄환 마테리얼
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// 충돌판정 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// 탄환 메쉬, 마테리얼 설정
	UFUNCTION()
	void SetProjectileMeshAndMarterial(UStaticMesh* ProjectileMesh, UMaterialInterface* ProjectileMaterial);

	// 탄환 속도, 데미지, 사거리 설정
	UFUNCTION()
	void SetProjectileSpeedDamageAndRange(float Speed, float Damage, float Range);

	//데이터 테이블 참조
	UPROPERTY()
	FString WeaponID;
	//데이터 테이블 참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* WeaponDataTable;
	//데이터 테이블 참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* WeaponDataTableRef;

};
