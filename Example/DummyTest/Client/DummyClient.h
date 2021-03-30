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
	// 기본 속력
	const float kDefaultSpeed = 600.f;

public:
	DummyClient(void) = default;

	DummyClient(class NetPlay::RemoteID* RemoteID);

	virtual ~DummyClient(void) = default;

public:
	// 더미 정보 보내기
	bool ReqDummyInfo(void);

	// 더미 정보 받기
	bool ResDummyInfo(class NetPlay::Packet* Packet);

	// 위치 보내기
	bool MoveToLocation(void);

	// 시야에 들어옴
	bool Insight(class NetPlay::Packet* Packet);

	// 시야에 멀어짐
	bool OutOfSight(class NetPlay::Packet* Packet);

public:
	// 업데이트
	void OnUpdate(float Width, float Height, float ReduceScale);

	// 그리기
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