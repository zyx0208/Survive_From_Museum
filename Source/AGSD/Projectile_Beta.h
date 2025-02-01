// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Enemy.h"
#include "Projectile_Beta.generated.h"

UCLASS()
class AGSD_API AProjectile_Beta : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile_Beta();

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile)
	float ProjectileSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile)
	float ProjectileRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile)
	float ProjectileDamage;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile)
	float PlayerAttack;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Projectile)
    float PlayerRange;

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
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void UpdatePlayerStat();

	// 무기 적중시 효과
	UFUNCTION()
	virtual void WeaponHitEffect();
};
