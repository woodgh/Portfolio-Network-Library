/*
 * DummyClient.cpp is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#include "Pch.h"
#include "Dummy.h"
#include "DummyClient.h"

DummyClient::DummyClient(class NetPlay::RemoteID* Session)
	: Dummy(Session)
{
	playable_ = false;
}

bool DummyClient::ReqDummyInfo(void)
{
	NetPlay::Packet* Packet = nullptr;

	if (NetPlay::CreatePacket(&Packet) == false)
		return false;

	*Packet
		<< Protocol::ReqDummyInfo
		<< playable_;

	if (Send(Packet) == false)
		return false;

	return true;
}

bool DummyClient::ResDummyInfo(class NetPlay::Packet* Packet)
{
	if (Packet == nullptr)
		return false;

	*Packet
		>> &index_
		>> &location_;

	targetLocation_ = location_;

	return true;
}

bool DummyClient::MoveToLocation(void)
{
	NetPlay::Packet* Packet = nullptr;

	if (NetPlay::CreatePacket(&Packet) == false)
		return false;

	*Packet
		<< Protocol::MoveToLocation
		<< index_
		<< location_
		<< rotation_
		<< velocity_
		<< targetLocation_;

	if (Send(Packet) == false)
		return false;

	return true;
}

bool DummyClient::Insight(class NetPlay::Packet* Packet)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	if (Packet == nullptr)
		return false;

	unsigned long ReadBytes = 0;

	while (Packet->ReadableBytes(ReadBytes))
	{
		int Index = 0;

		if (Packet->Read(&Index) == false)
			continue;

		NearByDummy Dummy = { 0.f, };

		if (Packet->Read(&Dummy.Playable) == false)
			continue;

		if (Dummy.Playable)
		{
			*Packet
				>> &Dummy.Location
				>> &Dummy.Rotation
				>> &Dummy.Velocity;
		}
		else
		{
			*Packet
				>> &Dummy.Location
				>> &Dummy.Rotation
				>> &Dummy.TargetLocation;
		}

		auto Found = closeGroup_.find(Index);

		if (Found == closeGroup_.end())
			closeGroup_.insert(std::make_pair(Index, Dummy));
		else
			Found->second = Dummy;
	}

	return true;
}

bool DummyClient::OutOfSight(class NetPlay::Packet* Packet)
{
	std::lock_guard< std::recursive_mutex > Locker(lock_);

	if (Packet == nullptr)
		return false;

	unsigned long ReadBytes = 0;

	while (Packet->ReadableBytes(ReadBytes))
	{
		int Index = 0;

		if (Packet->Read(&Index) == false)
			continue;

		closeGroup_.erase(Index);
	}

	return true;
}

void DummyClient::OnUpdate(float Width, float Height, float ReduceScale)
{
	float DeltaTime = std::chrono::duration< float >(std::chrono::high_resolution_clock::now() - clock_).count();

	rotation_ = Normalize(targetLocation_ - location_);
	velocity_ = rotation_ * kDefaultSpeed;

	if (Distance(location_, targetLocation_) <= 100.f)
	{
		velocity_ = { 0.f };

		targetLocation_ = {
			Rand(0.f, Width * ReduceScale),
			Rand(0.f, Height * ReduceScale),
			0.f
		};
	}
	
	location_ = MoveFowards(location_, velocity_, DeltaTime);

	if (location_.X > Width * ReduceScale)
		location_.X = targetLocation_.X;

	if (location_.Y > Height * ReduceScale)
		location_.Y = targetLocation_.Y;

	for (auto& It : closeGroup_)
	{
		NearByDummy& Dummy = It.second;

		Dummy.Rotation = Normalize(Dummy.TargetLocation - Dummy.Location);
		Dummy.Velocity = Dummy.Rotation * kDefaultSpeed;

		Dummy.Location = MoveFowards(Dummy.Location, Dummy.Velocity, DeltaTime);
	}

	clock_ = std::chrono::high_resolution_clock::now();
}

void DummyClient::OnDraw(wxPaintDC& Painter, float ReduceScale)
{
	for (auto& It : closeGroup_)
		Painter.DrawCircle(It.second.Location.X / ReduceScale, It.second.Location.Y / ReduceScale, 2);
}
