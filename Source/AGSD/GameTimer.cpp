// Fill out your copyright notice in the Description page of Project Settings.


#include "GameTimer.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UGameTimer::UGameTimer() {
    InitTime = 10.0f;
    TimeEnd = false;
}

void UGameTimer::TimeSet(float Time)
{
    UITime = Time;
    if (GWorld)
    {
        GWorld->GetTimerManager().SetTimer(GameTimer, this, &UGameTimer::UpdateTime, 1.0f, true);
    }
}

void UGameTimer::UpdateTime()
{
    if (UITime > 0)
    {
        UITime--;//정수형 시간 --
        StringTime = MinutesSeconds(UITime);//UItime을 분초 변환 후 문자열로
    }
    else
    {
        TimeOver();//정수형 시간이 0이되면 TimeOver
    }
}

void UGameTimer::TimeOver()
{
    if (GWorld)
    {
        GWorld->GetTimerManager().ClearTimer(GameTimer);
    }
    StringTime = "Boss Stage";
    TimeEnd = true;

    // 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("TimeOver called. TimeEnd is now true."));
}

FString UGameTimer::MinutesSeconds(int Time)
{
    int Minutes = Time / 60;
    int Seconds = Time % 60;
    return FString::Printf(TEXT("Time : %02d:%02d"), Minutes, Seconds);
}

