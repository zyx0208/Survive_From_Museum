// Fill out your copyright notice in the Description page of Project Settings.


#include "DisposableItem.h"
#include "Engine/World.h"
#include "AGSDCharacter.h"
#include "Components/SphereComponent.h"
#include "Enemy1Class.h"
#include "Enemy1AIController.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일

// Sets default values
ADisposableItem::ADisposableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //충돌 컴포넌트 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    RootComponent = CollisionComponent;

    // 충돌 이벤트 바인딩
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADisposableItem::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ADisposableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADisposableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADisposableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this) // 자기 자신을 제외하고
    {
        AAGSDCharacter* Player = Cast<AAGSDCharacter>(OtherActor);
        if (Player)
        {
            if (Item == 1)//폭탄
            {
                TArray<AActor*> AllEnemys;
                UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy1Class::StaticClass(), AllEnemys);
                if (AllEnemys.Num() > 0)
                {
                    for (int i = 0; i < FMath::Min(AllEnemys.Num(), 5); i++)
                    {
                        AEnemy1Class* Enemy = Cast<AEnemy1Class>(AllEnemys[i]);
                        if (Enemy)
                        {
                            AEnemy1AIController* AIC = Cast<AEnemy1AIController>(Enemy->GetController());
                            if (AIC)
                            {
                                if (FVector::Dist(GetActorLocation(), AllEnemys[i]->GetActorLocation()) <= BombRange)
                                {
                                    AIC->Died(Enemy->AttackType);
                                    UE_LOG(LogTemp, Display, TEXT("Enemy is dead by Bomb."));
                                }
                            }
                        }
                    }
                }

                else
                {
                    UE_LOG(LogTemp, Display, TEXT("Enemy do not exist."));
                }
            }
            else if (Item == 2)//HP 절반회복
            {
                Player->SetHP((Player->MaxHealth) / 2);
            }
            else if (Item == 3)//HP 전부 회복
            {
                Player->SetHP((Player->MaxHealth));
            }
            else if (Item == 4)//자석
            {
                Player->MagneticEffect(3.0f);
            }
            else//지정되지 않은 숫자
            {
                UE_LOG(LogTemp, Display, TEXT("Set Item Number."));
            }
            Destroy();
        }
    }
}