/*
 * Dummy.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _DUMMY_H_
#define _DUMMY_H_

/*
 * class Dummy
*/
template< class T >
class Dummy : public std::enable_shared_from_this< T >
{
public:
	Dummy(class NetPlay::RemoteID* RemoteID)
		: remoteID_(RemoteID)
	{
		if (remoteID_)
			remoteID_->SetUserData((T*)this);
	}

	~Dummy(void) = default;

public:
	// 패킷 보내기
	bool Send(class NetPlay::Packet* Packet)
	{
		if (Packet == nullptr)
			return false;

		if (remoteID_ == nullptr)
			return false;

		if (remoteID_->Send(Packet) == false)
			return false;

		return true;
	}

public:
	inline std::shared_ptr< T > SharedPtr(void)
	{
		return std::static_pointer_cast< T >(this->shared_from_this());
	}

	inline class NetPlay::RemoteID* Session(void)
	{
		return remoteID_;
	}

	inline int GetIndex(void) const
	{
		return index_;
	}

	inline bool GetPlayable(void) const
	{
		return playable_;
	}

	inline void SetLocation(Vector3& Location)
	{
		location_ = Location;
	}

	inline Vector3& GetLocation(void)
	{
		return location_;
	}

	inline void SetRotation(Vector3& Rotation)
	{
		rotation_ = Rotation;
	}

	inline Vector3& GetRotation(void)
	{
		return rotation_;
	}

	inline void SetVelocity(Vector3& Velocity)
	{
		velocity_ = Velocity;
	}

	inline Vector3& GetVelocity(void)
	{
		return velocity_;
	}

	inline void SetTargetLocation(Vector3& TargetLocation)
	{
		targetLocation_ = TargetLocation;
	}

	inline Vector3& GetTargetLocation(void)
	{
		return targetLocation_;
	}

public:
	static Randomize Rand;

protected:
	class NetPlay::RemoteID* remoteID_ = nullptr;

	int index_ = 0;						// 식별 번호

	bool playable_ = false;				// DummyClient이면 false, UE4 GameClient이면 true

	Vector3 location_ = { 0.f, };		// 위치

	Vector3 rotation_ = { 0.f, };		// 방향

	Vector3 velocity_ = { 0.f, };		// 속도

	Vector3 targetLocation_ = { 0.f, };	// 목표 위치
};

template< class T >
Randomize Dummy< T >::Rand;

#endif//_DUMMY_H_