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
	// 더미 클라이언트는 false
	playable_ = false;
}

bool DummyClient::ReqDummyInfo(void)
{
	// 더미 정보 보내기
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
	// 서버로 초기 위치 받기
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
	// 위치 보내기
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
	// 시야에 들어온 더미 목록 추가하기
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
	// 시야에 멀어진 더미 목록 삭제하기
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
	// 더미 정보 업데이트
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

	// 시야에 들어온 더미 정보는 UI를 위해 이동 시뮬레이션 처리
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
	// 시야 목록 그리기
	for (auto& It : closeGroup_)
		Painter.DrawCircle(It.second.Location.X / ReduceScale, It.second.Location.Y / ReduceScale, 2);
}
