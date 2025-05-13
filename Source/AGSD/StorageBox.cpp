// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageBox.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AStorageBox::AStorageBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // 구슬에 충돌 컴포넌트 추가
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->InitSphereRadius(50.0f);  // 충돌 범위 설정
    RootComponent = SphereComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(MeshComponent);

    InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
    InteractionWidget->SetDrawAtDesiredSize(true); // 자동 크기 조정 (선택)
    //InteractionWidget->SetRelativeRotation(FRotator(0.f, 180.f, 0.f)); // 위젯 방향 고정 (필요 시)
    InteractionWidget->SetDrawSize(FVector2D(64.f, 64.f));
    InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // 오브젝트 위쪽으로
    InteractionWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AStorageBox::BeginPlay()
{
	Super::BeginPlay();

    if (InteractionWidget && InteractionWidgetClass)
    {
        InteractionWidget->SetWidgetClass(InteractionWidgetClass);
    }

    //FVector WorldLoc = InteractionWidget->GetComponentLocation();
    //UE_LOG(LogTemp, Log, TEXT("BeginPlay Widget World Location: %s"), *WorldLoc.ToString());
    //DrawDebugSphere(GetWorld(), WorldLoc, 20.f, 12, FColor::Green, false, 5.f);

    //InteractionWidget->SetVisibility(true);
}

// Called every frame
void AStorageBox::Tick(float DeltaTime)
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
