/*
 * DummySession.cpp is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#include "Pch.h"
#include "Dummy.h"
#include "DummySession.h"

DummySession::DummySession(class NetPlay::RemoteID* RemoteID, float Width, float Height)
	: Dummy(RemoteID)
	, isAlive_(true)
{
	if (RemoteID)
		RemoteID->GetNativeSocket(index_);

	location_ = {
		Rand(0.f, Width),
		Rand(0.f, Height),
		0.f
	};
}

bool DummySession::ReqDummyInfo(class NetPlay::Packet* Packet)
{
	if (Packet == nullptr)
		return false;

	*Packet
	>> &playable_;

	if (NetPlay::CreatePacket(&Packet) == false)
		return false;

	*Packet
		<< Protocol::ResDummyInfo
		<< index_
		<< location_;

	if (Send(Packet) == false)
		return false;

	return true;
}

bool DummySession::MoveToLocation(NetPlay::Packet* Packet)
{
	if (Packet == nullptr)
		return false;

	int Index = 0;

	if (Packet->Read(&Index) == false)
		return false;

	if (Index != index_)
		return false;

	*Packet
		>> &location_
		>> &rotation_
		>> &velocity_
		>> &targetLocation_;

	return true;
}

int DummySession::SendToNearBy(class NetPlay::Packet* Packet)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	int NumFailure = 0;

	for (auto& It : closeGroup_)
	{
		if (It == nullptr)
			continue;

		if (It->Send(Packet) == false)
			++NumFailure;
	}

	return NumFailure;
}

bool DummySession::AddDummy(const std::shared_ptr< DummySession >& DummyPtr)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	if (DummyPtr == nullptr)
		return false;

	if (closeGroup_.find(DummyPtr) != closeGroup_.end())
		return false;

	closeGroup_.insert(DummyPtr);

	return true;
}

bool DummySession::SendFaraway(float Length)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	int NumSendable = 0;

	for(auto It = closeGroup_.begin(); It != closeGroup_.end(); )
	{
		if ((*It)->IsAlive() && Distance((*It)->GetLocation(), location_) <= Length)
		{
			++It;

			continue;
		}

		if (NumSendable % kMaxPagination == 0)
		{
			if (NumSendable > 0 && Send(packet_) == false)
				return false;

			if (NetPlay::CreatePacket(&packet_) == false)
				return false;

			if (packet_->Write(Protocol::OutOfSight) == false)
				return false;

			NumSendable = 0;
		}

		*packet_
			<< (*It)->GetIndex();

		++NumSendable;

		It = closeGroup_.erase(It);
	}

	if (NumSendable > 0 && Send(packet_) == false)
		return false;

	return true;
}

bool DummySession::SendNearBy(void)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	int NumSendable = 0;

	for (auto& It : closeGroup_)
	{
		if (NumSendable % kMaxPagination == 0)
		{
			if (NumSendable > 0 && Send(packet_) == false)
				return false;

			if (NetPlay::CreatePacket(&packet_) == false)
				return false;

			*packet_
				<< Protocol::InSight;

			NumSendable = 0;
		}

		if (It->GetPlayable())
		{
			*packet_
				<< It->GetIndex()
				<< It->GetPlayable()
				<< It->GetLocation()
				<< It->GetRotation()
				<< It->GetVelocity();
		}
		else
		{
			*packet_
				<< It->GetIndex()
				<< It->GetPlayable()
				<< It->GetLocation()
				<< It->GetRotation()
				<< It->GetTargetLocation();
		}

		++NumSendable;
	}

	if (NumSendable > 0 && Send(packet_) == false)
		return false;

	return true;
}