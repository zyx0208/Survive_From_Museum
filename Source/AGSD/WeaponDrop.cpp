// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDrop.h"
#include "WeaponDataTableBeta.h"

// Sets default values
AWeaponDrop::AWeaponDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/AGSD/AGSD_Character/Weapon/WeaponDataTableBeta.WeaponDataTableBeta'"));
    if (WeaponDataTableFinder.Succeeded())
    {
        WeaponDataTableRef = WeaponDataTableFinder.Object;
    }

    if (!CollisionComponent)
    {
        //투사체의 모양 생성
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        // 투사체 콜리전 이름
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("WeaponDropCollsion"));
        //투사체 크기 설정
        CollisionComponent->InitSphereRadius(50.0f);
        RootComponent = CollisionComponent;
        // Event called when component hits something.
        
        //콜리전 채널 설정
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponDrop::OnOverlapBegin);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
        
        CollisionComponent->SetHiddenInGame(true);
    }

    if (!WeaponMeshComponent) {
        WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
        WeaponMeshComponent->SetupAttachment(CollisionComponent);
        static ConstructorHelpers::FObjectFinder<UStaticMesh>WeaponMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
        if (WeaponMesh.Succeeded())
        {
            WeaponMeshComponent->SetStaticMesh(WeaponMesh.Object);
            WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        
    }

    WeaponIDInt = FMath::RandRange(5, 6);
    WeaponID = FString::FromInt(WeaponIDInt);
}

// Called when the game starts or when spawned
void AWeaponDrop::BeginPlay()
{
	Super::BeginPlay();

    FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
    //무기 자료 찾으면 생성
    if (WeaponData) {
        CurrentWeaponMesh = WeaponData->WeaponMesh;
        WeaponMeshComponent->SetStaticMesh(CurrentWeaponMesh);
        WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

// Called every frame
void AWeaponDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponDrop::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Debug! WeaponDrop Overlap Begin")));
}

void AWeaponDrop::DestroySelf()
{
    Destroy();
}

