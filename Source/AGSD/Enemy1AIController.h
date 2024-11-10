// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy1AIController.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API AEnemy1AIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//이 코드가 설정된 블루프린트 캐릭터마다 값을 수정할 수 있도록 public 및 UPROPERTY 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float MoveSpeed = 0;//해당 캐릭터의 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackRange = 0;//해당 캐릭터의 공격길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackCooltime = 0;//해당 캐릭터의 공격속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackCooltimeFirstDelay = 0;//공격범위에 들어왔을 때, 첫 공격이 [AttackCooltime - AttackCooltime_first]초 뒤에 실행됨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float AttackDamage = 0;//해당 캐릭터의 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")int64 HP = 0;//해당 캐릭터의 체력 (숫자 범위 : -9,223,372,036,854,775,808 ~ 9,223,372,036,854,775,807)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")int AttackType = 0;//해당 캐릭터의 공격 방식
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")bool IsAttacking;//해당 캐릭터가 현재 공격중인가?를 나타내는 변수(공격중일때는 추적을 멈춤)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> AttackEffect1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")TSubclassOf<AActor> AttackEffect2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")float DashCoolTime;//대쉬가 있는 적을 위한 설정 기능
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")bool Dash = false;//대쉬 기능이 있을경우 true로 설정
	virtual void Attacked();//플레이어의 공격을 맞았을 경우
	virtual void Died(int64 num);//죽을 경우(매개변수는 적 드롭 아이템 등을 설정하기 위한 수)
	int BossCount = 0; //보스 공격 횟수를 측정하기 위한 함수

private:
	ACharacter* PlayerCharacter; //플레이어 위치 및 방향 정보를 담는 액터 배열
	float AttackCooltime_temp; //공격속도 계산을 위한 임의의 변수
	bool IsFisrt;//C++과 블루프린트에서 begin 함수보다 반드시 늦게 실행해야 하지만 한번만 실행해야 되는 함수를 사용하기 위해
	bool TickSwitch;//게임 실행 중에 짝수 혹은 홀수의 틱마다 발동되는 코드를 작성하기 위함
	virtual void AttackTypeA();//1번 공격 타입 : 근거리공격
	virtual void AttackTypeB();//2번 공격 타입 : 보스1
	virtual void AttackTypeC();//3번 공격 타입 : 
	virtual void AttackTypeD();//4번 공격 타입 : 
	FVector ChenkIsolated_1;//적이 탐색이 됐는데 움직이지 않을 경우(버그)를 위한 변수1
	FVector ChenkIsolated_2;//적이 탐색이 됐는데 움직이지 않을 경우(버그)를 위한 변수2
	float ChenkIsolated_Timer;//적이 탐색이 됐는데 움직이지 않을 경우(버그)를 위한 변수3
	float DashCoolTime_Temp = 0;//대쉬 쿨타임을 계산하기 위한 변수
	bool IsDashing = false;//대쉬중인가를 나타내는 변수
};
