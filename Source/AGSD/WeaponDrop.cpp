// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDrop.h"
#include "WeaponDataTableBeta.h"
#include "Kismet/GameplayStatics.h"

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
        
        //투사체 크기 설정
        CollisionComponent->InitSphereRadius(50.0f);
        RootComponent = CollisionComponent;
        // Event called when component hits something.
        
        //콜리전 채널 설정
        CollisionComponent->SetCollisionProfileName(TEXT("WeaponDropCollision"));
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponDrop::OnOverlapBegin);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
        
        //CollisionComponent->SetHiddenInGame(true);
    }

    if (!WeaponMeshComponent) {
        WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
        WeaponMeshComponent->SetupAttachment(CollisionComponent);
        static ConstructorHelpers::FObjectFinder<UStaticMesh>WeaponMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
        if (WeaponMesh.Succeeded())
        {
            WeaponMeshComponent->SetStaticMesh(WeaponMesh.Object);
            WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            WeaponMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
        }
        
    }

    WeaponIDInt = FMath::RandRange(4, 10);
    WeaponID = FString::FromInt(WeaponIDInt);

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);

    InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
    InteractionWidget->SetDrawAtDesiredSize(true); // 자동 크기 조정 (선택)
    //InteractionWidget->SetRelativeRotation(FRotator(0.f, 180.f, 0.f)); // 위젯 방향 고정 (필요 시)
    InteractionWidget->SetDrawSize(FVector2D(64.f, 64.f));
    InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // 오브젝트 위쪽으로
    InteractionWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AWeaponDrop::BeginPlay()
{
	Super::BeginPlay();

    SetMesh();
}

// Called every frame
void AWeaponDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (InteractionWidget && InteractionWidget->IsVisible())
    {
        APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (CameraManager)
        {
            FVector ToCamera = CameraManager->GetCameraLocation() - InteractionWidget->GetComponentLocation();
            FRotator LookAtRotation = FRotationMatrix::MakeFromX(ToCamera).Rotator();
            InteractionWidget->SetWorldRotation(LookAtRotation);
        }
    }
}

void AWeaponDrop::SetMesh()
{
    FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
    //무기 자료 찾으면 생성
    if (WeaponData) {
        CurrentWeaponMesh = WeaponData->WeaponMesh;
        WeaponMeshComponent->SetStaticMesh(CurrentWeaponMesh);
        WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (WeaponMeshComponent->DoesSocketExist(FName("DropScale"))) {
            FTransform SocketTransform = WeaponMeshComponent->GetSocketTransform(FName("DropScale"), RTS_World);
            FVector SocketScale = SocketTransform.GetScale3D();
            WeaponMeshComponent->SetWorldScale3D(SocketScale);
        }
        
    }

    if (InteractionWidget && InteractionWidgetClass)
    {
        InteractionWidget->SetWidgetClass(InteractionWidgetClass);
    }
}

void AWeaponDrop::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Debug! WeaponDrop Overlap Begin")));
}

void AWeaponDrop::DestroySelf()
{
    Destroy();
}

