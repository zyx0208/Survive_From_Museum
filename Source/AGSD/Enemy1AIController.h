// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy1Class.h"
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
	virtual void Attacked(float damage);//플레이어의 공격을 맞았을 경우(무적시간X)
    virtual void Attacked(float damage, int chanel);//플레이어의 공격을 맞았을 경우(동일 채널 내 무적 시간 존재)
	virtual void Died(int64 num);//죽을 경우(매개변수는 적 드롭 아이템 등을 설정하기 위한 수)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")int BossCount = 0; //보스 공격 횟수를 측정하기 위한 함수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")float AttackCooltime_temp = 0.0f; //공격속도 계산을 위한 임의의 변수

    void KillCountCall(UWorld* World);

    FTimerHandle Chanel1TimerHandle;//무적 관리를 위한 함수1
    void Chanel1TimerEnd();//무적 해제를 위한 함수1
    FTimerHandle Chanel2TimerHandle;//무적 관리를 위한 함수2
    void Chanel2TimerEnd();//무적 해제를 위한 함수2
    FTimerHandle Chanel3TimerHandle;//무적 관리를 위한 함수3
    void Chanel3TimerEnd();//무적 해제를 위한 함수3
    FTimerHandle Chanel4TimerHandle;//무적 관리를 위한 함수4
    void Chanel4TimerEnd();//무적 해제를 위한 함수4
    FTimerHandle Chanel5TimerHandle;//무적 관리를 위한 함수5
    void Chanel5TimerEnd();//무적 해제를 위한 함수5

    UFUNCTION(BlueprintCallable, Category = "Test")
    void Stun(float duration);//스턴 상태를 나타내기 위한 함수
    FTimerHandle StunTimer;//기절 시간을 관리하기 위한 타이머
    void StunTimerEnd();//기절 해제를 관리하기 위한 함수
    bool IsStun;//기절 상태인지를 나타내는 변수

private:
    UPROPERTY()
    TWeakObjectPtr<AEnemy1Class> Enemy; //Enemy 캐릭터의 속성을 불러오기 위함(체력, 공격력 등)
	ACharacter* PlayerCharacter; //플레이어 위치 및 방향 정보를 담는 액터 배열
	bool IsFisrt = true;//C++과 블루프린트에서 begin 함수보다 반드시 늦게 실행해야 하지만 한번만 실행해야 되는 함수를 사용하기 위해
	bool TickSwitch = false;//게임 실행 중에 짝수 혹은 홀수의 틱마다 발동되는 코드를 작성하기 위함
	virtual void AttackTypeA();//1번 공격 타입 : 근거리
	virtual void AttackTypeB();//2번 공격 타입 : 왕자 보스
	virtual void AttackTypeC();//3번 공격 타입 : 공격모션이 없는 근거리
	virtual void AttackTypeD();//4번 공격 타입 : 고릴라 보스
    virtual void AttackTypeE();//5번 공격 타입 : 원거리
	FVector ChenkIsolated_1 = FVector(0.0f, 0.0f, 0.0f);//적이 탐색이 됐는데 움직이지 않을 경우(버그)를 위한 변수1
	FVector ChenkIsolated_2 = FVector(0.0f, 0.0f, 0.0f);//적이 탐색이 됐는데 움직이지 않을 경우(버그)를 위한 변수2
	float ChenkIsolated_Timer = 0.0f;//적이 탐색이 됐는데 움직이지 않을 경우(버그)를 위한 변수3
	float DashCoolTime_Temp = 0;//대쉬 쿨타임을 계산하기 위한 변수
	bool IsDashing = false;//대쉬중인가를 나타내는 변수
    bool Temp_Dead = false;//죽음판정을 빨리 계산하기 위한 변수
    bool IsPlayingAnim; //몬스터 애니메이션 관련 변수
};
