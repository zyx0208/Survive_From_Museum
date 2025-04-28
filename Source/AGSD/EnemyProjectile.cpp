// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "AGSDCharacter.h"
#include "Components/SphereComponent.h"


// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    RootComponent = CollisionComponent;

    // 충돌 이벤트 바인딩
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (IsFirst)
    {
        IsFirst = false;
        
    }
    SetActorLocation(GetActorLocation() + (GetActorForwardVector() * ShotSpeed * DeltaTime));
}

void AEnemyProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this) // 자기 자신을 제외하고
    {
        AAGSDCharacter* Player = Cast<AAGSDCharacter>(OtherActor);
        if (Player)
        {
            Player->Attacked(Damage);  // 플레이어의 Attacked 함수 호출
            GetWorld()->SpawnActor<AActor>(HitSound, GetActorLocation(), FRotator::ZeroRotator);
            if (IsAbleDistroy)
            {
                Destroy();
            }
        }
    }
}