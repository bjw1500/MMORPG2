#pragma once
class GameObject
{
public:
	GameObject();
	~GameObject();


	virtual void Update(float deltaTime);
	float GetDistanceFromTarget(Protocol::Position pos);





	/////////////
	ObjectInfo GetInfo() { return _info; };
	void SetInfo(ObjectInfo& info) { _info.CopyFrom(info); };
	Position GetPos() { return _info.position(); }
	void SetPos(Position pos) { _info.mutable_position()->CopyFrom(pos); }

	int32 GetHP() { return _info.stat().hp(); }
	void SetHP(int32 value){

		if (value >= _info.stat().maxhp())
			value = _info.stat().maxhp();
		else if (value <= 0)
			value = 0;

		_info.mutable_stat()->set_hp(value);
	}
	
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

