// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Boomerang.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "SubWeapon.h"
AProjectile_Boomerang::AProjectile_Boomerang()
{
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;

    SetActorTickEnabled(true);
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 200.0f;
    ProjectileRange = 100.0f;/*
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->Deactivate();
        ProjectileMovementComponent->SetUpdatedComponent(nullptr);
        ProjectileMovementComponent->Velocity = FVector::ZeroVector;
        ProjectileMovementComponent->SetComponentTickEnabled(false);
    }*/

    RotationSpeed = FRotator(0.0f, 1000.0f, 0.0f);
    CurrentRotation = FRotator(0.0f, 0.0f, 0.0f);
}

void AProjectile_Boomerang::BeginPlay()
{
    Super::BeginPlay();
    //SetActorTickEnabled(true);

    // 투사체 속도, 데미지, 사거리
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile_Beta::OnOverlapBegin);
    if (CollisionComponent->OnComponentBeginOverlap.IsBound())
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap delegate is bound."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap delegate is NOT bound."));
    }
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
    UE_LOG(LogTemp, Warning, TEXT("[Boomerang]"));
}

/*void AProjectile_Boomerang::Tick(float DeltaTime)
{
    Super::Super::Tick(DeltaTime);
    CurrentRotation = (CurrentRotation + (RotationSpeed * DeltaTime));
    SetActorRotation(CurrentRotation);
}*/


