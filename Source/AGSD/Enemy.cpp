// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;

    if (!CollisionComponent)
    {
        //투사체의 모양 생성
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        // 투사체 콜리전 이름
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));
        //투사체 크기 설정
        CollisionComponent->InitSphereRadius(15.0f);
        // Event called when component hits something.
        CollisionComponent->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
        //CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);
        //투사체 충돌설정
        RootComponent = CollisionComponent;
    }
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherComp)
    {
        // 충돌한 오브젝트의 콜리전 채널을 가져옴
        ECollisionChannel HitChannel = OtherComp->GetCollisionObjectType();

        // 콜리전 채널을 출력
        UE_LOG(LogTemp, Warning, TEXT("Hit Object Collision Channel: %d"), HitChannel);
    }
}

