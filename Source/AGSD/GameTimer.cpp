// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTimer.h"

UGameTimer::UGameTimer() {
    InitTime = 10.0f;
}

void UGameTimer::TimeSet()
{
    GetWorld()->GetTimerManager().SetTimer(GameTimer, this, &UGameTimer::UpdateTime, 1.0f, true);
    UITime = InitTime;
    StringTime = MinutesSeconds(UITime);
}

void UGameTimer::UpdateTime()
{
    UITime--;
    StringTime = MinutesSeconds(UITime);
    if (UITime <= 0) {
        TimeOver();
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
