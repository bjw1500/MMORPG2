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

class FSkillData
{

public:
		string Name;

		string Description;

		Protocol::Skill_ID SkillType_ID;

		int32 CoolTime;

		int32 Damage;

		float SkillCoolTime = 0;
		bool CanUse = true;
};

class FCharacterData
{
public:
	int32 Id;
	string CharacterName;

	Protocol::Stat Stat;
};


//서버와 데이터 베이스에서는 플레이어가 진행중인 퀘스트 아이디와 상황만 기억하자. 
class FQuestData
{
public:
	int32 Id;
	int32 TargetTemplatedId;
	int32 TargetNumbers;
	int32 CurrentProgress;
};
