#pragma once
#include "GameObject.h"
class Item : public GameObject
{
public:
	static void MakeItem(int32 templateId);

	void SetItemInfo(Protocol::ItemInfo itemInfo) { Info.CopyFrom(itemInfo); }

private:
	Protocol::ItemInfo Info;
};


class Weapon : public Item
{


};


class Armor : public Item
{


};


class Consumable : public Item
{



};

