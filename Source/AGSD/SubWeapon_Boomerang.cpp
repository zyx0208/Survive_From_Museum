// Fill out your copyright notice in the Description page of Project Settings.


#include "SubWeapon_Boomerang.h"
#include "WeaponDataTableBeta.h"

ASubWeapon_Boomerang::ASubWeapon_Boomerang() {
    WeaponID = "1";
    RotationSpeed = FRotator(0.0f, 100.0f, 0.0f);
    CurrentRotation = FRotator(0.0f, 0.0f, 0.0f);
}

void ASubWeapon_Boomerang::BeginPlay()
{
    Super::BeginPlay();
}

void ASubWeapon_Boomerang::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CurrentRotation = CurrentRotation + (RotationSpeed * DeltaTime);
    SetActorRotation(CurrentRotation);
}

void ASubWeapon_Boomerang::StartFiring()
{
    // 발사
    if (ProjectileClass)
    {
        // 총구 위치
        MuzzleOffset.Set(400.0f, 0.0f, 0.0f);

        // 총구 방향
        FRotator MuzzleRotation;
        MuzzleRotation.Yaw = 0;
        MuzzleRotation.Pitch = 0;
        MuzzleRotation.Roll = 0;

        //MuzzleLocation.Normalize();
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
                float AdjustedYaw =  360 / (NumberOfProjectile)*(i+1);
                FRotator AdjustedRotation = FRotator(MuzzleRotation.Pitch, AdjustedYaw, MuzzleRotation.Roll);

                FTransform AdjustedTransform(AdjustedRotation);

                // 총구위치 설정
                MuzzleLocation = GetActorLocation() + FRotationMatrix(AdjustedRotation).TransformPosition(MuzzleOffset);
                MuzzleLocation.Z = 90;

                AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                if (Projectile)
                {
                    Projectile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
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

void ASubWeapon_Boomerang::CreateProjectile()
{
    
}

void ASubWeapon_Boomerang::Fire()
{
    Super::Fire();
}

