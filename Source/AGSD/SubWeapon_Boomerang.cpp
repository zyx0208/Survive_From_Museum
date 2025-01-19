// Fill out your copyright notice in the Description page of Project Settings.


#include "SubWeapon_Boomerang.h"
#include "WeaponDataTableBeta.h"

ASubWeapon_Boomerang::ASubWeapon_Boomerang() {
    WeaponID = "1";
}

void ASubWeapon_Boomerang::BeginPlay()
{
    Super::BeginPlay();
}

void ASubWeapon_Boomerang::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASubWeapon_Boomerang::StartFiring()
{
    Super::StartFiring();
}

void ASubWeapon_Boomerang::CreateProjectile()
{
    // 발사
        if (ProjectileClass)
        {
            // 총구 위치
            MuzzleOffset.Set(0.1f, 0.0f, 0.0f);

            // 총구 방향
            FRotator MuzzleRotation;
            MuzzleRotation.Yaw = 0;
            MuzzleRotation.Pitch = 0;
            MuzzleRotation.Roll = 0;

            // 총구위치 설정
            MuzzleLocation = FTransform(MuzzleRotation).TransformVector(MuzzleOffset);
            MuzzleLocation.Z = 90;

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
                    // 현재 액터의 위치와 회전
                    FVector SpawnLocation = GetActorLocation();
                    FRotator SpawnRotation = GetActorRotation();
                    AProjectile_Beta* Projectile = World->SpawnActor<AProjectile_Beta>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
                    float AdjustedYaw = SpawnRotation.Yaw + 360/(NumberOfProjectile)*i;
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

void ASubWeapon_Boomerang::Fire()
{
    Super::Fire();
}

