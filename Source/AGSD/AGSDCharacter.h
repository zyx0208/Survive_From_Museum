// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Projectile_A.h"
#include "Projectile_Beta.h"
#include "WeaponDataTable.h"
#include "AccessoryData.h"
#include "NPC1Class.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "MouseIndicatorActor.h"    //마우스 포인터
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Animation/AnimMontage.h"
#include "Components/PoseableMeshComponent.h"
#include "XPOrb.h"
#include "SubWeapon.h"
#include "WeaponDrop.h"
#include "GameTimer.h"

//#include "AGSDCharacter_LevelUP.h"

#include "AGSDCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AAGSDCharacter_LevelUP;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AAGSDCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

    /** Interaction Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* InteractionAction;

public:
	AAGSDCharacter();
	virtual void Tick(float DeltaTime) override;

	FVector CharacterLocation;//캐릭터 위치

	FVector TraceHitLocation;  // 라인트레이스 충돌 위치
	FVector TraceHitDirection; // 라인트레이스 충돌 방향

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundWave* WalkingSound;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundWave* DashSound;
    UAudioComponent* WalkingAudioComponent;
    UAudioComponent* DashAudioComponent;

    void UpdateCameraObstruction();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Dash();

    void Interaction();
		
    void PlayWalkingSound();
    bool IsWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<class AProjectile_Beta> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AProjectile_Beta* ProjectilePool;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarUIBPClass;

	UPROPERTY()
	UUserWidget* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> LevelUpUIBPClass;

	UPROPERTY()
	UUserWidget* LevelUpWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> ClearUIClass;

    UPROPERTY()
    UUserWidget* ClearWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameoverUIClass;

    UPROPERTY()
    UUserWidget* GameoverWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> StorageBoxWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> DrawingBookWidgetClass;

    UPROPERTY()
    UUserWidget* WeaponExchangeWidget;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> WeaponExchangeWidgetClass;

    UPROPERTY()
    UUserWidget* StorageBoxWidget;
    UPROPERTY()
    UUserWidget* DrawingBookWidget;

    UPROPERTY()
    UUserWidget* DamageTextWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> DamageTextWidgetClass;

    // 레벨업을 AAGSDCharacter_LevelUP에서 처리하도록 변경
    UPROPERTY()
    AAGSDCharacter_LevelUP* LevelUpHandler;
    //마우스 포인터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MouseIndicator")
    TSubclassOf<AMouseIndicatorActor> MouseIndicatorClass;
    UPROPERTY()
    class AMouseIndicatorActor* MouseIndicator;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


public:
	//캐릭터 스탯
	int32 MaxHealth;        // 최대 체력
	int32 CurrentHealth;    // 현제 체력
	int32 Defense;          // 방어력
    float SpeedLevel;       // 이동속도
    int32 Barrier;           // 방어막

	float Attack;           // 공격력
    float AttackRangeLevel; // 공격 범위
    float AttackSpeedLevel; // 공격 속도

	int32 CharacterLevel;   // 캐릭터 레벨
	int32 CurrentXP;        // 현재 경험치
	int32 XPToNextLevel;    // 다음 레벨까지 필요한 경험치
	float BounsXPLevel;		// 업그레이드 옵션(획득 경험치 증가 옵션)
    float XPRangeLevel;     // 경험치 획득 반경

    bool IsResurrection = false;



    // GameTimer 인스턴스
    UPROPERTY()
    UGameTimer* InGameTimer;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthBar(); //체력바 갱신함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateXPBar(); //경험치바 갱신함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateStat();

	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void ApplyLevelUpOption(AAGSDCharacter* Character, const FAccessoryData& SelectedAccessory);

    //UFUNCTION(BlueprintCallable, Category = "LevelUp")
    //void ParseAccessoryEffect(const FString& EffectString, TArray<FString>& OutEffects);

    //UFUNCTION(BlueprintCallable, Category = "LevelUp")
    //void ApplyAccessoryEffect(const struct FAccessoryData& Accessory);


	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void PauseGameForLevelUp();

	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void ResumeGameAfterLevelUp();

	void ShowLevelUpUI();
	// XP를 추가하고 레벨 업을 처리하는 함수
	void AddXP(int32 XPAmount);
	void LevelUp();
    UFUNCTION(BlueprintCallable, Category = "Interaction") void Attacked(float Damage); //데미지를 받을때 발생하는 함수
    void SetHP(float value); //해당 수치만큼 체력을 조정(현재 체력값에서 조정)
    void MagneticEffect(float time); //time의 시간 만큼 자석 효과 활성화
    void MagneticEffectOff(); //자석효과 종료 후 정상화시키는 함수

    //자석 범위
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magnet", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* MagnetSphere;
    //자석 범위 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnet", meta = (AllowPrivateAccess = "true"))
    float MagnetStrength = 250.0f; //초기 자석 값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnet")
    float MagnetAcceleration = 50.0f; // 기본 가속도 값
    UPROPERTY()
    TArray<AXPOrb*> MagnetField;

    void AddToMagnetField(AXPOrb* XPOrb);
    void RemoveFromMagnetField(AXPOrb* XPOrb);

    //대쉬 관련 변수
    // 쿨타임 관련 변수
    /** 대시 쿨타임 UI 업데이트 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateDashCooldownUI();
    /** UI 위젯 */
    UPROPERTY()
    class UDashCooldown_UI* DashCooldownWidget;
    /** UI 클래스 */
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> DashCooldownUIClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashCooldown = 3.0f;
    /** 현재 남은 대시 쿨타임 */
    float DashCooldownTimer;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    bool bCanDash = true;

    // 무적 상태 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    bool bIsInvincible = false;
    // 대쉬 쿨타임 타이머 함수
    UFUNCTION()
    void ResetDashCooldown();

    /** 원래 충돌 상태를 저장할 변수 */
    ECollisionResponse OriginalMonsterCollision;
    /** 몬스터 충돌 무시 설정 */
    void IgnoreMonsterCollision();
    /** 몬스터 충돌 복구 */
    void RestoreMonsterCollision();
    // 몬스터 충돌 타이머 핸들
    FTimerHandle CollisionRestoreTimerHandle;

    // 쿨타임 타이머 핸들
    FTimerHandle DashCooldownTimerHandle;

    // 무적 해제 타이머 핸들
    FTimerHandle InvincibilityTimerHandle;

    // 쿨타임 UI 타이머 핸들러
    FTimerHandle DashCooldownUpdateTimerHandle;

    FTimerHandle MagneticEffectTimerHandle;

    

    // 무적 해제 함수
    UFUNCTION()
    void ResetInvincibility();

    //사망 함수
    UFUNCTION()
    void OnDeath();

    UFUNCTION()
    void Clear();

	//무기 발사
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void StartFiring();

	UFUNCTION()
	void StopFiring();

	UFUNCTION()
	void CreateProjectile();

	//무기 공격속도
	float FireRate;

	//무기 타이머
	FTimerHandle FireTimerHandle;
	FTimerHandle FireRateTimerHandle;
    FTimerHandle AttackCooldownTimerHandle;

    //무기 쿨타임 변수
    float AttackCooldownStartTime;
    float AttackTotalCooldownTime;
    void UpdateAttackCooldownProgress();
    void OnAttackCooldownFinished();

	//무기 탄환 숫자
	int Numberofprojectile;

    //무기 반복 발사
    int RepeatFire;

	//탄환 각도
	float SpreadAngle;

    //무기 유형
    bool WeaponType;

	//무기 메쉬 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMeshComponent;

	//무기 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMesh* CurrentWeaponMesh;

    //무기 공격 사운드
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USoundCue* WeaponSoundCue;

    //무기 유형에 따른 애니메이션
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TSubclassOf<UAnimInstance> WeaponAnimType;


    //무기 소켓
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* SubWeaponAttachPoint;
	//무기 교체
	UFUNCTION()
	void WeaponSwap();
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateSwapWeaponIcon();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleLocation;

	UPROPERTY()
	FString WeaponID = "0";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* WeaponDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* WeaponDataTableRef;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UDataTable* AccessoryDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AProjectile_Beta> CurrentProjectile;

	//애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FireMontage;

    // 파티클 스폰 함수
    UFUNCTION(BlueprintCallable, Category = "Particle")
    void SpawnParticle(FVector Location, FRotator Rotation);

    //파티클
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle")
    UNiagaraSystem* WeaponParticle;

    //보조무기 추가용
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ASubWeapon* SubWeaponSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASubWeapon> SubWeaponSelector;

    //보유한 보조무기들
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ASubWeapon*> SubWeapons;

    //보조무기 생성
    UFUNCTION()
    void SpawnSubWeapon(TSubclassOf<ASubWeapon> SubWeapon);

    //보유한 주무기 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> WeaponArray;

    //현재 든 무기 슬롯
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool CurrentWeaponSlot = true;

    //무기 들기
    UFUNCTION()
    void WeaponTake();

    //드랍된 무기와 들고 있는 무기 교체
    UFUNCTION()
    void GetWeapon();

    //오버랩된 무기드랍
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AWeaponDrop* OverlapWeaponDrop;

    //현재 오버랩된 드랍된 무기 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString OverlapID = "0";

    //현재 무기와 오버랩 되었는지 확인
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OverlapDropWeapon = false;

    //창고 관련 함수 및 변수
    //현재 상자와 오버랩 되었는지 확인
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OverlapBox = false;
    //도감 오버랩 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OverlapDrawingBook = false;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowStorageBoxUI();
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowDrawingBookUI();

    UFUNCTION()
    bool WeaponSwapCheck();
    bool NoSwap = false;

    //디버그
    UFUNCTION()
    void Debug();
    
    //오버랩
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    //오버랩 종료
    UFUNCTION()
    void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

    //오버랩 확인
    bool IsOverlappingActor(const AActor* Other) const;

    //몽타주 재생
    UFUNCTION()
    void PlayFireMontage(UAnimMontage* Montage);

    //UFUNCTION()
    //void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UAnimMontage* CurrentMontage;

    int RepeatCount;
    int CurrentCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimInstance* AnimInstance;

    //무기 획득시 교체 확인
    UFUNCTION()
    void ShowWeaponExchangeUI();

};

