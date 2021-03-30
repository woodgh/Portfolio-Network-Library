/*
 * DummyClient.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _DUMMY_CLIENT_H_
#define _DUMMY_CLIENT_H_

/*
 * class DummyClient
*/
class DummyClient : public Dummy< DummyClient >
{
public:
	/*
	  * struct NearByDummy
	 */
	typedef struct NearByDummy
	{
		Vector3 Location = { 0.f, };

		Vector3 Rotation = { 0.f, };

		Vector3 Velocity = { 0.f, };

		Vector3 TargetLocation = { 0.f, };

		bool Playable = false;

	} NearByDummy;

public:
	const float kDefaultSpeed = 600.f;

public:
	DummyClient(void) = default;

	DummyClient(class NetPlay::RemoteID* RemoteID);

	virtual ~DummyClient(void) = default;

public:
	bool ReqDummyInfo(void);

	bool ResDummyInfo(class NetPlay::Packet* Packet);

	bool MoveToLocation(void);

	bool Insight(class NetPlay::Packet* Packet);

	bool OutOfSight(class NetPlay::Packet* Packet);

public:
	void OnUpdate(float Width, float Height, float ReduceScale);

	void OnDraw(wxPaintDC& Painter, float ReduceScale);

public:
	inline int NumNearBy(void) const
	{
		return closeGroup_.size();
	}

	inline bool HasDummy(int Index)
	{
		if (closeGroup_.find(Index) == closeGroup_.end())
			return false;

		return true;
	}

private:
	std::map< int, NearByDummy > closeGroup_;

	std::chrono::high_resolution_clock::time_point clock_ = std::chrono::high_resolution_clock::now();

	std::recursive_mutex lock_;
};

#endif//_DUMMY_CLIENT_H_