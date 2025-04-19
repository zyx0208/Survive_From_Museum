// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBook.h"

// Sets default values
ADrawingBook::ADrawingBook()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // 구슬에 충돌 컴포넌트 추가
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->InitSphereRadius(50.0f);  // 충돌 범위 설정
    RootComponent = SphereComponent;

    // 구체 오브젝트 초기화
    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
    BoxMesh->SetupAttachment(SphereComponent); // SphereComponent의 자식으로 설정
}

// Called when the game starts or when spawned
void ADrawingBook::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADrawingBook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

