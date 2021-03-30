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
	// 소켓 정보를 Index 값으로 사용
	if (RemoteID)
		RemoteID->GetNativeSocket(index_);

	// 초기 위치 할당하기
	location_ = {
		Rand(0.f, Width),
		Rand(0.f, Height),
		0.f
	};
}

bool DummySession::ReqDummyInfo(class NetPlay::Packet* Packet)
{
	// 클라이언트로 온 더미 정보를 확인하고, 초기 위치 정보 알려주기
	if (Packet == nullptr)
		return false;

	*Packet
	>> &playable_;	// 플레이어 또는 더미 인지 여부

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
	// 위치 받아서 저장하기
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
	// 시야에 들어온 더미들에게 패킷 보내기 
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
	// 시야에 들어오면 시야 목록에 더미 추가하기
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
	/**
	* 시야에서 멀어지면 시야 목록에서 삭제하고 삭제한 더미 정보 보내기
	* kPagenation 만큼 가변적으로 보내기
	*/
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
	/**
	* 시야에 들어온 목록을 가지고 보내기
	* kPagenation 만큼 가변적으로 보내기
	*/
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

		// 플레이어일 경우 속도를, 더미일 경우 목표 지점을 전달한다
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