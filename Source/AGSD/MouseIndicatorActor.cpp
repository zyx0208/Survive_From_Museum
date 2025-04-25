// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseIndicatorActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMouseIndicatorActor::AMouseIndicatorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    // 머티리얼이나 메시 할당은 블루프린트나 여기서 직접 설정
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Engine/BasicShapes/Plane")); // 평면 메시
    if (Mesh.Succeeded())
    {
        MeshComponent->SetStaticMesh(Mesh.Object);
        MeshComponent->SetRelativeScale3D(FVector(0.5f)); // 사이즈 조절
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}


