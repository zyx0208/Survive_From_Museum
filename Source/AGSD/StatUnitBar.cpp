// Fill out your copyright notice in the Description page of Project Settings.


#include "StatUnitBar.h"
#include "Animation/UMGSequencePlayer.h"

void UStatUnitBar::PlayBlink()
{
    if (Blink)
    {
        PlayAnimation(Blink, 0.f, 1, EUMGSequencePlayMode::Forward, 1.0f); 
    }
}
