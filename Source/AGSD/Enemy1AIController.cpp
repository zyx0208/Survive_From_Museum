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
#include "AGSDCharacter.h"
void AEnemy1AIController::BeginPlay()
{
	Super::BeginPlay();

	
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
	if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= AttackRange) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) > 0.7)//적이 공격범위 안에 있으면서, 전방에 있을 경우 공격 판정
	{
		//플레이어 구현이 완료되면 이 안에 코드를 수정
		UE_LOG(LogTemp, Display, TEXT("Hit!"));
		Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(AttackDamage);
	}
	GetWorld()->SpawnActor<AActor>(AttackEffect1, GetCharacter()->GetActorLocation() + GetCharacter()->GetActorForwardVector() * AttackRange, GetCharacter()->GetActorRotation());

}

void AEnemy1AIController::AttackTypeB()
{
	if (BossCount >= 2) //3번째 공격은 공격범위의 2배 및 전범위 공격
	{
		BossCount = 0;
		DrawDebugCylinder(GetWorld(), //공격 범위 그리기
			GetCharacter()->GetActorLocation(), //원통 시작위치
			GetCharacter()->GetActorLocation(), //원통 끝 위치
			AttackRange * 2, //원통 반지름
			36, //원통의 세그먼트 개수
			FColor::Red, //원통 색상
			false, //지속적으로 화면에 표시하는가?
			0.3f, //지속시간
			100.0f, //선 두께(0이 기본)
			1.0f); //투명도(불투명 = 1)
		if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= AttackRange * 2) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) >= -1)
		{
			//플레이어 구현이 완료되면 이 안에 코드를 수정
			UE_LOG(LogTemp, Display, TEXT("Hit!"));
			Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(AttackDamage);
		}
		GetWorld()->SpawnActor<AActor>(AttackEffect2, GetCharacter()->GetActorLocation(), GetCharacter()->GetActorRotation());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("BossCount : %d"), BossCount);
		BossCount++;
		if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) <= AttackRange) and FVector::DotProduct(GetCharacter()->GetActorForwardVector(), (PlayerCharacter->GetActorLocation() - GetCharacter()->GetActorLocation()).GetSafeNormal()) >= 0.7f)//적이 공격범위 안에 있으면서, 전방에 있을 경우 공격 판정
		{
			//플레이어 구현이 완료되면 이 안에 코드를 수정
			UE_LOG(LogTemp, Display, TEXT("Hit!"));
			Cast<AAGSDCharacter>(PlayerCharacter)->Attacked(AttackDamage);
		}
		GetWorld()->SpawnActor<AActor>(AttackEffect1, GetCharacter()->GetActorLocation() + GetCharacter()->GetActorForwardVector() * AttackRange, GetCharacter()->GetActorRotation());
	}
}

void AEnemy1AIController::AttackTypeC()
{
	//미구현
}

void AEnemy1AIController::AttackTypeD()
{
	//미구현
}

void AEnemy1AIController::Attacked(float damage)
{
	//데미지 계산 방식에 따라 수정 필요
	CurrentHP -= damage;
	UE_LOG(LogTemp, Display, TEXT("CurrentHP : %d"), CurrentHP);

	//체력이 0이하일 경우 죽음
	if (CurrentHP <= 0.0f)
	{
		Died(1);//데미지 계산 방식에 따라 수정 필요
	}
}

void AEnemy1AIController::Died(int64 num)
{
	//드랍 아이템 설정
	switch (num)
	{
	case 1:
		UE_LOG(LogTemp, Display, TEXT("Enemy is dead!"));
		GetWorld()->SpawnActor<AActor>(EXball, GetCharacter()->GetActorLocation(), FRotator::ZeroRotator);
		break;
	default://버그 등으로 인해 강제로 삭제해야 하는 경우
		UE_LOG(LogTemp, Display, TEXT("Enemy is isolated!"));
		break;
	}
	IsDead = true;
}

void AEnemy1AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//begin보다 늦게 실행하면서 Tick보다는 빨리 실행하는 한번만 발생하는 코드
	//동적으로 생성할 때 beginplay를 블프랑 여기서 둘다 실행하면 터지는 오류가 있음
	if (IsFisrt)
	{
		UE_LOG(LogTemp, Display, TEXT("Enemy created."));//생성됐음을 알리는 로그
		IsAttacking = false;
		IsFisrt = true;
		PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		ChenkIsolated_1 = GetCharacter()->GetActorLocation();
		ChenkIsolated_2 = GetCharacter()->GetActorLocation();
		IsFisrt = false;
		AttackCooltime_temp = AttackCooltimeFirstDelay;//공격속도 초기화
		CurrentHP = MaxHP;
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

	//캐릭터 움직임 관련
	if (PlayerCharacter)//플레이어 탐색이 됐을 경우
	{
		if ((FVector::Dist(PlayerCharacter->GetActorLocation(), GetCharacter()->GetActorLocation()) >= AttackRange) and (!IsAttacking)) //플레이어가 공격범위 밖에 있으면서 공격 중이 아닐 경우 
		{
			//적 추적
			GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
			AttackCooltime_temp = AttackCooltimeFirstDelay;
			MoveToActor(PlayerCharacter, -1.0f, true, true, true, 0, true);
			//대쉬기능
			if (Dash)
			{
				if (IsDashing)
				{
					DashCoolTime_Temp += DeltaTime;
					if (DashCoolTime_Temp >= 1.0f)
					{
						DashCoolTime_Temp = 0.0f;
						IsDashing = false;
						MoveSpeed /= 10.0f;
					}
				}
				else
				{
					DashCoolTime_Temp += DeltaTime;
					if (DashCoolTime_Temp >= DashCoolTime)
					{
						DashCoolTime_Temp = 0.0f;
						IsDashing = true;
						MoveSpeed *= 10.0f;
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
			IsAttacking = true;
			AttackCooltime_temp += DeltaTime;
			if (AttackCooltime_temp >= AttackCooltime)//공격실행
			{
				AttackCooltime_temp = 0.0f;
				switch (AttackType)//공격방식에 따라 공격 실행
				{
				case 1:
					AttackTypeA();
					break;
				case 2:
					AttackTypeB();
					break;
				default://공격타입이 설정되지 않았을 경우
					UE_LOG(LogTemp, Display, TEXT("Please seting the attack type."));
					break;
				}

				IsAttacking = false;
				MoveToActor(PlayerCharacter, -1.0f, true, true, true, 0, true); //재공격 시 플레이어를 바라보도록
			}
		}
	}
}
