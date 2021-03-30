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
	bool Init(int NumClients);

	void Release(void);

	bool OnParsing(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet, void* UserData);

	bool Enter(class NetPlay::RemoteID* RemoteID);

	bool Leave(class NetPlay::RemoteID* RemoteID);

	void Select(class NetPlay::RemoteID* RemoteID);

public:
	void OnUpdate(void);

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