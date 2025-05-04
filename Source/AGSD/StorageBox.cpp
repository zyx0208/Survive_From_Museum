// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageBox.h"

// Sets default values
AStorageBox::AStorageBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // 구슬에 충돌 컴포넌트 추가
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->InitSphereRadius(50.0f);  // 충돌 범위 설정
    RootComponent = SphereComponent;


}

// Called when the game starts or when spawned
void AStorageBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStorageBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
