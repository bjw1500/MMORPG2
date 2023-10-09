// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyPlayerController.h"
#include "MyAnimInstance.h"
#include "Network/GameManager.h"
#include "Managers/NetworkManager.h"
#include "Network/ClientPacketHandler.h"
#include "MyItem.h"
#include "StatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/DataTableManager.h"
#include "Managers/ObjectManager.h"
#include "Engine/DamageEvents.h"
#include "Components/WidgetComponent.h"
#include "Object/GameStruct.h"
#include "HpBarWidget.h"
#include "Skill/SkillComponent.h"

ACreature::ACreature()
{
	Init();

}

bool ACreature::Init()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	Stat = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));

	HpBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP Bar"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	HpBar->SetWidgetSpace(EWidgetSpace::Screen);

	HitBoxCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBoxCollision"));
	HitBoxCollision->SetupAttachment(GetMesh());

	static ConstructorHelpers::FClassFinder<UUserWidget> UW(TEXT("/Game/Blueprints/WBP_HPBar"));
	if (UW.Succeeded())
	{
		HpBar->SetWidgetClass(UW.Class);
		HpBar->SetDrawSize(FVector2D(200.f, 50.f));
	}

	Skill = CreateDefaultSubobject<USkillComponent>(TEXT("Skill Component"));



	return true;

}

void ACreature::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	IsAttacking = false;
	IsRolling = false;
}

void ACreature::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//몽타주에 델리게이트 추가
	Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(Anim) == true)
	{
		Anim->OnMontageEnded.AddDynamic(this, &ACreature::OnAttackMontageEnd);
		Anim->OnMontageEnded.AddDynamic(this, &ACreature::OnRollingMontageEnd);
		Anim->OnMontageEnded.AddDynamic(this, &ACreature::OnSkill_1MontageEnd);
		Anim->OnAttackHit.AddUObject(this, &ACreature::AttackCheck);
		Anim->OnAttackStart.AddUObject(this, &ACreature::AttackStart);
		Anim->OnAttackEnd.AddUObject(this, &ACreature::AttackEnd);
	}

	if (IsValid(HitBoxCollision) == true)
	{
		HitBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACreature::OnHitBoxOverlap);

	}


	HpBar->InitWidget();
	UHpBarWidget* hpBar = Cast<UHpBarWidget>(HpBar->GetUserWidgetObject());
	hpBar->BindHp(Stat);
	hpBar->SetVisibility(ESlateVisibility::Hidden);

}

void ACreature::LoadStat(int id)
{
	//온라인 상태가 아니라면,
	if (GameInstance->bOnline == false)
	{

		FStatData* stat = GameInstance->GetDataManager()->GetStat(1);
		if (stat == nullptr)
			return;

		Protocol::Stat* infoStat = Info.mutable_stat();
		infoStat->set_level(stat->Level);
		infoStat->set_hp(stat->MaxHp);
		infoStat->set_maxhp(stat->MaxHp);
		infoStat->set_damage(stat->Attack);
		UE_LOG(LogTemp, Log, TEXT("Level %d HP %d Attack %f"), stat->Level, stat->MaxHp, stat->Attack);
		UE_LOG(LogTemp, Log, TEXT("Info Level %d HP %d Attack %f"), infoStat->level(), infoStat->hp(), infoStat->damage());
	}
}



//////////////////////////////////////////////////////////////////////////
// Input
void ACreature::Move(const FInputActionValue& Value)
{

	if (IsAttacking == true)
		return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		Horizontal = MovementVector.X;
		Vertical = MovementVector.Y;


		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACreature::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACreature::Attack()
{
	if (IsValid(Anim) == false)
		return;
	if (IsAttacking == true)
		return;

	Anim->PlayAttackMontage();
	Anim->JumpToSection(AttackIndex, EMontageType::Attack);
	AttackIndex = (AttackIndex+1) % 4;

	IsAttacking = true;
}

void ACreature::Dash()
{
	if (IsValid(Anim) == false)
		return;
	if (IsRolling == true)
		return;
	Skill->ActiveSkill(SkillType::Dash);
	Anim->PlayDashMontage();
	IsRolling = true;

}

void ACreature::Run()
{
	//달리면 캐릭터의 속도를 높여준다.

	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = (1.5)* Stat->GetSpeed();
	Info.mutable_position()->set_isrunning(1);

}

void ACreature::RunStop()
{
	IsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = Stat->GetSpeed();
	Info.mutable_position()->set_isrunning(0);
}



void ACreature::Jump()
{
	Super::Jump();

	//패킷 보내기
	UseSkill(Protocol::Skill_ID::JUMP);
}

void ACreature::TryAttack()
{
	//패킷 보내기
	UseSkill(Protocol::ATTACK);
}

void ACreature::TryDash()
{

	UseSkill(Protocol::Skill_ID::DASH);
}

void ACreature::TryActiveSkill_1()
{
	UseSkill(Protocol::SKILL_1);
}

void ACreature::TryActiveSkill_2()
{
	UseSkill(Protocol::SKILL_2);
}

void ACreature::TryActiveSkill_3()
{
	UseSkill(Protocol::SKILL_3);
}

void ACreature::ActiveSkill_1()
{
	if (IsValid(Anim) == false)
		return;
	//if (IsAttacking == true)
	//	return;

	if (Skill->ActiveSkill(SkillType::SKILL_1) == false)
		return;
	Anim->PlaySkill_1Montage();
	IsCasting = true;
}

void ACreature::ActiveSkill_2()
{
	if (IsValid(Anim) == false)
		return;

	if (Skill->ActiveSkill(SkillType::SKILL_2) == false)
		return;
	Anim->PlaySkill_2Montage();
	IsCasting = true;
}

void ACreature::ActiveSkill_3()
{
	if (IsValid(Anim) == false)
		return;

	//FString skillName = UTF8_TO_TCHAR(Stat->GetStat()->skills()[2].c_str());
	if (Skill->ActiveSkill(SkillType::SKILL_3) == false)
		return;
	Anim->PlaySkill_3Montage();
	IsCasting = true;
}



void ACreature::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
}

void ACreature::OnRollingMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IsRolling = false;
}

void ACreature::OnSkill_1MontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	IsCasting = false;
}

//네트워크
void ACreature::UseSkill(Protocol::Skill_ID id)
{
	//다른 클라이언트가 조종하는 객체라면 패킷 보내기 X
	if (ThisMasterOtherClient == true || Info.type() == Protocol::MONSTER)
		return;

	//온라인 상태가 아니라면 패킷을 보내지 않는다.
	if (GameInstance->bOnline == false || GameInstance->bConnected == false)
		return;

	//스킬을 사용하면 패킷을 만들어서 보내자.
	Protocol::C_Skill pkt;
	pkt.mutable_info()->CopyFrom(Info);
	pkt.set_attackindex(AttackIndex);
	pkt.set_skillid(id);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt, Protocol::C_SKILL);
	GameInstance->GetNetworkManager()->SendPacket(sendBuffer);
}

void ACreature::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	float AttackRange = 200.f;
	float AttackRadius = 100.f;

	bool bResult = GetWorld()->SweepSingleByChannel(
		OUT HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

	FVector Vec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + Vec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Vec).ToQuat();
	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	//DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius,
	//	Rotation, DrawColor, false, 2.f);


	if (bResult == true&& IsValid(HitResult.GetActor()) == true)
	{
		Protocol::Stat* infoStat = Info.mutable_stat();
		FDamageEvent damageEvent;
		//HitResult.GetActor()->TakeDamage(Stat->GetDamage(), damageEvent, GetController(), this);
		HitResult.GetActor()->TakeDamage(20, damageEvent, GetController(), this);
	}


}

void ACreature::AttackStart()
{
	if (IsValid(WeaponCollision) == false)
		return;

	//	Utils::DebugLog(TEXT("Attack Start"));
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACreature::AttackEnd()
{
	if (IsValid(WeaponCollision) == false)
		return;
	//Utils::DebugLog(TEXT("Attack End"));
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

float ACreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//무적 상태라면,
	if (IsNoDamage == true)
		return 0;

	Stat->OnDamaged(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}

void ACreature::OnDead()
{
	Anim->SetState(Protocol::CreatureState::Dead);
	Anim->OnDead();

	GameInstance->GetObjectManager()->DespawnObject(GetInfo());

}



void ACreature::UpdateInfo(Protocol::ObjectInfo* info)
{
	Info.CopyFrom(*info);

	IsRunning = (bool)Info.position().isrunning();

	FVector location;
	location.X = Info.position().locationx();
	location.Y = Info.position().locationy();
	location.Z = Info.position().locationz();

	FRotator rotation;
	rotation.Roll = Info.position().rotationx();
	rotation.Pitch = Info.position().rotationy();
	rotation.Yaw = Info.position().rotationz();

	//TODO Velocity 추가
	SetActorRelativeLocation(location);
	SetActorRelativeRotation(rotation);
	
	FVector velocity;
	velocity.X = Info.position().velocityx();
	velocity.Y = Info.position().velocityy();
	velocity.Z = Info.position().velocityz();
	GetCharacterMovement()->Velocity = velocity;

}

void ACreature::SyncPos()
{
}

void ACreature::UpdatePlayerSkill(int32 skillId, int32 attackIndex)
{
	//if (ThisMasterOtherClient == false)
	//	return;
	//패킷 정보에 따라 스킬을 사용한다.
	switch (skillId)
	{
	case Protocol::JUMP:
		Dash();
		break;
	case Protocol::ATTACK:
		AttackIndex = AttackIndex;
		Attack();
		break;
	case Protocol::DASH:
		Dash();
		break;
	case Protocol::SKILL_1:
		ActiveSkill_1();
		break;
	case Protocol::SKILL_2:
		ActiveSkill_2();
		break;
	case Protocol::SKILL_3:
		ActiveSkill_3();
		break;
	default:
		break;
	}
}



void ACreature::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		ACreature* Master = Cast<ACreature>(OtherActor);
		if (IsValid(Master) != false)
		{
			if(OnHitBox.IsBound() == true)
				OnHitBox.Broadcast(Master);
		}
	}

}
