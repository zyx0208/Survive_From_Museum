// Fill out your copyright notice in the Description page of Project Settings.


#include "SubWeapon.h"
#include "WeaponDataTableBeta.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASubWeapon::ASubWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //무기 데이터 테이블 찾기
    static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/DataTable/SubWeaponDataTable.SubWeaponDataTable'"));
    if (WeaponDataTableFinder.Succeeded())
    {
        WeaponDataTableRef = WeaponDataTableFinder.Object;
    }

    //무기 메쉬 컴포넌트 생성
    if (!WeaponMeshComponent) {
        WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
        WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        /*static ConstructorHelpers::FObjectFinder<UStaticMesh>WeaponMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
        if (WeaponMesh.Succeeded())
        {
            WeaponMeshComponent->SetStaticMesh(WeaponMesh.Object);
        }*/
    }

    //무기 능력치 초기화
    Rate = 1.0f;
    NumberOfProjectile = 1;
    SpreadAngle = 10.0f;

    //무기 ID
    WeaponID = "0";
}

// Called when the game starts or when spawned
void ASubWeapon::BeginPlay()
{
    Super::BeginPlay();

    FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
    //무기 자료 찾으면 생성
    if (WeaponData) {
        Rate = WeaponData->Frate;
        NumberOfProjectile = WeaponData->Inumberofprojectile;
        ProjectileClass = WeaponData->WeaponProjectile;
        if (ProjectileClass) {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Find ProjectileClass"));
        }
        WeaponMesh = WeaponData->WeaponMesh;
        WeaponMeshComponent->SetStaticMesh(WeaponMesh);
        WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Not Found WeaponData"));
    }
    //무기 발사 시작
    StartFiring();
}

// Called every frame
void ASubWeapon::Tick(float DeltaTime)
{

}

void ASubWeapon::Fire()
{
    CreateProjectile();
}

void ASubWeapon::StartFiring()
{
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ASubWeapon::Fire, Rate, true);
}

void ASubWeapon::CreateProjectile()
{
    // 발사
    if (ProjectileClass)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SubWeaponFire"));
        // 탄환 생성
        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            //탄환숫자만큼 발사반복
            for (int i = 0; i < NumberOfProjectile; i++) {

                // 총구에 탄환 생성.
                // 현재 액터의 위치와 회전
                FVector SpawnLocation = GetActorLocation();
                FRotator SpawnRotation = GetActorRotation();
                AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
                float AdjustedYaw = SpawnRotation.Yaw+ 90.0f +(i - (NumberOfProjectile - 1) / 2.0f) * SpreadAngle;
                FRotator AdjustedRotation = FRotator(SpawnRotation.Pitch, AdjustedYaw, SpawnRotation.Roll);
                if (Projectile)
                {
                    FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(FName(*WeaponID), TEXT("Weapon Lookup"));
                    if (WeaponData)
                    {
                        // 탄환 방향설정
                        FVector LaunchDirection = AdjustedRotation.Vector();
                        Projectile->FireInDirection(LaunchDirection);
                    }

                }
            }
        }

    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Not Found"));
    }
}

