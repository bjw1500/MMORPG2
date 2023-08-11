#pragma once


struct FRewardData
{
		//아이템 드랍 확률
		float ItemDropRate;
		int32 Id;
		int32 Count;
};


class MonsterData
{
public:
	int32 Id;
	string MonsterName;

	Protocol::Stat Stat;
	FRewardData Reward;
};


