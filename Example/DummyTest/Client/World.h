/*
 * World.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _WORLD_H_
#define _WORLD_H_

/*
 * class World
*/
class World : public wxPanel
{
public:
	World(wxWindow* Parent, int MaxRow, int MaxCol, float Distance, float ReduceScale);

	virtual ~World(void) = default;

public:
	// 월드 맵 초기화 하기
	bool Init(int NumClients);

	// 월드 맵 종료하기
	void Release(void);

	// 프로토콜 파싱하기
	bool OnParsing(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet);

	//월드 입장
	bool Enter(class NetPlay::RemoteID* RemoteID);

	// 월드 퇴장
	bool Leave(class NetPlay::RemoteID* RemoteID);

	void Select(class NetPlay::RemoteID* RemoteID);

public:
	// 업데이트
	void OnUpdate(void);

	// 그리기
	void OnPaint(wxPaintEvent& Event);
		
private:
	std::unordered_map< class NetPlay::RemoteID*, std::shared_ptr< class DummyClient > > dummyGroup_;

	std::shared_ptr< class DummyClient > selectDummy_;

	std::recursive_mutex lock_;

	int maxRow_ = 0;

	int maxCol_ = 0;

	float distance_ = 0.f;

	float width_ = 0.f;

	float height_ = 0.f;

	float reduceScale_ = 0.f;
};

#endif//_WORLD_H_