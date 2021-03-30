/*
 * World.cpp is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#include "Pch.h"
#include "Dummy.h"
#include "DummyClient.h"
#include "World.h"

// 'wxPen::wxPen': deprecated: use wxPENSTYLE_XXX constants
#pragma warning(disable : 4996)

World::World(wxWindow* Parent, int MaxRow, int MaxCol, float Distance, float ReduceScale)
	: wxPanel(Parent, wxID_ANY, wxDefaultPosition, wxSize(MaxCol * Distance, MaxRow * Distance))
	, maxRow_(MaxRow)
	, maxCol_(MaxCol)
	, distance_(Distance)
	, width_(MaxCol * Distance)
	, height_(MaxRow * Distance)
	, reduceScale_(ReduceScale)
{
	Connect(wxEVT_PAINT, wxPaintEventHandler(World::OnPaint));
}

bool World::Init(int NumClients)
{
	return true;
}

void World::Release(void)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	dummyGroup_.clear();
}

bool World::OnParsing(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet, void* UserData)
{
	if (RemoteID == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown RemoteID");
		return false;
	}

	if (Packet == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Packet");
		return false;
	}

	std::shared_ptr< DummyClient > DummyPtr = RemoteID->UserData< DummyClient >()->SharedPtr();

	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown DummyPtr");
		return false;
	}

	Protocol ProtoType = Protocol::Unknown;

	if (Packet->Read(&ProtoType) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Packet: %5d, Index: %5d", ProtoType, DummyPtr->GetIndex());
		return false;
	}

	bool Result = true;

	switch (ProtoType)
	{
		case Protocol::ResDummyInfo:
			Result = DummyPtr->ResDummyInfo(Packet);
			break;

		case Protocol::InSight:
			Result = DummyPtr->Insight(Packet);
			break;

		case Protocol::OutOfSight:
			Result = DummyPtr->OutOfSight(Packet);
			break;
	}

	if (Result == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Parsing: %5d, Index: %5d", ProtoType, DummyPtr->GetIndex());
		return false;
	}

	return Result;
}

bool World::Enter(class NetPlay::RemoteID* RemoteID)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	if (RemoteID == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Unknown RemoteID");
		return false;
	}

	std::shared_ptr< DummyClient > DummyPtr = std::make_shared< DummyClient >(RemoteID);

	if (dummyGroup_.find(RemoteID) != dummyGroup_.end())
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Already Dummy, Index: %5d", DummyPtr->GetIndex());
		return false;
	}

	if (DummyPtr->ReqDummyInfo() == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Dummy Info, Index: %5d", DummyPtr->GetIndex());
		return false;
	}

	dummyGroup_.insert(std::make_pair(RemoteID, DummyPtr));

	return true;
}

bool World::Leave(class NetPlay::RemoteID* RemoteID)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	if (RemoteID == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Leave World, Unknown RemoteID");
		return false;
	}

	std::shared_ptr< DummyClient > DummyPtr = RemoteID->UserData< DummyClient >()->SharedPtr();

	if (dummyGroup_.find(RemoteID) == dummyGroup_.end())
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Not Found Dummy, Index: %5d", DummyPtr->GetIndex());
		return false;
	}

	dummyGroup_.erase(RemoteID);

	return true;
}

void World::Select(class NetPlay::RemoteID* RemoteID)
{
	if (RemoteID != nullptr)
		selectDummy_ = RemoteID->UserData< DummyClient >()->SharedPtr();
	else
		selectDummy_ = nullptr;
}

void World::OnUpdate(void)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	for (auto& It : dummyGroup_)
	{
		std::shared_ptr< DummyClient >& DummyPtr = It.second;

		if (DummyPtr == nullptr)
			continue;

		if (DummyPtr->Session() == nullptr)
			continue;

		if (DummyPtr->GetIndex() <= 0)
			continue;

		DummyPtr->OnUpdate(width_, height_, reduceScale_);

		if (DummyPtr->MoveToLocation() == false)
			NetPlay::Logging(PLAY_LOG_WARNING, "Replicate to Location, Index: %5d", DummyPtr->GetIndex());
	}
}

void World::OnPaint(wxPaintEvent& Event)
{
	wxPaintDC Painter(this);
	
	int Hint = 1;

	for (int Row : Range(maxRow_))
		Painter.DrawLine(0, Row * distance_ - Hint, width_, Row * distance_ - Hint);

	for (int Col : Range(maxCol_))
		Painter.DrawLine(Col * distance_ - Hint, 0, Col * distance_ - Hint, height_);

	Painter.SetBrush(*wxTRANSPARENT_BRUSH);
	Painter.SetPen(wxPen(*wxBLACK, 0));

	lock_.lock();

	for (auto& It : dummyGroup_)
	{
		std::shared_ptr< DummyClient >& DummyPtr = It.second;

		if (DummyPtr == nullptr)
			continue;

		if (DummyPtr->Session() == nullptr)
			continue;

		if (DummyPtr->GetIndex() <= 0)
			continue;

		if (DummyPtr == selectDummy_)
			continue;

		Painter.DrawCircle(DummyPtr->GetLocation().X / reduceScale_, DummyPtr->GetLocation().Y / reduceScale_, 2);
	}

	lock_.unlock();

	if (selectDummy_)
	{
		Painter.SetBrush(*wxRED);
		selectDummy_->OnDraw(Painter, reduceScale_);
		
		Painter.SetBrush(*wxBLUE);
		Painter.DrawCircle(selectDummy_->GetLocation().X / reduceScale_, selectDummy_->GetLocation().Y / reduceScale_, 2);

		Painter.SetBrush(*wxTRANSPARENT_BRUSH);
		Painter.SetPen(wxPen(*wxBLACK, 1));
		Painter.DrawText(wxString::Format(wxT("%d (%d)"), selectDummy_->GetIndex(), selectDummy_->NumNearBy()), selectDummy_->GetLocation().X / reduceScale_ - 33, selectDummy_->GetLocation().Y / reduceScale_ - 23);
		Painter.DrawCircle(selectDummy_->GetLocation().X / reduceScale_, selectDummy_->GetLocation().Y / reduceScale_, distance_);
	}

	SetDoubleBuffered(true);
}