#include "pch.h"
#include "DataManager.h"
#include "Account.h"

Account::Account()
{
}

Account::Account(string accountName, int32 dbId) : _accountName(accountName), _accountDbId(dbId)
{
}

Account::~Account()
{
}

PlayerDb::PlayerDb()
{

}

PlayerDb::~PlayerDb()
{
}

Protocol::ObjectInfo PlayerDb::GetPlayerInfo()
{
	Protocol::ObjectInfo newInfo;
	newInfo.set_type(ObjectType::PLAYER);
	newInfo.set_name(PlayerName);
	newInfo.set_templateid(TemplatedId);
	newInfo.mutable_stat()->set_level(Level);
	return newInfo;
}

Protocol::ItemInfo PlayerDb::GetCurrentUseWeapon()
{
	for (auto item : Items)
	{
		if (item->IsEquipped == true)
			return item->GetItemInfo();

	}

	return Protocol::ItemInfo();
}

void PlayerDb::AddQuest(int32 templatedId)
{
	FQuestData data = GDataManager->GetQuestData(templatedId);
	data.CurrentProgress = 0;
	CurrentQuestList[templatedId] = data;

	//데베에 저장


}

ItemDb::ItemDb()
{
}

ItemDb::~ItemDb()
{
}

Protocol::ItemInfo ItemDb::GetItemInfo()
{
	Protocol::ItemInfo info;

	info.set_templateid(TemplatedId);
	info.set_isequipped(true);

	return info;
}
