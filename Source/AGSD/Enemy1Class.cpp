// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy1Class.h"
#include "Enemy1AIController.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "GameFramework/Character.h"//레벨에 등장하는 액터의 방향이나 이동을 조절하기 위한 함수를 가진 헤더파일
#include "GameFramework/CharacterMovementComponent.h"//캐릭터의 움직임을 제어하는 함수를 참조하기 위한 헤더파일
#include "DrawDebugHelpers.h"//범위 시각화를 위한 헤더파일
#include "KillCount.h"//킬카운트용
#include "Engine/LevelScriptActor.h"//레벨블루프린트
#include "AGSDCharacter.h"
#include "Math/UnrealMathUtility.h"//랜덤 수 추출을 위한 헤더

// Sets default values
AEnemy1Class::AEnemy1Class()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy1Class::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy1Class::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

