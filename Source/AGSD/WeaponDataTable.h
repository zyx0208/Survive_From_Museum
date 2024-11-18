// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponDataTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponDataTable :public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	//무기 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IID;

	//무기 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sname;

	//무기 탄환 숫자
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Inumberofprojectile;

	//무기 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frange;

	//무기 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fdamage;

	//무기 공격속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frate;

	//무기 정확도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Faccuracy;

	//무기 탄환 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fspeedofprojectile;

	//무기 모양
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* WeaponMesh;

	//무기 마테리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* WeaponMaterial;

	//탄환 모양
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* ProjectileMesh;

	//무기 마테리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* ProjectileMaterial;
};
