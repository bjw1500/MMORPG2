// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/SkillBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/GameManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyAnimInstance.h"
#include "Managers/ObjectManager.h"
#include "MonsterController.h"
#include "Engine/DamageEvents.h"
#include "Engine/DecalActor.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"
#include "Components/CapsuleComponent.h"

USkillBase::USkillBase()
{

}


void USkillBase::Active()
{
	

	//if (ret == true)
	//	GameInstance->GetWorld()->GetTimerManager().PauseTimer(SkillTimer);
	//ret = GameInstance->GetWorld()->GetTimerManager().TimerExists(SkillTimer);
	//if (ret == true)
	//	GameInstance->GetWorld()->GetTimerManager().ClearTimer(SkillTimer);

}

void USkillBase::ActiveEnd()
{
}

void USkillBase::Update()
{

}

USkillActive::USkillActive()
{

}

void USkillActive::Active()
{
	Super::Active();

}

void USkillActive::ActiveEnd()
{
	Super::ActiveEnd();
}

void USkillActive::Update()
{
	Super::Update();

}


//Skill Stampede

USkill_Stampede::USkill_Stampede() 
{

}

void USkill_Stampede::Active()
{
	Super::Active();
	//Stampede 스킬은 돌진 스킬이다.

	//목표 지점 까지 달려가게 만든다.

	//캐릭터가 바라보는 방향을 구한다.

	//거리를 더한다.

	//목표지점까지 움직인다.
	bool ret = GameInstance->GetWorld()->GetTimerManager().IsTimerActive(SkillTimer);
	if (ret == true)
		return;

	Utils::DebugLog(TEXT("Skill Stampede Active!"));
	int32 AttackRange = 5000;
	int32 TargetId = Master->GetInfo()->targetid();
	ACreature* Target = GameInstance->GetObjectManager()->GetPlayerByID(TargetId);

	if(IsValid(Target) == true)
	{
		FVector TargetLocation = Target->GetActorLocation();
		FVector Current = Master->GetActorLocation();

		FVector Dir = TargetLocation - Current;
		//Dir.Z = 0;
		FVector Dest = Master->GetActorLocation() + Dir.GetSafeNormal() * AttackRange;
		FString test = FString::Printf(TEXT("Target  %lf"), Dest.Size());
		Utils::DebugLog(test);
		NormalSpeed = Master->Stat->GetSpeed();
		SkillSpeed = NormalSpeed * 5;
		Master->GetCharacterMovement()->MaxWalkSpeed = SkillSpeed;

		//히트박스에 충돌 효과를 추가.
		Master->OnHitBox.AddUFunction(this, FName("Stampede"));
		//Master->IsSync = false;

		//끝나면 제거
		//걍 계속 달리게 하다가 몇 초뒤에 멈추게 하자.

		FTimerHandle timer;
		GameInstance->GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateLambda([=]()
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(Master->GetController(), Dest);

			}), 1.0, false);


		GameInstance->GetWorld()->GetTimerManager().SetTimer(SkillTimer, FTimerDelegate::CreateLambda([&]()
			{
				if (IsValid(Master.Get()) == false)
					return;
				ActiveEnd();
				Master->OnHitBox.Clear();

			}), 3.2, false);
	}
	else
	{
		Master->Anim->Montage_Stop(0, Master->Anim->SkillMontage_1);
		ActiveEnd();
	}

}

void USkill_Stampede::ActiveEnd()
{
	Super::ActiveEnd();
	Utils::DebugLog("Active End");
	Master->GetCharacterMovement()->StopActiveMovement();
	Master->GetCharacterMovement()->MaxWalkSpeed = Master->Stat->GetSpeed();
	Master->Anim->JumpToSection(1, EMontageType::ActiveSkill_1);
	Master->SyncPos();
}

void USkill_Stampede::Update()
{

}

void USkill_Stampede::Stampede(ACreature* target)
{

	Utils::DebugLog(TEXT("Stampede 부딪힘"));
	FDamageEvent damageEvent;
	target->TakeDamage(Master->Stat->GetDamage() + SkillData.Damage, damageEvent, Master->GetController(), Master.Get());


}


/*Shock Wave
Grux 스킬
근접해 있는 적들한테 점프해서 내려찍는다.
*/

USkill_ShockWave::USkill_ShockWave()
{
}

void USkill_ShockWave::Active()
{
	Super::Active();
	Utils::DebugLog(TEXT("Skill ShockWave Active!"));
	
	TSubclassOf<ADecalActor> skillArea = StaticLoadClass(ADecalActor::StaticClass(), NULL, TEXT("/Game/Effect/BP_SkillDecal.BP_SkillDecal_C"));
	int32 TargetId = Master->GetInfo()->targetid();
	ACreature* Target = GameInstance->GetObjectManager()->GetPlayerByID(TargetId);

	//~초 동안 활공할 시간을 주고,

	//몬스터를 이동 시킨 후 그 범위에 있는 대상에게 데미지를 준다.

	if (IsValid(Target) == true)
	{
		TargetLocation = Target->GetActorLocation();
		FVector Current = Master->GetActorLocation();

		FRotator DecalSpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
		ADecalActor* SkillDecal = GetWorld()->SpawnActor<ADecalActor>(skillArea,TargetLocation, DecalSpawnRotation);
		if (SkillDecal != nullptr)
		{
			SkillDecal->SetLifeSpan(2.f);
			SkillDecal->GetDecal()->DecalSize = FVector(256.0f, 512.0f, 512.0f);
		}
	//	DrawDebugSphere(GetWorld(), TargetLocation, 256, 26, FColor(181, 0, 0), true, 10, 0, 2);
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(Master->GetController(), TargetLocation);

		GameInstance->GetWorld()->GetTimerManager().SetTimer(SkillTimer, FTimerDelegate::CreateLambda([&]()
			{
		
				if (IsValid(Master.Get()) == false)
				{
					Utils::DebugLog(TEXT("Shock Wave Error"));
					return;
				}


				//범위 안에 들어 있는 플레이어 판별 후 데미지를 준다.

				TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
				TArray<AActor*> IgnoreActors;
				TArray<AActor*> OutActors;

				ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
				IgnoreActors.Add(Master.Get());

				bool isOverlapped = UKismetSystemLibrary::SphereOverlapActors(GameInstance->GetWorld(), TargetLocation, 512, ObjectTypes, nullptr, IgnoreActors, OutActors);
				if (isOverlapped)
				{
					for (int i = 0; i < OutActors.Num(); i++)
					{
						ACreature* creature = Cast<ACreature>(OutActors[i]);
						if (IsValid(creature) == false)
							continue;
						Utils::DebugLog(TEXT("Sphere Overlap Actors"));
						FDamageEvent damageEvent;
						creature->TakeDamage(Master->Stat->GetDamage() + SkillData.Damage, damageEvent, Master->GetController(), Master.Get());
					}
				}

				ActiveEnd();

			}), 1.0f, false);
	}
	else
	{
		ActiveEnd();
	}
}

void USkill_ShockWave::ActiveEnd()
{
	Super::ActiveEnd();
	Utils::DebugLog("Active End");
	Master->SyncPos();
}

void USkill_ShockWave::Update()
{

}

void USkill_ShockWave::ShockWave(ACreature* target)
{



}


USkill_Roar::USkill_Roar()
{

}

void USkill_Roar::Active()
{
	Super::Active();

	//Grux의 피가 50 아래로 떨어지면 사용.

	//바닥에 스킬 범위가 나타나며, 0.5초 후 폭발 효과와 함께

	//범위 안에 있던 플레이어가 튕겨나간다.


	TSubclassOf<ADecalActor> skillArea = StaticLoadClass(ADecalActor::StaticClass(), NULL, TEXT("/Game/Effect/BP_SkillDecal.BP_SkillDecal_C"));

	FVector Current = Master->GetActorLocation();

	FRotator DecalSpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
	ADecalActor* SkillDecal = GetWorld()->SpawnActor<ADecalActor>(skillArea, Current, DecalSpawnRotation);
	if (SkillDecal != nullptr)
	{
		SkillDecal->SetActorRelativeScale3D(FVector(2.2f, 2.2f, 2.2f));
		SkillDecal->SetLifeSpan(4.f);
	}

	Master->Anim->OnSkillStart.AddUObject(this, &USkill_Roar::Roar);
}

void USkill_Roar::ActiveEnd()
{
	Super::ActiveEnd();
	Master->Anim->OnSkillStart.Clear();
	Master->SyncPos();

}

void USkill_Roar::Roar()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	IgnoreActors.Add(Master.Get());

	//DrawDebugSphere(GetWorld(), Master->GetActorLocation(), 1024, 128, FColor::Green, false, 10.f);
	bool isOverlapped = UKismetSystemLibrary::SphereOverlapActors(GameInstance->GetWorld(), Master->GetActorLocation(), 1024, ObjectTypes, nullptr, IgnoreActors, OutActors);
	if (isOverlapped)
	{
		for (int i = 0; i < OutActors.Num(); i++)
		{
			ACreature* creature = Cast<ACreature>(OutActors[i]);
			if (IsValid(creature) == false)
				continue;
			FDamageEvent damageEvent;
			creature->TakeDamage(SkillData.Damage, damageEvent, Master->GetController(), Master.Get());
		
			//맞은 적들은 전부 튕겨나가게 해주기.
			FVector targetForward = Master->GetActorForwardVector();
			creature->LaunchCharacter(targetForward * 20000, false, false);

		}
	}
	Master->Anim->OnSkillEnd.Clear();
}



//Shinbi 공격 스킬
//

UFlashAttack::UFlashAttack()
{

}

void UFlashAttack::Active()
{
	Super::Active();

	//캐릭터가 바라보는 방향으로 일정 거리 앞으로 이동시킨다.
	//이후 이동했던 라인 안의 적한테 데미지를 준다.
	Master->IsAttacking = true;
	Master->Anim->OnSkillEnd.AddUObject(this, &UFlashAttack::ActiveEnd);
	FVector playerPosition = Master->GetActorLocation();
	FVector flashDir = Master->GetActorForwardVector();
	Destination = playerPosition + flashDir * 700;
	Rotation = Master->GetActorRotation();
	float AttackRadius = 100.f;

	//캐릭터를 순간 이동 시킨후 
	Master->SetActorLocation(Destination);
	Master->TeleportTo(Destination, Rotation, false, true);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	IgnoreActors.Add(Master.Get());

	bool isOverlapped = UKismetSystemLibrary::CapsuleOverlapActors(GameInstance->GetWorld(), Destination/ 2, 128, 100, ObjectTypes,nullptr, IgnoreActors, OutActors);
	if (isOverlapped)
	{
		for (int i = 0; i < OutActors.Num(); i++)
		{
			ACreature* creature = Cast<ACreature>(OutActors[i]);
			if (IsValid(creature) == false)
				continue;
			Utils::DebugLog(TEXT("Sphere Overlap Actors"));
			FDamageEvent damageEvent;
			creature->TakeDamage(Master->Stat->GetDamage() + SkillData.Damage, damageEvent, Master->GetController(), Master.Get());
		}
	}
}

void UFlashAttack::ActiveEnd()
{
	Super::ActiveEnd();
	Master->IsAttacking = false;
	Master->Anim->OnSkillEnd.Clear();

}

//Shinbi Dash

UDash::UDash()
{
}

void UDash::Active()
{
	Super::Active();
	NormalSpeed = Master->GetMovementComponent()->GetMaxSpeed();
	Master->IsNoDamage = true;
	Utils::DebugLog(TEXT("No Damage"));

	Master->GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * 4;
	Master->Anim->OnSkillEnd.AddUObject(this, &UDash::ActiveEnd);

	GameInstance->GetWorld()->GetTimerManager().SetTimer(SkillTimer, FTimerDelegate::CreateLambda([&]()
		{
			Master->IsNoDamage = false;
			Utils::DebugLog(TEXT("No Damage false"));

		}), 1.2, false);

}

void UDash::ActiveEnd()
{
	Super::ActiveEnd();
	//Master->IsNoDamage = false;
	Master->GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	Master->Anim->OnSkillEnd.Clear();
}


UPhantomSlash::UPhantomSlash()
{
}

//PhantomSlash
void UPhantomSlash::Active()
{
	Super::Active();
	Master->IsAttacking = true;
	TSubclassOf<ADecalActor> skillArea = StaticLoadClass(ADecalActor::StaticClass(), NULL, TEXT("/Game/Effect/BP_SkillDecal.BP_SkillDecal_C"));

	//~초 동안 활공할 시간을 주고,
	FVector jump = FVector(0.0f, 0.0f, 2000.0f);
	Master->LaunchCharacter(jump, false, false);
	Master->Anim->OnSkillEnd.AddUObject(this, &UPhantomSlash::ActiveEnd);


	FVector forward = Master->GetActorForwardVector();
	Dest = forward * 4000;
	Dest.Z = -16000.0f;

	GameInstance->GetWorld()->GetTimerManager().SetTimer(SkillTimer, FTimerDelegate::CreateLambda([&]()
		{

			if (IsValid(Master.Get()) == false)
			{
				return;
			}
			Master->LaunchCharacter(Dest, false, false);

		}), 0.5f, false);
}

void UPhantomSlash::ActiveEnd()
{
	Super::ActiveEnd();
	Master->IsAttacking = true;
	//범위 안에 들어 있는 플레이어 판별 후 데미지를 준다.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	IgnoreActors.Add(Master.Get());

	bool isOverlapped = UKismetSystemLibrary::SphereOverlapActors(GameInstance->GetWorld(), Master->GetActorLocation(), 1024, ObjectTypes, nullptr, IgnoreActors, OutActors);
	if (isOverlapped)
	{
		for (int i = 0; i < OutActors.Num(); i++)
		{
			ACreature* creature = Cast<ACreature>(OutActors[i]);
			if (IsValid(creature) == false)
				continue;
			FDamageEvent damageEvent;
			creature->TakeDamage(Master->Stat->GetDamage() + SkillData.Damage, damageEvent, Master->GetController(), Master.Get());
			
			ARPGPlayer* Player = Cast<ARPGPlayer>(Master.Get());
			UWorld* World = GameInstance->GetWorld();
			UGameplayStatics::PlaySoundAtLocation(World, Player->SoundCue, Player->GetActorLocation());
		}
	}
	Master->Anim->OnSkillEnd.Clear();

}

UTripleBash::UTripleBash()
{
}

void UTripleBash::Active()
{
	Master->IsAttacking = true;
	Master->Anim->OnSkillEnd.AddUObject(this, &UTripleBash::ActiveEnd);
}

void UTripleBash::ActiveEnd()
{
	Master->IsAttacking = true;
}

