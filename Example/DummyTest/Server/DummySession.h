/*
 * DummySession.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _DUMMY_SESSION_H_
#define _DUMMY_SESSION_H_

/*
* class DummySession
*/
class DummySession : public Dummy< DummySession >
{
public:
	const static unsigned int kMaxPagination = 8;

public:
	DummySession(class NetPlay::RemoteID* RemoteID, float Width, float Height);

	~DummySession(void) = default;

public:
	// 더미 정보 보내기
	bool ReqDummyInfo(class NetPlay::Packet* Packet);

	// 위치 보내기
	bool MoveToLocation(class NetPlay::Packet* Packet);

	// 주변 더미들에게 패킷 보내기
	int SendToNearBy(class NetPlay::Packet* Packet);

public:
	// 더미 추가하기
	bool AddDummy(const std::shared_ptr< DummySession >& DummyPtr);

	// 주변에서 멀어진 더미 정보 보내기
	bool SendFaraway(float Length);

	// 주변에서 가까운 더미 정보 보내기
	bool SendNearBy(void);

public:
	inline void Dead(void)
	{
		isAlive_ = false;
	}

	inline bool IsAlive(void) const
	{
		return isAlive_;
	}

private:
	NetPlay::Packet* packet_ = nullptr;

	std::set< std::shared_ptr< DummySession > > closeGroup_;

	std::recursive_mutex lock_;

	bool isAlive_ = false;
};

#endif//_DUMMY_SESSION_H_