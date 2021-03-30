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
	bool Init(int MaxRow, int MaxCol, float Distance);

	void Release(void);

	bool OnParsing(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet, void* UserData);

	bool Enter(class NetPlay::RemoteID* RemoteID);

	bool Leave(class NetPlay::RemoteID* RemoteID);

private:
	bool DummyInfo(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet);

	bool MoveToLocation(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet);

	bool Chat(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet);

private:
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