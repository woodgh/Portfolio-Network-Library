/*
 * World.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _WORLD_H_
#define _WORLD_H_

/*
* class World
*/
class World
{
public:
	World(void) = default;

	~World(void) = default;

public:
	// 월드 초기화 하기
	bool Init(int MaxRow, int MaxCol, float Distance);

	// 월드 해제 하기
	void Release(void);

	// 프로토콜 파싱하기
	bool OnParsing(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet, void* UserData);

	// 월드 입장
	bool Enter(class NetPlay::RemoteID* RemoteID);

	// 월드 퇴장
	bool Leave(class NetPlay::RemoteID* RemoteID);

private:
	// 더미 정보 받기
	bool DummyInfo(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet);

	// 위치 정보 받기
	bool MoveToLocation(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet);

	// 채팅 받기
	bool Chat(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet);

private:
	// 위치 정보로 섹터 찾기
	std::shared_ptr< class Sector > Where(Vector3& Pos);

private:
	std::vector< std::shared_ptr< class Sector > > sectorGroup_;

	float width_ = 0;

	float height_ = 0;

	float distance_ = 0;

	int maxRow_ = 0;

	int maxCol_ = 0;
};

#endif//_WORLD_H_