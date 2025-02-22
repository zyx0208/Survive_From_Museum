// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Boomerang.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "SubWeapon.h"
AProjectile_Boomerang::AProjectile_Boomerang()
{
    PrimaryActorTick.bCanEverTick = true;
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 200.0f;
    ProjectileRange = 100.0f;
    if (ProjectileMovementComponent)
    {
        // 속도 0으로 설정
        ProjectileMovementComponent->Velocity = FVector::ZeroVector;
        ProjectileMovementComponent->InitialSpeed = 0;
        ProjectileMovementComponent->MaxSpeed = 0;
    }

    RotationSpeed = FRotator(0.0f, 1000.0f, 0.0f);
    CurrentRotation = FRotator(0.0f, 0.0f, 0.0f);
}

void AProjectile_Boomerang::BeginPlay()
{
    Super::BeginPlay();
    // 투사체 속도, 데미지, 사거리
}

void AProjectile_Boomerang::UpdatePlayerStat()
{
    Super::UpdatePlayerStat();
    AActor* ParentActor = GetAttachParentActor();
    if (ParentActor && ParentActor->IsA(ASubWeapon::StaticClass()))
    {
        ASubWeapon* Character = Cast<ASubWeapon>(ParentActor);
        if (CollisionComponent) {
            FVector Scale = FVector(0.4 + Character->PlayerRange/10, 0.4 + Character->PlayerRange / 10, 0.4 + Character->PlayerRange / 10);
            CollisionComponent->SetRelativeScale3D(Scale);
        }
    }
    
    
}

void AProjectile_Boomerang::WeaponHitEffect(AActor* OtherActor)
{
}

void AProjectile_Boomerang::Tick(float DeltaTime)
{
    CurrentRotation = CurrentRotation + (RotationSpeed * DeltaTime);
    SetActorRotation(CurrentRotation);
}


