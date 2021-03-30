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
	bool ReqDummyInfo(class NetPlay::Packet* Packet);

	bool MoveToLocation(class NetPlay::Packet* Packet);

	int SendToNearBy(class NetPlay::Packet* Packet);

public:
	bool AddDummy(const std::shared_ptr< DummySession >& DummyPtr);

	bool SendFaraway(float Length);

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