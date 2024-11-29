// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTimer.h"

UGameTimer::UGameTimer() {
    InitTime = 10.0f;
}

void UGameTimer::TimeSet(float Time)
{
    GetWorld()->GetTimerManager().SetTimer(GameTimer, this, &UGameTimer::UpdateTime, 1.0f, true);//타이머 생성
    UITime = Time;//입력값을 UItime으로 설정
    StringTime = MinutesSeconds(UITime);//UItime을 분초 변환 후 문자열로
}

void UGameTimer::UpdateTime()
{
    UITime--;//정수형 시간 --
    StringTime = MinutesSeconds(UITime);//UItime을 분초 변환 후 문자열로
    if (UITime <= 0) {
        TimeOver();//정수형 시간이 0이되면 TimeOver
    }
}

void UGameTimer::TimeOver()
{
    GetWorld()->GetTimerManager().ClearTimer(GameTimer);
    StringTime = "LevelEnd";
    TimeEnd = true;
}

FString UGameTimer::MinutesSeconds(int Time)
{
    int Minutes = Time / 60;
    int Seconds = Time % 60;
    return FString::Printf(TEXT("Time : %02d:%02d"), Minutes, Seconds);
}
