// Fill out your copyright notice in the Description page of Project Settings.
/*
※ AI 설정 방법 ※
1. 원하는 캐릭터 클래스 또는 폰 클래스를 상속받은 블루프린트를 생성
2. 블루프린트 창을 열고 클래스 디폴트 창을 열기
3. [디테일 - 폰 - AI 컨트롤러 클래스]에 해당 파일을 적용
4. 원하는 AI 인식 범위를 [액터 배치 - 볼륨 - 내비 메시 바운드 볼륨]을 통해 지정
5. 해당 범위에 1번에서 만든 캐릭터를 배치하면 아래의 코드를 따라 적을 추적함
*/

#include "Enemy1AIController.h"
#include "Kismet/GameplayStatics.h"// 레벨에서 액터를 불러오기 위한 함수를 가진 헤더파일
#include "GameFramework/Character.h"//레벨에 등장하는 액터의 방향이나 이동을 조절하기 위한 함수를 가진 헤더파일
#include "GameFramework/CharacterMovementComponent.h"//캐릭터의 움직임을 제어하는 함수를 참조하기 위한 헤더파일
#include "DrawDebugHelpers.h"//범위 시각화를 위한 헤더파일
#include "KillCount.h"//킬카운트용
#include "Engine/LevelScriptActor.h"//레벨블루프린트
#include "AGSDCharacter.h"
#include "Math/UnrealMathUtility.h"//랜덤 수 추출을 위한 헤더
#include "Enemy1Class.h"
#include "TimerManager.h"
#include "GameFramework/HUD.h"
#include "AGSDGameInstance.h"
#include "Damage_UI.h"
#include "Animation/AnimInstance.h"

void AEnemy1AIController::BeginPlay()
{
	Super::BeginPlay();
    UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
    if(GI){
        DamageUI = Cast<UDamage_UI>(GI->DamageUIInstance);
    }
}

void AEnemy1AIController::AttackTypeA()
{
	/*
	DotProduct는 내적을 구하는 함수
	1 : 정면(0도)
	0.7 : 약 45도
	0 : 수직(90도)
	-1: 후면(180도)
	ex : 0 이상으로 설정하면 캐릭터의 앞을 기준으로 좌우 90도가 범위
	*/
	if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= Enemy->AttackRange * 1.2f) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) > 0.7)//적이 공격범위 안에 있으면서, 전방에 있을 경우 공격 판정
	{
		//플레이어 구현이 완료되면 이 안에 코드를 수정
		UE_LOG(LogTemp, Display, TEXT("Hit!"));
		Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
	}
	GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() + GetCharacter()->GetActorForwardVector() * Enemy->AttackRange, GetCharacter()->GetActorRotation());

}

void AEnemy1AIController::AttackTypeB()
{
    FRotator EnemyRotator = GetCharacter()->GetActorRotation();
    GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() + 200 * (GetCharacter()->GetActorForwardVector()), GetCharacter()->GetActorRotation());
    GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect2, GetCharacter()->GetActorLocation() + 200 * (GetCharacter()->GetActorForwardVector() - GetCharacter()->GetActorRightVector()), GetCharacter()->GetActorRotation());
    GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() + 200 * (GetCharacter()->GetActorForwardVector() + GetCharacter()->GetActorRightVector()), GetCharacter()->GetActorRotation());
    if (IsRage)
    {
        Enemy->AttackCooltime = 3.0f;
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect4, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= Enemy->AttackRange * 0.3f))
        {
            //플레이어 구현이 완료되면 이 안에 코드를 수정
            UE_LOG(LogTemp, Display, TEXT("Hit!"));
            Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
        }
    }
}

void AEnemy1AIController::AttackTypeC()
{
    if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= Enemy->AttackRange * 1.0f) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) >= -1)
    {
        //플레이어 구현이 완료되면 이 안에 코드를 수정
        UE_LOG(LogTemp, Display, TEXT("Hit!"));
        Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
    }
}

void AEnemy1AIController::AttackTypeD()
{
    if (BossCount >= 2) //3번째 공격은 오크통 발사
    {
        BossCount = 0;
        FRotator EnemyRotator = GetCharacter()->GetActorRotation();
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect2, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        if (IsRage)
        {
            Enemy->DashCoolTime = 1.5f;
            for (int i = 0; i < 16; i++)
            {
                GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), EnemyRotator);
                EnemyRotator.Yaw += 22.5f;
            }
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), EnemyRotator);
                EnemyRotator.Yaw += 45.0f;
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("BossCount : %d"), BossCount);
        BossCount++;
        if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= Enemy->AttackRange * 1.2f) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) >= 0.7f)
        {
            //플레이어 구현이 완료되면 이 안에 코드를 수정
            UE_LOG(LogTemp, Display, TEXT("Hit!"));
            Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
        }
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() + GetCharacter()->GetActorForwardVector() * Enemy->AttackRange, GetCharacter()->GetActorRotation());
    }



}

void AEnemy1AIController::AttackTypeE()
{
    FRotator EnemyRotator = GetCharacter()->GetActorRotation();
    GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
    GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), EnemyRotator);
}

void AEnemy1AIController::AttackTypeF(int AttackNum)
{
    if (((BossCount+1) % 3) == 0) //3번째 공격은 충격파 발사
    {
        if (AttackNum)
        {
            BossCount = 0;
            UE_LOG(LogTemp, Display, TEXT("BossCount : %d"), BossCount);
            GetCharacter()->SetActorLocation(PlayerLocation);
            IsSavePlayerLocation = false;
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect2, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("BossCount : %d"), BossCount);
            BossCount = 0;
            IsSavePlayerLocation = false;
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect2, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        }
    }
    else //이외에는 4방향 부채꼴 파이어볼 공격
    {
        UE_LOG(LogTemp, Display, TEXT("BossCount : %d"), BossCount);
        BossCount++;
        FRotator EnemyRotator = GetCharacter()->GetActorRotation();
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        EnemyRotator.Yaw -= 15.0f;
        for (int i = 0; i < 4; i++)
        {
            GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), EnemyRotator);
            EnemyRotator.Yaw += 10.0f;
        }
    }
}

void AEnemy1AIController::AttackTypeG(int AttackNum)
{
    /*
    변수 n에 따라 다음 패턴을 사용
    1 : 돌진 1회
    2 : 돌진 2회 후 3초 기절
    3 : 지뢰소환 패턴
    4 : 점프찍기 패턴
    5 : 기절 강공격
    */
    IsSavePlayerLocation = false;
    int n = AttackNum;
    if (n == 1)
    {
        IsLaunchAttacking = true;
        Stun(1.5f);
        GetWorldTimerManager().SetTimer(LaunchAttackTimerHandle, this, &AEnemy1AIController::LaunchAttackTimerEnd, 1.0f, false);
        Enemy->LaunchCharacter((PlayerLocation - Enemy->GetActorLocation()).GetSafeNormal() * 6000.0f, true, true);
    }
    else if (n == 2)
    {
        IsLaunchAttacking = true;
        Stun(5.0f);
        GetWorldTimerManager().SetTimer(LaunchAttackTimerHandle, this, &AEnemy1AIController::LaunchDoubleAttackTimerEnd, 2.0f, false);
        Enemy->LaunchCharacter((PlayerLocation - Enemy->GetActorLocation()).GetSafeNormal() * 6000.0f, true, true);
        PlayerLocation = PlayerCharacter->GetActorLocation();
    }
    else if (n == 3)
    {
        if (Enemy->AttackEffect1)
        {
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() + (GetCharacter()->GetActorForwardVector() * 500.0f), GetCharacter()->GetActorRotation());
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() - (GetCharacter()->GetActorForwardVector() * 500.0f), GetCharacter()->GetActorRotation());
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() + (GetCharacter()->GetActorRightVector() * 500.0f), GetCharacter()->GetActorRotation());
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, GetCharacter()->GetActorLocation() - (GetCharacter()->GetActorRightVector() * 500.0f), GetCharacter()->GetActorRotation());
        }
    }
    else if (n == 4)
    {
        Stun(1.5f);
        GetCharacter()->SetActorLocation(PlayerLocation);
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect2, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= 300.0f)
        {
            Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
            Cast<AAGSDCharacter>(PlayerCharacter)->KnockbackApply((PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal() * 4000.0f, 1200.0f);
        }
        GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);

    }
    else if (n == 5)
    {
        Stun(0.5f);
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect4, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= Enemy->AttackRange * 1.2f) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) > 0.7)//적이 공격범위 안에 있으면서, 전방에 있을 경우 공격 판정
        {
            Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
            Cast<AAGSDCharacter>(PlayerCharacter)->KnockbackApply((PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal() * 1000.0f, 1200.0f);
            Cast<AAGSDCharacter>(PlayerCharacter)->StunApply(2.0f);
        }
    }
}

void AEnemy1AIController::LaunchAttackTimerEnd()
{
    GetWorldTimerManager().ClearTimer(LaunchAttackTimerHandle);
    IsLaunchAttacking = false;
}

void AEnemy1AIController::LaunchDoubleAttackTimerEnd()
{
    GetWorldTimerManager().ClearTimer(LaunchDoubleAttackTimerHandle);
    GetWorldTimerManager().SetTimer(LaunchAttackTimerHandle, this, &AEnemy1AIController::LaunchAttackTimerEnd, 1.0f, false);
    Enemy->LaunchCharacter((PlayerLocation - Enemy->GetActorLocation()).GetSafeNormal() * 9000.0f, true, true);
}

void AEnemy1AIController::BossTimerEnd()
{
    if (Enemy->AttackType == 7)
    {
        GetWorldTimerManager().ClearTimer(LaunchDoubleAttackTimerHandle);
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() + (GetCharacter()->GetActorForwardVector() * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() - (GetCharacter()->GetActorForwardVector() * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() + (GetCharacter()->GetActorRightVector() * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() - (GetCharacter()->GetActorRightVector() * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() + ((GetCharacter()->GetActorForwardVector() + GetCharacter()->GetActorRightVector()) * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() - ((GetCharacter()->GetActorForwardVector() + GetCharacter()->GetActorRightVector()) * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() + ((GetCharacter()->GetActorForwardVector() - GetCharacter()->GetActorRightVector()) * 700.0f), GetCharacter()->GetActorRotation());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, GetCharacter()->GetActorLocation() - ((GetCharacter()->GetActorForwardVector() - GetCharacter()->GetActorRightVector()) * 700.0f), GetCharacter()->GetActorRotation());
        if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) >= 500.0f) && (FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= 900.0f))
        {
            Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
        }
    }
    else if ((Enemy->AttackType == 8)&&(AttackNum_Temp == 1))
    {
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorRotation());
    }
    else if ((Enemy->AttackType == 8) && (AttackNum_Temp == 3))
    {
        if (IsRage)
        {
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f));
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 90.0f, 0.0f));
        }
        else
        {
            if (BossCount == 0)
            {
                GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 90.0f, 0.0f));
            }
            else
            {
                GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f));
            }
        }
    }
    else if ((Enemy->AttackType == 8) && (AttackNum_Temp == 4))
    {
        Groggy(1.0f);
        GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
    }
    else if ((Enemy->AttackType == 8) && (AttackNum_Temp == 5))
    {
        Groggy(1.0f);
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect4, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
    }
}

void AEnemy1AIController::AttackTypeH(int AttackNum)
{
    /*
    1 : 전조 후 구역 미사일 떨구기 [광폭 : 연속으로 여러 번]
    2 : 보스 주변에 광역으로 밀어내기 [광폭 : 넉백 후 스턴]
    3 : 맵 전체에 럭스 궁 여러 번(가로 세로) [광폭 : 가로 세로 동시에 등장]
    4 : 2*4 모양으로 총알 발사 [광폭 : 연속으로]
    5 : 유도 미사일 발사 [광폭 : 연속으로]
    */
    IsSavePlayerLocation = false;
    int n = AttackNum;
    if (n == 1)
    {
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect1, PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorRotation());
        if (IsRage)
        {
            Groggy(1.0f);
            GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);
        }
    }
    else if (n == 2)
    {
        GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect2, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= Enemy->AttackRange * 1.0f)
        {
            Cast<AAGSDCharacter>(PlayerCharacter)->KnockbackApply((PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal() * 1000.0f, 1200.0f);
            if (IsRage)
            {
                Cast<AAGSDCharacter>(PlayerCharacter)->StunApply(2.0f);
            }
            Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(Enemy->AttackDamage);
        }
    }
    else if (n == 3)
    {
        if (IsRage)
        {
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f));
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 90.0f, 0.0f));
            Groggy(1.0f);
            GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);
        }
        else
        {
            if (FMath::RandRange(0, 1))
            {
                BossCount = 0;
                GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f));
                Groggy(1.0f);
                GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);
            }
            else
            {
                BossCount = 1;
                GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect3, PlayerCharacter->GetActorLocation(), FRotator(0.0f, 90.0f, 0.0f));
                Groggy(1.0f);
                GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);
            }
        }
    }
    else if (n == 4)
    {
        if (IsRage)
        {
            Groggy(1.0f);
            GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
            GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);
        }
        else
        {
            Groggy(1.0f);
            GetWorld()->SpawnActor<AActor>(Enemy->EnemyProjectile, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        }
    }
    else if (n == 5)
    {
        if (IsRage)
        {
            Groggy(2.0f);
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect4, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
            GetWorldTimerManager().SetTimer(BossTimer, this, &AEnemy1AIController::BossTimerEnd, 1.0f, false);
        }
        else
        {
            Groggy(1.0f);
            GetWorld()->SpawnActor<AActor>(Enemy->AttackEffect4, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        }
    }
}

void AEnemy1AIController::Attacked(float damage)
{
    if (IsFisrt)
    {
        return;
    }
    if (IsNotAttacked)
    {
        damage = 0.0f;
    }
    if (Enemy->AttackType == -1)
    {
        IsLaunchAttacking = true;
        GetWorldTimerManager().SetTimer(LaunchAttackTimerHandle, this, &AEnemy1AIController::LaunchAttackTimerEnd, 0.5f, false);
        Enemy->LaunchCharacter((PlayerCharacter->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal() * -1000.0f * damage, true, true);
        return;
    }
    FVector PawnLocation = Enemy->GetActorLocation();
    PawnLocation.Z += 100.0f;
    FVector2D ScreenLocation;
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (Enemy.IsValid()) {
        Enemy->CurrentHP -= damage;
        UE_LOG(LogTemp, Display, TEXT("Damamge : %f CurrentHP : %d Actor : %s"), damage, Enemy->CurrentHP, *Enemy->GetName());
        GetWorld()->SpawnActor<AActor>(Enemy->AttackedEffect, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
        if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
            ShowDamage(damage, ScreenLocation);
        }
        
        if (!GetWorldTimerManager().IsTimerActive(AttackedEffectHandle))
        {
            Enemy->IsAttacked = true;
            GetWorldTimerManager().SetTimer(AttackedEffectHandle, this, &AEnemy1AIController::AttackedEffectEnd, 0.1f, false);
        }
        
        //체력이 0이하일 경우 죽음
        if ((Enemy->CurrentHP <= 0.0f)&&(Enemy->IsDontDie == false))
        {
            Died(Enemy->AttackType);
        }
        else if(Enemy->CurrentHP <= 0.0f)
        {
            Enemy->CurrentHP = 0;
        }
    }
}

void AEnemy1AIController::Attacked(float damage, int chanel)
{
    if (IsFisrt)
    {
        return;
    }
    if (IsNotAttacked)
    {
        damage = 0.0f;
    }
    if (Enemy->AttackType == -1)
    {
        IsLaunchAttacking = true;
        GetWorldTimerManager().SetTimer(LaunchAttackTimerHandle, this, &AEnemy1AIController::LaunchAttackTimerEnd, 0.5f, false);
        Enemy->LaunchCharacter((PlayerCharacter->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal() * -1000.0f * damage, true, true);
        return;
    }
    FVector PawnLocation = Enemy->GetActorLocation();
    PawnLocation.Z += 100.0f;
    FVector2D ScreenLocation;
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    
    switch (chanel)
    {
    case 1 :
        if (!GetWorldTimerManager().IsTimerActive(Chanel1TimerHandle))
        {
            GetWorldTimerManager().SetTimer(Chanel1TimerHandle, this, &AEnemy1AIController::Chanel1TimerEnd, 0.1f, false);
            Enemy->CurrentHP -= damage;
            if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
                ShowDamage(damage, ScreenLocation);
            }
        }
        break;
    case 2:
        if (!GetWorldTimerManager().IsTimerActive(Chanel2TimerHandle))
        {
            GetWorldTimerManager().SetTimer(Chanel2TimerHandle, this, &AEnemy1AIController::Chanel2TimerEnd, 0.1f, false);
            Enemy->CurrentHP -= damage;
            if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
                ShowDamage(damage, ScreenLocation);
            }
        }
        break;
    case 3:
        if (!GetWorldTimerManager().IsTimerActive(Chanel3TimerHandle))
        {
            GetWorldTimerManager().SetTimer(Chanel3TimerHandle, this, &AEnemy1AIController::Chanel3TimerEnd, 0.1f, false);
            Enemy->CurrentHP -= damage;
            if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
                ShowDamage(damage, ScreenLocation);
            }
        }
        break;
    case 4:
        if (!GetWorldTimerManager().IsTimerActive(Chanel4TimerHandle))
        {
            GetWorldTimerManager().SetTimer(Chanel4TimerHandle, this, &AEnemy1AIController::Chanel4TimerEnd, 0.1f, false);
            Enemy->CurrentHP -= damage;
            if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
                ShowDamage(damage, ScreenLocation);
            }
        }
        break;
    case 5:
        if (!GetWorldTimerManager().IsTimerActive(Chanel5TimerHandle))
        {
            GetWorldTimerManager().SetTimer(Chanel5TimerHandle, this, &AEnemy1AIController::Chanel5TimerEnd, 0.1f, false);
            Enemy->CurrentHP -= damage;
            if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
                ShowDamage(damage, ScreenLocation);
            }
        }
        break;
    default:
        Enemy->CurrentHP -= damage;
        if (PC && PC->ProjectWorldLocationToScreen(PawnLocation, ScreenLocation)) {
            ShowDamage(damage, ScreenLocation);
        }
        break;
    }
    //UE_LOG(LogTemp, Warning, TEXT("Test"));
    UE_LOG(LogTemp, Warning, TEXT("Enemy Addr = 0x%p"), Enemy);
    if (Enemy.IsValid())
    {
        if (!GetWorldTimerManager().IsTimerActive(AttackedEffectHandle))
        {
            Enemy->IsAttacked = true;
            GetWorldTimerManager().SetTimer(AttackedEffectHandle, this, &AEnemy1AIController::AttackedEffectEnd, 0.1f, false);
        }
        GetWorld()->SpawnActor<AActor>(Enemy->AttackedEffect, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
    }
    UE_LOG(LogTemp, Display, TEXT("Damamge : %f CurrentHP : %d Actor : %s"), damage, Enemy->CurrentHP, *Enemy->GetName());
    //체력이 0이하일 경우 죽음
    if ((Enemy->CurrentHP <= 0.0f) && (Enemy->IsDontDie == false))
    {
        Died(Enemy->AttackType);
    }
    else if (Enemy->CurrentHP <= 0.0f)
    {
        Enemy->CurrentHP = 0;
    }
}

void AEnemy1AIController::AttackedEffectEnd()
{
    Enemy->IsAttacked = false;
    GetWorldTimerManager().ClearTimer(AttackedEffectHandle);
}

void AEnemy1AIController::Chanel1TimerEnd()
{
    GetWorldTimerManager().ClearTimer(Chanel1TimerHandle);
}

void AEnemy1AIController::Chanel2TimerEnd()
{
    GetWorldTimerManager().ClearTimer(Chanel2TimerHandle);
}

void AEnemy1AIController::Chanel3TimerEnd()
{
    GetWorldTimerManager().ClearTimer(Chanel3TimerHandle);
}

void AEnemy1AIController::Chanel4TimerEnd()
{
    GetWorldTimerManager().ClearTimer(Chanel4TimerHandle);
}

void AEnemy1AIController::Chanel5TimerEnd()
{
    GetWorldTimerManager().ClearTimer(Chanel5TimerHandle);
}

void AEnemy1AIController::Stun(float duration)
{
    GetWorldTimerManager().SetTimer(StunTimer, this, &AEnemy1AIController::StunTimerEnd, duration, false);
    IsStun = true;
}

void AEnemy1AIController::StunTimerEnd()
{
    IsStun = false;
}

void AEnemy1AIController::Groggy(float duration)
{
    GetWorldTimerManager().SetTimer(GroggyTimer, this, &AEnemy1AIController::GroggyTimerEnd, duration, false);
    IsGroggy = true;
}

void AEnemy1AIController::GroggyTimerEnd()
{
    IsGroggy = false;
}

void AEnemy1AIController::GroggyForceEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(GroggyTimer);
    AEnemy1AIController::GroggyTimerEnd();
}

void AEnemy1AIController::ShowDamage(float damage, FVector2D screenPosition)
{

    if (DamageUI) {
        DamageUI->DamageTextCreate(damage, screenPosition);
    }
}


void AEnemy1AIController::Died(int64 num)
{
    //이미 죽은 상태라면 더 이상 죽음 판정이 안생김
    if (Temp_Dead)
    {
        return;
    }
    Temp_Dead = true;

    //월드 정보 불러오기 및 난수 생성
    UWorld* World = GetWorld();
    int DropNum = FMath::RandRange(1, 100);

    //엘리트 몬스터 드랍 아이템 설정
    if (Enemy->IsElite)
    {
        //킬 카운트
        UE_LOG(LogTemp, Display, TEXT("Enemy is dead!"));
        if (World)
        {
            KillCountCall(World);
        }
        //드랍설정
        if (Enemy->EXball)
        {
            for (int i = 0; i < FMath::RandRange(10, 20); i++)
            {
                World->SpawnActor<AActor>(Enemy->EXball, GetCharacter()->GetActorLocation() + FVector(FMath::FRandRange(-100.0f, 100.0f), FMath::FRandRange(-100.0f, 100.0f), 0.0f), FRotator::ZeroRotator);
            }
        }
        Enemy->IsDead = true;
        return;
    }

    //튜토리얼 몬스터 드랍 아이템 설정
    if (Enemy->IsTutorialEnemy)
    {
        UE_LOG(LogTemp, Display, TEXT("Enemy is dead!"));
        //드랍설정
        if (Enemy->EXball)
        {
            World->SpawnActor<AActor>(Enemy->EXball, GetCharacter()->GetActorLocation() + FVector(FMath::FRandRange(-100.0f, 100.0f), FMath::FRandRange(-100.0f, 100.0f), 0.0f), FRotator::ZeroRotator);
        }
        Enemy->IsDead = true;
        return;
    }
    
	//드랍 아이템 설정
	switch (num)
	{
    case 1: //일반 몬스터(공격타입 1, 3, 5번) : [무기 : 1%] [경험치 : 80%] [HP회복 : 절반(6%), 전부(3%)] [자석 : 5%] [폭탄 5%]
	case 3: 
    case 5:
        //킬 카운트
		UE_LOG(LogTemp, Display, TEXT("Enemy is dead!"));
        if (World)
        {
            KillCountCall(World);
        }
        //드랍 설정
        if (DropNum <= 1) //1% 확률로 무기 드랍
        {
            if (Enemy->WeaponDrop)
            {
                World->SpawnActor<AActor>(Enemy->WeaponDrop, GetCharacter()->GetActorLocation(), FRotator::ZeroRotator);
            }
        }
        else if (DropNum <= 81) //80% 확률로 경험치 드랍
        {
            if (Enemy->EXball)
            {
                for (int i = 0; i < FMath::RandRange(1, 3); i++)
                {
                    World->SpawnActor<AActor>(Enemy->EXball, GetCharacter()->GetActorLocation() + FVector(FMath::FRandRange(-100.0f, 100.0f), FMath::FRandRange(-100.0f, 100.0f), 0.0f), FRotator::ZeroRotator);
                }
            }
        }
        else if (DropNum <= 87) //6% 확률로 HP 절반 회복 드랍
        {
            if (Enemy->Heal_half)
            {
                World->SpawnActor<AActor>(Enemy->Heal_half, GetCharacter()->GetActorLocation(), FRotator::ZeroRotator);
            }
        }
        else if (DropNum <= 90) //3% 확률로 HP 전부 회복 드랍
        {
            if (Enemy->Heal_full)
            {
                World->SpawnActor<AActor>(Enemy->Heal_full, GetCharacter()->GetActorLocation(), FRotator::ZeroRotator);
            }
        }
        else if (DropNum <= 95) //5% 확률로 자석 드랍
        {
            if (Enemy->Magnetic)
            {
                World->SpawnActor<AActor>(Enemy->Magnetic, GetCharacter()->GetActorLocation(), FRotator::ZeroRotator);
            }
        }
        else//5% 확률로 폭탄 드랍
        {
            if (Enemy->Bomb)
            {
                World->SpawnActor<AActor>(Enemy->Bomb, GetCharacter()->GetActorLocation(), FRotator::ZeroRotator);
            }
        }
		break;
	default://버그나 보스전 등으로 인해 강제로 삭제해야 하는 경우
		UE_LOG(LogTemp, Display, TEXT("Enemy is deleted!"));
		break;
	}
    if (Enemy->IsBoss)
    {
        Cast<AAGSDCharacter>(PlayerCharacter)->Clear();
    }
    else
    {
        Enemy->IsDead = true;
    }
}

void AEnemy1AIController::KillCountCall(UWorld* World)
{
    if (!World) {
        return;
    }
    ALevelScriptActor* LevelScript = World->GetLevelScriptActor();
    if (LevelScript)
    {
        // LevelBlueprint의 FunctionName 호출
        FName FunctionName(TEXT("KillEnemy")); // 레벨 블루프린트 함수 이름
        UFunction* Function = LevelScript->FindFunction(FunctionName);
        if (Function)
        {
            LevelScript->ProcessEvent(Function, nullptr); // 함수 호출
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Function '%s' not found in LevelScriptActor!"), *FunctionName.ToString());
        }
    }
}

void AEnemy1AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//begin보다 늦게 실행하면서 Tick보다는 빨리 실행하는 한번만 발생하는 코드
	//동적으로 생성(ex:스포너로 생성)할 때 beginplay를 블프랑 여기서 둘다 실행하면 터지는 오류가 있음
	if (IsFisrt)
	{
		UE_LOG(LogTemp, Display, TEXT("Enemy created."));//생성됐음을 알리는 로그
        //플레이어 캐릭터 및 적 캐릭터의 변수 지정
        Enemy = Cast<AEnemy1Class>(GetPawn());
        PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

        //기본값 설정
        Enemy->IsAttacking = false;
		ChenkIsolated_1 = GetCharacter()->GetActorLocation();
		ChenkIsolated_2 = GetCharacter()->GetActorLocation();
		AttackCooltime_temp = Enemy->AttackCooltimeFirstDelay;//공격속도 초기화
        Enemy->CurrentHP = Enemy->MaxHP;
        Temp_Dead = false;
        IsStun = false;
        IsPlayingAnim = true;
        IsNotAttacked = false;
        IsSavePlayerLocation = false;
        Enemy->IsAttacked = false;
        IsRage = false;
        IsLaunchAttacking = false;
        if (PlayerCharacter)
        {
            MoveToActor(PlayerCharacter, 999999999.0f, true, true, true, 0, true);
        }

        //초기값 설정이 끝난 상태
        IsFisrt = false;
	}
    
    //테스트 중 죽는 경우를 테스트하기 위한 코드
    if (Enemy->IsDead)
    {
        Died(-1);
    }

	//게임 실행 중에 짝수 혹은 홀수의 틱마다 발동되는 코드를 작성하기 위함
	if (TickSwitch)
	{
		TickSwitch = false;
	}
	else
	{
		TickSwitch = true;
	}

    //분노 상태
    if ((Enemy->IsBoss) and (Enemy->CurrentHP / Enemy->MaxHP <= 0.5f))
    {
        IsRage = true;
    }

	//캐릭터 움직임 관련
	if (PlayerCharacter)//플레이어 탐색이 됐을 경우
	{
        if (IsGroggy)
        {
            StopMovement();
        }
        else if (!IsStun and !IsPlayingAnim)
        {
            IsPlayingAnim = true;
            GetCharacter()->GetMesh()->bPauseAnims = false;
        }
        else if ((IsStun) or (Enemy->IsDead))
        {
            IsPlayingAnim = false;
            StopMovement();
            GetCharacter()->GetMesh()->bPauseAnims = true;
        }
		else if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) >= Enemy->AttackRange) and (!Enemy->IsAttacking)) //플레이어가 공격범위 밖에 있으면서 공격 중이 아닐 경우 
		{
			//적 추적
			GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = Enemy->MoveSpeed;
			AttackCooltime_temp = Enemy->AttackCooltimeFirstDelay;
			MoveToActor(PlayerCharacter, -1.0f, true, true, true, 0, true);
			//대쉬기능
			if (Enemy->Dash)
			{
				if (IsDashing)
				{
					DashCoolTime_Temp += DeltaTime;
					if (DashCoolTime_Temp >= 1.0f)
					{
						DashCoolTime_Temp = 0.0f;
						IsDashing = false;
                        Enemy->MoveSpeed /= Enemy->FDash;
					}
				}
				else
				{
					DashCoolTime_Temp += DeltaTime;
					if (DashCoolTime_Temp >= Enemy->DashCoolTime)
					{
						DashCoolTime_Temp = 0.0f;
						IsDashing = true;
                        Enemy->MoveSpeed *= Enemy->FDash;
					}
				}
			}
			//만약 공격 중이 아니면서 탐색을 했는데 캐릭터가 일정시간동안 움직이지 않을 경우
			if (TickSwitch)
			{
				ChenkIsolated_1 = GetCharacter()->GetActorLocation();
				if (ChenkIsolated_1.Equals(ChenkIsolated_2, 1.0f))
				{
					ChenkIsolated_Timer += DeltaTime;
				}
				else
				{
					ChenkIsolated_Timer = 0.0f;
				}
			}
			else
			{
				ChenkIsolated_2 = GetCharacter()->GetActorLocation();
				if (ChenkIsolated_2.Equals(ChenkIsolated_1, 1.0f))
				{
					ChenkIsolated_Timer += DeltaTime;
				}
				else
				{
					ChenkIsolated_Timer = 0.0f;
				}
			}

			//설정된 시간 내에 움직임이 없을 경우 객체를 강제 삭제
			if (ChenkIsolated_Timer >= 30.0f)
			{
				Died(-1);
			}
		}
		else//플레이어가 공격범위 안에 있을 경우
		{
			//공격 상호작용
			StopMovement();
            if (((!IsSavePlayerLocation) and (Enemy->AttackType == 6) and (BossCount == 2))||((Enemy->AttackType == 7)&&(!IsSavePlayerLocation)) || ((Enemy->AttackType == 8) && (!IsSavePlayerLocation)))
            {
                IsSavePlayerLocation = true;
                PlayerLocation = PlayerCharacter->GetActorLocation();
                if (Enemy->AttackType == 6)
                {
                    AttackNum_Temp = FMath::RandRange(0, 1);
                }
                else if (Enemy->AttackType == 7)
                {
                    AttackNum_Temp = FMath::RandRange(1, 5);
                }
                else if (Enemy->AttackType == 8)
                {
                    AttackNum_Temp = FMath::RandRange(1, 5);
                }
            }
            Enemy->IsAttacking = true;
			AttackCooltime_temp += DeltaTime;
			if (AttackCooltime_temp >= Enemy->AttackCooltime)//공격실행
			{
				AttackCooltime_temp = 0.0f;
				switch (Enemy->AttackType)//공격방식에 따라 공격 실행
				{
				case 1:
					AttackTypeA();
					break;
				case 2:
					AttackTypeB();
					break;
                case 3:
                    AttackTypeC();
                    break;
                case 4:
                    AttackTypeD();
                    break;
                case 5:
                    AttackTypeE();
                    break;
                case 6:
                    AttackTypeF(AttackNum_Temp);
                    break;
                case 7:
                    AttackTypeG(AttackNum_Temp);
                    break;
                case 8:
                    AttackTypeH(AttackNum_Temp);
                    break;
				default://공격타입이 설정되지 않았을 경우
					UE_LOG(LogTemp, Display, TEXT("Please seting the attack type."));
					break;
				}

                Enemy->IsAttacking = false;
				MoveToActor(PlayerCharacter, -1.0f, true, true, true, 0, true); //재공격 시 플레이어를 바라보도록
			}
		}
	}
}
