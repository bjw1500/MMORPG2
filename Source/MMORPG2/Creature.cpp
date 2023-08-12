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
#include "Managers/DataManager.h"
#include "Engine/DamageEvents.h"
#include "Components/WidgetComponent.h"
#include "GameStruct.h"
#include "HpBarWidget.h"

ACreature::ACreature()
{
	Init();

}

bool ACreature::Init()
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("ACreature Init"));
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

	/////내가 추가한 컴포넌트
	Stat = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));

	HpBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP Bar"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	HpBar->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> UW(TEXT("/Game/Blueprints/WBP_HPBar"));
	if (UW.Succeeded())
	{
		HpBar->SetWidgetClass(UW.Class);
		HpBar->SetDrawSize(FVector2D(200.f, 50.f));
	}

	return true;
}

void ACreature::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (AMyPlayerController* PlayerController = GameInstance->GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	LoadStat(1);
}

void ACreature::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//몽타주에 델리게이트 추가
	Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(Anim) == true)
	{
		Anim->OnMontageEnded.AddDynamic(this, &ACreature::OnAttackMontageEnd);
		Anim->OnAttackHit.AddUObject(this, &ACreature::AttackCheck);
	}



	HpBar->InitWidget();
	UHpBarWidget* hpBar = Cast<UHpBarWidget>(HpBar->GetUserWidgetObject());
	hpBar->BindHp(Stat);

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

void ACreature::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{

	IsAttacking = true;

}

//////////////////////////////////////////////////////////////////////////
// Input

void ACreature::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("SetUpPlayerInputComponent by Creatrue"));
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACreature::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACreature::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACreature::Look);

		//공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACreature::Attack);

		//아이템 줍기
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &ACreature::PickUpItem);

		//채팅
				//아이템 줍기
		EnhancedInputComponent->BindAction(ShowChatAction, ETriggerEvent::Triggered, this, &ACreature::ShowChat);

	}

}

void ACreature::Move(const FInputActionValue& Value)
{
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
		if (IsAttacking == false)
			return;

		Anim->PlayAttackMontage();
		IsAttacking = true;

		//패킷 보내기
		UseSkill(Protocol::ATTACK);
}

void ACreature::Jump()
{
	Super::Jump();

	//패킷 보내기
	UseSkill(Protocol::Skill_ID::JUMP);
}

void ACreature::ShowChat()
{
	//APlayerController* player = GameInstance->GetPlayerController();

	//if (player->bShowMouseCursor == false)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Show Cursor True")));
	//	player->SetInputMode(FInputModeGameAndUI());
	//	player->SetShowMouseCursor(true);
	//}
	//else if (player->bShowMouseCursor == true)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Show Cursor False")));
	//	player->SetInputMode(FInputModeGameOnly());
	//	player->SetShowMouseCursor(false);
	//}
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
	pkt.set_skillid(id);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt, Protocol::C_SKILL);
	GameInstance->GetNetworkManager()->SendPacket(sendBuffer);
}

void ACreature::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	float AttackRange = 100.f;
	float AttackRadius = 50.f;

	bool bResult = GetWorld()->SweepSingleByChannel(
		OUT HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
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

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius,
		Rotation, DrawColor, false, 2.f);


	if (bResult == true&& IsValid(HitResult.GetActor()) == true)
	{
		Protocol::Stat* infoStat = Info.mutable_stat();
		FDamageEvent damageEvent;
		//HitResult.GetActor()->TakeDamage(Stat->GetDamage(), damageEvent, GetController(), this);
		HitResult.GetActor()->TakeDamage(Stat->GetDamage(), damageEvent, GetController(), this);
	}


}

float ACreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Stat->OnDamaged(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;

}

void ACreature::OnDead()
{
}

void ACreature::FindNearItem(AMyItem* item)
{
	FString ItemText = FString::Printf(TEXT("%s 를 발견했습니다."), UTF8_TO_TCHAR(item->GetItemInfo().name().c_str()));
	Utils::DebugLog(ItemText);

	if (CanPickUpItemList.Contains(item) == false)
		CanPickUpItemList.Add(item);
}

void ACreature::LoseNearItem(AMyItem* item)
{
	FString ItemText = FString::Printf(TEXT("%s의 획득 범위에서 멀어졌습니다."), UTF8_TO_TCHAR(item->GetItemInfo().name().c_str()));
	Utils::DebugLog(ItemText);

	if (CanPickUpItemList.Contains(item)== true)
		CanPickUpItemList.Remove(item);
}

void ACreature::PickUpItem()
{
	//CanPickUpItemList의 목록에 있는 걸 획득한다.
	if (CanPickUpItemList.Num() == 0)
		return;

	AMyItem* item = CanPickUpItemList.Pop();
	if (IsValid(item) == false)
		return;

	//싱글 게임일 때는 그냥 아이템을 먹게 해준다.
	if (GameInstance->bConnected == false)
	{
		EquippedItem(item);
		return;
	}

	//온라인은 아이템을 주웠다는 패킷 보내 서버의 허락을 받아주자. 
	//나중에는 DB 연동해서 계정의 인벤토리에 저장되게 만들어주자.
	GameInstance->GetPacketHandler()->Make_C_PickUpItem(*GetInfo(), item->GetObjectInfo());
}

void ACreature::EquippedItem(AMyItem* item)
{
	CurrentUseItem = item;
	CurrentUseItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	CurrentUseItem->Master = this;
	Anim->IsEquipped = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Equipped Item"));
}

void ACreature::CreateItem()
{

	//임시 코드. 생성해서 먹는게 아니라, 나중에 아이템 주우면 장착하는 코드로 바꾸자.
	FItemData* data = GameInstance->GetDataManager()->GetItemData(2);
	if (data == nullptr)
		return;

	CurrentUseItem = GetWorld()->SpawnActor<AMyItem>(data->Model ,FVector::ZeroVector, FRotator::ZeroRotator);

	if (CurrentUseItem.IsValid() == true)
	{
		CurrentUseItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		CurrentUseItem->Master = this;

		FString text = FString::Printf(TEXT("%s 생성"), *data->Name);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *text);
	}
}

void ACreature::UpdateInfo(Protocol::ObjectInfo* info)
{
	Info.CopyFrom(*info);

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

void ACreature::UpdatePlayerSkill(int32 skillId)
{
	//패킷 정보에 따라 스킬을 사용한다.
	switch (skillId)
	{
	case Protocol::JUMP:
		Jump();
		break;
	case Protocol::ATTACK:
		Attack();
		break;
	default:
		break;
	}

}
