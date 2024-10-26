// Fill out your copyright notice in the Description page of Project Settings.


#include "Playable_Character.h"
#include "WeaponDataTable.h"
#include "UObject/ConstructorHelpers.h"
// Sets default values
APlayable_Character::APlayable_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    WeaponID = "1";

    static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/Table/WeaponData.WeaponData'"));
    if (WeaponDataTableFinder.Succeeded())
    {
        WeaponDataTableRef = WeaponDataTableFinder.Object;
    }

    FireRate = 1.0f;
    Numberofprojectile = 1;
    SpreadAngle = 10.0f;
}

// Called when the game starts or when spawned
void APlayable_Character::BeginPlay()
{
    Super::BeginPlay();

    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using Playable_Character."));
    
    if (WeaponDataTableRef)
    {
        FName RowName = FName(*WeaponID);  // FString을 FName으로 변환
        FWeaponDataTable* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTable>(RowName, TEXT("Weapon Lookup"));
        if (WeaponData)
        {
            FString WeaponName = WeaponData->Sname;
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Name: %s"), *WeaponName));
        }
    }
}

// Called every frame
void APlayable_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayable_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayable_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayable_Character::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayable_Character::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayable_Character::AddControllerPitchInput);

	//PlayerInputComponent->BindAction("Fire", IE_Repeat, this, &APlayable_Character::Fire);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayable_Character::StartFiring);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayable_Character::StopFiring);
    PlayerInputComponent->BindAction("WeaponSwap", IE_Pressed, this, &APlayable_Character::WeaponSwap);
}

void APlayable_Character::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void APlayable_Character::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void APlayable_Character::Fire()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Fire"));
 
    // 발사
    if (ProjectileClass)
    {
        // 카메라 방향, 위치 가져오기
        FVector CameraLocation;
        FRotator CameraRotation;
        GetActorEyesViewPoint(CameraLocation, CameraRotation);

        // 총구 위치
        MuzzleOffset.Set(200.0f, 0.0f, 0.0f);

        // 총구위치 설정
        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

        // 총구 방향
        FRotator MuzzleRotation = CameraRotation;
        MuzzleRotation.Pitch += 0.0f;

        // 탄환 생성
        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            //탄환숫자만큼 발사반복
            for (int i = 0; i < Numberofprojectile; i++) {
                // 총구에 탄환 생성.
                //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("RepeatFire"));
                AProjectile_A* Projectile = World->SpawnActor<AProjectile_A>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                float AdjustedYaw = MuzzleRotation.Yaw + (i - (Numberofprojectile - 1) / 2.0f) * SpreadAngle;
                FRotator AdjustedRotation = FRotator(MuzzleRotation.Pitch, AdjustedYaw, MuzzleRotation.Roll);
                if (Projectile)
                {
                    FWeaponDataTable* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTable>(FName(*WeaponID), TEXT("Weapon Lookup"));
                    if (WeaponData)
                    {
                        //탄환에서 메쉬,마테리얼,데미지,속도,사거리 설정
                        Projectile->SetProjectileMeshAndMarterial(WeaponData->ProjectileMesh, WeaponData->ProjectileMaterial);
                        Projectile->SetProjectileSpeedDamageAndRange(WeaponData->Fspeedofprojectile, WeaponData->Fdamage, WeaponData->Frange);
                        // 탄환 방향설정
                        FVector LaunchDirection = AdjustedRotation.Vector();
                        Projectile->FireInDirection(LaunchDirection);
                    }
                    
                }
            }
        }
    }
}
void APlayable_Character::WeaponSwap() {
    if (WeaponID == "0") {
        WeaponID = "1";
    }
    else
    {
        WeaponID = "0";
    }
    FWeaponDataTable* WeaponData = WeaponDataTableRef->FindRow<FWeaponDataTable>(FName(*WeaponID), TEXT("Weapon Lookup"));
    FireRate = WeaponData->Frate;
    Numberofprojectile = WeaponData->Inumberofprojectile;
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Rate: %f"), FireRate));
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Weapon Projectile: %i"), Numberofprojectile));
}
void APlayable_Character::StartFiring()
{
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayable_Character::Fire, FireRate, true);
}

void APlayable_Character::StopFiring()
{
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}
