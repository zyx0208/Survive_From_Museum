// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class AGSD_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyProjectile")float ShotSpeed = 0.0f;//발사체의 이동속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyProjectile")float Damage = 0.0f;//발사체의 공격력
    UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    // 충돌을 감지할 콜리전 컴포넌트
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* CollisionComponent;
private:
    bool IsFirst = true;//처음으로 실행되는 경우를 나타내기 위한 변수


};
