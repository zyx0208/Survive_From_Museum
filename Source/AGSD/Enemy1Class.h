// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy1Class.generated.h"

UCLASS()
class AGSD_API AEnemy1Class : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy1Class();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float MoveSpeed = 0;//해당 캐릭터의 이동속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackRange = 0;//해당 캐릭터의 공격길이

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackCooltime = 0;//해당 캐릭터의 공격속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackCooltimeFirstDelay = 0;//공격범위에 들어왔을 때, 첫 공격이 [AttackCooltime - AttackCooltime_first]초 뒤에 실행됨
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackDamage = 0;//해당 캐릭터의 공격력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")int64 MaxHP = 0;//해당 캐릭터의 최대체력 (숫자 범위 : -9,223,372,036,854,775,808 ~ 9,223,372,036,854,775,807)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")int64 CurrentHP = 0;//해당 캐릭터의 현재체력 (숫자 범위 : -9,223,372,036,854,775,808 ~ 9,223,372,036,854,775,807)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")int AttackType = 0;//해당 캐릭터의 공격 방식
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")bool IsAttacking;//해당 캐릭터가 현재 공격중인가?를 나타내는 변수(공격중일때는 추적을 멈춤)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> AttackEffect1 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> AttackEffect2 = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> EnemyProjectile = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> EXball = NULL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> WeaponDrop = NULL;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float DashCoolTime = 0.0f;//대쉬가 있는 적을 위한 설정 기능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")bool Dash = false;//대쉬 기능이 있을경우 true로 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float FDash = 0.0f;//대쉬 계수(몇배속으로 할 것인가?)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")bool IsDead = false;//대쉬 기능이 있을경우 true로 설정

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
