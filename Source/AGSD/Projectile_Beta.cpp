// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Beta.h"

// Sets default values
AProjectile_Beta::AProjectile_Beta()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    if (!RootComponent)
    {
        //투사체 생성
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
    }

    if (!CollisionComponent)
    {
        //투사체의 모양 생성
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        // 투사체 콜리전 이름
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
        //투사체 크기 설정
        CollisionComponent->InitSphereRadius(15.0f);
        // Event called when component hits something.
        CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile_Beta::OnHit);
        //투사체 충돌설정
        RootComponent = CollisionComponent;
    }

    if (!ProjectileMovementComponent)
    {
        // 투사체 속도 설정
        ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
        ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
        ProjectileMovementComponent->InitialSpeed = 3000.0f;
        ProjectileMovementComponent->MaxSpeed = 3000.0f;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = true;
        ProjectileMovementComponent->Bounciness = 0.3f;
        ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    }

    if (!ProjectileMeshComponent)
    {
        ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
        static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
        if (Mesh.Succeeded())
        {
            ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
        }
        static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("/Script/Engine.Material'/Game/Projectile/Material/Projectile_Red.Projectile_Red'"));
        if (Material.Succeeded())
        {
            ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
        }
        ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
        ProjectileMeshComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
        ProjectileMeshComponent->SetupAttachment(RootComponent);
    }

    //시간 지나면 파괴
    InitialLifeSpan = 30.0f;

    // 투사체 생성 위치 저장
    StartLocation = FVector::ZeroVector;

    // 투사체 속도, 데미지, 사거리
    ProjectileSpeed = 50.0f;
    ProjectileDamage = 1.0f;
    ProjectileRange = 1000.0f;
}

// Called when the game starts or when spawned
void AProjectile_Beta::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AProjectile_Beta::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 현재 위치와 시작 위치의 거리를 계산
    float DistanceTraveled = FVector::Dist(StartLocation, GetActorLocation());

    // 특정 거리 이상 이동하면 파괴
    if (DistanceTraveled > ProjectileRange)
    {
        Destroy();
    }
}

void AProjectile_Beta::FireInDirection(const FVector& ShootDirection)
{
    StartLocation = GetActorLocation();
    //탄환시작방향설정
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile_Beta::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComponent != nullptr)
    {
        // 충돌한 오브젝트가 AEnemy 클래스인지 확인
        if (OtherActor->GetClass()->IsChildOf(AEnemy::StaticClass()))
        {
            // AEnemy로 캐스팅
            AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
            if (HitEnemy)
            {
                // 체력감소
                HitEnemy->Health -= ProjectileDamage;

                // 적처치
                if (HitEnemy->Health <= 0.0f)
                {
                    HitEnemy->Destroy();
                }

                // 체력 감소 결과를 화면에 출력
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Hit enemy's remaining health: %f"), HitEnemy->Health));
            }
            WeaponHitEffect();
        }
    }

    // 발사체를 파괴 (필요시)
    Destroy();
}

void AProjectile_Beta::WeaponHitEffect()
{
}

