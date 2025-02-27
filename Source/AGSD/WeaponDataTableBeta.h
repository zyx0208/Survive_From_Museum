// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Projectile_Beta.h"
#include "Engine/Texture2D.h"
#include "Animation/AnimMontage.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeaponDataTableBeta.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FWeaponDataTableBetaStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()
	//무기 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IID;

	//무기 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sname;

    // 획득 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsAcquired = false;

	//무기 발사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frate;

	//무기 탄환 숫자
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Inumberofprojectile;

    //무기 발사 반복
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int RepeatFire;

	//무기 탄환
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectile_Beta> WeaponProjectile;

	//무기 공격 모션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* WeaponAnimationMontage;

	//무기 모양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMesh")
	UStaticMesh* WeaponMesh;

    //무기 공격 파티클
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UParticleSystem* WeaponParticle;

    //무기 공격 유형
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool WeaponType;

    // 아이콘 이미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* WeaponIcon;
};
