// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelectListUI.h"
#include "Components/ListView.h"
#include "CharacterListElement.h"
#include "Object/RPGPlayer.h"
#include "Network/GameManager.h"
#include "Managers/DataTableManager.h"
#include "Network/ClientPacketHandler.h"
#include "MyAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LobbyUI.h"
#include "Object/Weapon.h"

void UCharacterSelectListUI::RefreshUI()
{
	if (IsValid(ListElement) == false)
	{
		ListElement = StaticLoadClass(UCharacterListElement::StaticClass(), NULL, TEXT("/Game/Blueprints/UI/WBP_CharacterElement.WBP_CharacterElement_C"));
		if (IsValid(ListElement) == false)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Chat WBP_CharacterListElement Class 로드 실패"));
			return;
		}
	}

	CharacterListView->ClearListItems();

	for (auto info : CharacterInfos)
	{
		UCharacterListElement* newElement= Cast<UCharacterListElement>(CreateWidget(GetWorld(), ListElement));
		newElement->SetElement(info);
		newElement->Master = this;
		CharacterListView->AddItem(newElement);
	}


}

void UCharacterSelectListUI::SetSelectedSlot(int32 Index)
{
	//이미 선택된 슬롯이면 무시
	if (SelectedSlot == Index)
		return;
	//슬롯이 선택되었다면, 선택된 슬롯의 캐릭터로 바꿔준다.
	SelectedSlot = Index;
	SpawnSlotCharacter(Index);
}

void UCharacterSelectListUI::ClickedCreateCharacterButton()
{
	Lobby->ChangeCreateCharacterMode();
}

void UCharacterSelectListUI::SpawnCharacter(int32 Index)
{
	//기존에 있던 캐릭터는 지우고, 새로운 캐릭터를 생성한다.
	if (Index == Lobby->SelectIndex)
		return;

	if (IsValid(SelectedCharacter) == true)
	{
		if (SelectedCharacter->CurrentUseWeapon.IsValid() == true)
			SelectedCharacter->CurrentUseWeapon->Destroy();

		SelectedCharacter->Destroy();
		SelectedCharacter = nullptr;
	}

	//캐릭터 불러오기
	FCharacterData* data = GameInstance->GetDataManager()->GetCharacterData(Index);
	if (IsValid(data->Model) == false)
	{
		Utils::DebugLog(TEXT("Spawn Character Load Fail"));
		return;
	}

	FVector SpawnLocation;
	SpawnLocation.X = 500;
	SpawnLocation.Y = 500;
	SpawnLocation.Z = 100;

	FRotator SpawnRotation;
	SpawnRotation.Yaw = 0;
	SpawnRotation.Pitch = 0;
	SpawnRotation.Roll = 0;

	////스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SelectedCharacter = GameInstance->GetWorld()->SpawnActor<ARPGPlayer>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);
	SelectedCharacter->Anim->PlayRespawnMontage();
}

void UCharacterSelectListUI::SpawnSlotCharacter(int32 Index)
{
	//기존에 있던 캐릭터는 지우고, 새로운 캐릭터를 생성한다.
	if (IsValid(SelectedCharacter) == true)
	{
		if (SelectedCharacter->CurrentUseWeapon.IsValid() == true)
			SelectedCharacter->CurrentUseWeapon->Destroy();

		SelectedCharacter->Destroy();
		SelectedCharacter = nullptr;
	}

	Protocol::CharacterListElement element = CharacterInfos[Index];
	int32 characterId = element.playerinfo().templateid();

	//데이터 매니저에서 만들어낼 캐릭터 데이터를 가져온다.


	//캐릭터 불러오기
	FCharacterData* data = GameInstance->GetDataManager()->GetCharacterData(characterId);
	if (IsValid(data->Model) == false)
	{
		Utils::DebugLog(TEXT("Spawn Character Load Fail"));
		return;
	}

	FVector SpawnLocation;
	SpawnLocation.X = 500;
	SpawnLocation.Y = 500;
	SpawnLocation.Z = 100;

	FRotator SpawnRotation;
	SpawnRotation.Yaw = 0;
	SpawnRotation.Pitch = 0;
	SpawnRotation.Roll = 0;

	////스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SelectedCharacter = GameInstance->GetWorld()->SpawnActor<ARPGPlayer>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);

	//캐릭터 장비 껴주기
	SelectedCharacter->EquippedItem(element.mutable_equippeditem());

	SelectedCharacter->Anim->PlayRespawnMontage();

}

void UCharacterSelectListUI::CreateCharacter(int32 Index, FString Name)
{
	//버튼을 누르면 카메라 액터를 움직여 만들 캐릭터를 보여준다.

	//원하는 캐릭터를 골랐으면 생성 패킷을 보내준다.
	GameInstance->GetPacketHandler()->Make_C_CreateCharacter(Index, Name);
}

void UCharacterSelectListUI::DeleteCharacter(int32 Index)
{
	if (SelectedSlot < 0)
		return;

	Protocol::CharacterListElement element = CharacterInfos[SelectedSlot];
	GameInstance->GetPacketHandler()->Make_C_DeleteCharacter(element.slot());
}
