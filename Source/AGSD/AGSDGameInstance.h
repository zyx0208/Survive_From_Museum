// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AGSDGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UAGSDGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // 게임 인스턴스 초기화 시 호출되는 함수
    virtual void Init() override;
};
