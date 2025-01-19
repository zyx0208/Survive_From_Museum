// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Boomerang.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
AProjectile_Boomerang::AProjectile_Boomerang()
{
    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 1.0f;
    ProjectileDamage = 4.0f;
    ProjectileRange = 100.0f;
    if (ProjectileMovementComponent)
    {
        // 속도 0으로 설정
        ProjectileMovementComponent->Velocity = FVector::ZeroVector;
        ProjectileMovementComponent->InitialSpeed = 0;
        ProjectileMovementComponent->MaxSpeed = 0;
    }
}

void AProjectile_Boomerang::BeginPlay()
{
    // 회전 중심 캐릭터 설정 (플레이어 캐릭터 기준)
    TargetCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    Radius = 300.0f; // 회전 반지름 설정
    CurrentAngle = 0.0f; // 초기 각도
    RotationSpeed = 90.0f; // 초당 회전 속도 (도 단위)
}

void AProjectile_Boomerang::WeaponHitEffect()
{
}

void AProjectile_Boomerang::Tick(float DeltaTime)
{
    Super::AActor::Tick(DeltaTime);

    if (TargetCharacter)
    {
        // 중심 위치 (캐릭터의 위치)
        FVector CenterLocation = TargetCharacter->GetActorLocation();

        // 각도 업데이트 (도 → 라디안 변환: RotationSpeed * π / 180)
        CurrentAngle += RotationSpeed * DeltaTime * PI / 180.0f;

        // X, Y 좌표 계산
        float X = FMath::Cos(CurrentAngle) * Radius;
        float Y = FMath::Sin(CurrentAngle) * Radius;

        // 새로운 위치 계산
        FVector NewLocation = CenterLocation + FVector(X, Y, 0.0f);

        // 액터 위치 업데이트
        SetActorLocation(NewLocation);

        // 액터의 회전 방향 설정 (중심을 향하도록)
        FRotator NewRotation = (CenterLocation - NewLocation).Rotation();
        SetActorRotation(NewRotation);
    }
}


