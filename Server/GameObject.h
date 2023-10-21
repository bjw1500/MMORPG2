#pragma once
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();


	virtual void Update(float deltaTime);
	virtual void UpdateInfo();
	float GetDistanceFromTarget(Protocol::Position pos);



	/////////////
	Protocol::ObjectInfo& GetInfo() { return _info; };
	void SetInfo(Protocol::ObjectInfo& info) {_info.CopyFrom(info); };

	Protocol::Position GetPos() { return _info.position(); }
	void SetPos(Protocol::Position pos) { _info.mutable_position()->CopyFrom(pos);  }

	int32 GetHP() { return _info.stat().hp(); }
	void SetHP(int32 value){

		if (value >= _info.stat().maxhp())
			value = _info.stat().maxhp();
		else if (value <= 0)
			value = 0;

		_info.mutable_stat()->set_hp(value);
	}

	int32 GetMaxHP() { return _info.stat().maxhp(); }

	int32 GetLevel() { return _info.stat().level(); }
	void SetLevel(int32 level) { _info.mutable_stat()->set_level(level); }

	int32 GetExp() { return _info.stat().totalexp(); }
	void SetExp(int32 exp) { _info.mutable_stat()->set_totalexp(exp); }

	int32 GetDamage() { return _info.stat().damage(); }
	void SetDamage(int32 damage) { _info.mutable_stat()->set_damage(damage); }

	int32 GetSpeed() { return _info.stat().movespeed(); }
	void SetSpeed(int32 speed) { _info.mutable_stat()->set_movespeed(speed); }

	int32 GetTemplatedId() { return _info.templateid(); }

	void SetRoom(GameRoomRef room)
	{
		_room = room;
	}

	GameRoomRef GetRoomRef()
	{
		return _room;
	}

protected:

	Protocol::ObjectInfo _info;
	GameRoomRef _room;

};

