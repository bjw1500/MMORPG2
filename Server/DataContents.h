#pragma once
#include "Protocol.pb.h"

struct FRewardData
{
		//아이템 드랍 확률
		float ItemDropRate;
		int32 Id;
		string ItemName;
		int32 Count;
};


class MonsterData
{
public:
	int32 Id;
	string MonsterName;

	Protocol::Stat Stat;
	vector<FRewardData> RewardDatas;
};



