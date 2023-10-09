// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGPlayer.h"
#include "Creature.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
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
#include "Engine/DamageEvents.h"
#include "Components/WidgetComponent.h"
#include "GameStruct.h"
#include "HpBarWidget.h"
#include "Weapon.h"
#include "Managers/UIManager.h"
#include "UI/InGameUI.h"
#include "UI/InventoryUI.h"
#include "Managers/InventoryManager.h"
#include "Managers/ObjectManager.h"
#include "UI/ItemSlot.h"
#include "UI/ShopItemList.h"
#include "Object/NPC.h"
#include "UI/ChatWidget.h"
#include "UI/QuestUI.h"

ARPGPlayer::ARPGPlayer() 
{
	//Init();
}

void ARPGPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {


		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACreature::Move);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ACreature::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ACreature::RunStop);
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACreature::Look);

		//공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACreature::TryAttack);

		//아이템 줍기
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &ARPGPlayer::PickUpItem);

		//채팅
		EnhancedInputComponent->BindAction(ShowChatAction, ETriggerEvent::Triggered, this, &ARPGPlayer::ShowChat);

		//스킬
		EnhancedInputComponent->BindAction(ActiveSkillAction1, ETriggerEvent::Triggered, this, &ACreature::TryActiveSkill_1);
		EnhancedInputComponent->BindAction(ActiveSkillAction2, ETriggerEvent::Triggered, this, &ACreature::TryActiveSkill_2);
		EnhancedInputComponent->BindAction(ActiveSkillAction3, ETriggerEvent::Triggered, this, &ACreature::TryActiveSkill_3);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACreature::TryDash);

		//인벤토리
		EnhancedInputComponent->BindAction(ShowInventoryAction, ETriggerEvent::Triggered, this, &ARPGPlayer::ShowInventory);

		//상점
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ARPGPlayer::InteractNPC);

		//퀘스트창
		EnhancedInputComponent->BindAction(OpenQuestAction, ETriggerEvent::Triggered, this, &ARPGPlayer::OpenQuest);
	}

}

void ARPGPlayer::BeginPlay()
{
	//Add Input Mapping Context
	Super::BeginPlay();
	if (AMyPlayerController* PlayerController = GameInstance->GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ARPGPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//다른 플레이어가 조종하는 캐릭터일경우
	if (ThisMasterOtherClient == false)
		return;

	UHpBarWidget* hpBar = Cast<UHpBarWidget>(HpBar->GetUserWidgetObject());
	hpBar->SetVisibility(ESlateVisibility::Visible);
}


void ARPGPlayer::OnDead()
{
	Super::OnDead();


}

void ARPGPlayer::Destroyed()
{
	Super::Destroy();

	if (CurrentUseWeapon.IsValid() == false)
		return;

	CurrentUseWeapon.Get()->Destroy();

}


void ARPGPlayer::FindNearItem(AMyItem* Item)
{
	FString ItemText = FString::Printf(TEXT("%s 를 발견했습니다."), UTF8_TO_TCHAR(Item->GetItemInfo().name().c_str()));
	//Utils::DebugLog(ItemText);

	if (CanPickUpItemList.Contains(Item) == false)
		CanPickUpItemList.Add(Item);
}

void ARPGPlayer::LoseNearItem(AMyItem* Item)
{
	FString ItemText = FString::Printf(TEXT("%s의 획득 범위에서 멀어졌습니다."), UTF8_TO_TCHAR(Item->GetItemInfo().name().c_str()));
	//Utils::DebugLog(ItemText);

	if (CanPickUpItemList.Contains(Item) == true)
		CanPickUpItemList.Remove(Item);
}

void ARPGPlayer::PickUpItem()
{
	//CanPickUpItemList의 목록에 있는 걸 획득한다.
	if (CanPickUpItemList.Num() == 0)
		return;

	AMyItem* item = CanPickUpItemList.Pop();
	if (IsValid(item) == false)
		return;

	//임시 코드
	//싱글 게임일 때는 그냥 아이템을 먹게 해준다.
	if (GameInstance->bConnected == false)
	{
		//EquippedItem(item);
		return;
	}

	//Equipped 패킷 만들기

	GameInstance->GetPacketHandler()->Make_C_PickUpItem(*GetInfo(), item->GetObjectInfo());
}

void ARPGPlayer::AddToInventory(Protocol::ItemInfo* ItemInfo)
{
	//인벤토리 매니저에 아이템을 채워준다.
	UInventoryManager* Inven = GameInstance->GetInventory();
	Inven->AddItem(ItemInfo);

	//인벤 UI를 업데이트 한다.
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	if (IsValid(UI) == false)
		return;
	UInventoryUI* InvenUI = UI->InventoryUI;
	if (IsValid(InvenUI) == false)
	{
		Utils::DebugLog(TEXT("Add To Inventory Error. Inven is not Valid"));
		return;
	}

	//인벤토리 매니저에 저장된 정보에 따라 UI를 업데이트 한다.
	InvenUI->RefreshUI();

	if (CurrentUseWeapon.IsValid() == false && ItemInfo->itemtype() == Protocol::WEAPON)
	{
		GameInstance->GetPacketHandler()->Make_C_EquippedItem(*GetInfo(), *ItemInfo);
	}

}

void ARPGPlayer::EquippedItem(Protocol::ItemInfo* Item)
{
	//템플릿 아이디가 없다면, 아무런 장비를 착용 중인게 아니라면,
	//
	if (Item->templateid() == 0)
	{
		return;
	}

	//장착중인 아이템이 있다면,
	if (CurrentUseWeapon.IsValid() == true)
	{
		CurrentUseWeapon->Destroy();

	}


	AMyItem* item = GameInstance->GetObjectManager()->CreateItem(Item);
	item->SetItemInfo(*Item);
	CurrentUseWeapon = Cast<AWeapon>(item);
	CurrentUseWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	CurrentUseWeapon->Master = this;
	CurrentUseWeapon->SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CurrentUseWeapon->SphereCollision->SetSimulatePhysics(false);
	WeaponCollision = CurrentUseWeapon->WeaponCollision;
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	Anim->IsEquipped = true;
	if (GameInstance->GetObjectManager()->GetMyPlayer() != this)
		return;
}

void ARPGPlayer::CreateItem()
{

	//임시 코드. 생성해서 먹는게 아니라, 나중에 아이템 주우면 장착하는 코드로 바꾸자.
	FItemData* data = GameInstance->GetDataManager()->GetItemData(2);
	if (data == nullptr)
		return;

	CurrentUseWeapon = GetWorld()->SpawnActor<AWeapon>(data->Model, FVector::ZeroVector, FRotator::ZeroRotator);

	if (CurrentUseWeapon.IsValid() == true)
	{
		CurrentUseWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		CurrentUseWeapon->Master = this;
	}
}

void ARPGPlayer::ShowChat()
{
	//팝업창을 띄운다?
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	if (IsValid(UI) == false)
		return;
	if (UI->ChatUI->Visibility == ESlateVisibility::Visible)
	{
		UI->ChatUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UI->ChatUI->SetVisibility(ESlateVisibility::Visible);
	}

}

void ARPGPlayer::ShowInventory()
{
	//팝업창을 띄운다?
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	if (IsValid(UI) == false)
		return;

	if (UI->InventoryUI->Visibility == ESlateVisibility::Visible)
	{
		UI->InventoryUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UI->InventoryUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void ARPGPlayer::InteractNPC()
{
	if (IsValid(CanInteractNPC) == false)
		return;

	CanInteractNPC->InteractPlayer();
}

void ARPGPlayer::OpenQuest()
{
	//팝업창을 띄운다?
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	if (IsValid(UI) == false)
		return;

	if (UI->QuestUI->Visibility == ESlateVisibility::Visible)
	{
		UI->QuestUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UI->QuestUI->SetVisibility(ESlateVisibility::Visible);
	}
}

float ARPGPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//같은 플레이어가 공격할 경우 데미지를 무시한다.
	ARPGPlayer* otherPlayer = Cast<ARPGPlayer>(DamageCauser);
	if (IsValid(otherPlayer) == true)
		return 0;
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (IsNoDamage == true)
		return 0;

	if (GetController() == GetWorld()->GetFirstPlayerController())
	{
		if (IsValid(ShakeClass) == true)
		{
			UUserWidget* flash = CreateWidget<UUserWidget>(GetWorld(), HitFlash);
			flash->AddToViewport();
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(ShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator(0.f));
		}
	}

	return DamageAmount;
}
