/*
 * World.cpp is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#include "Pch.h"
#include "Dummy.h"
#include "DummySession.h"
#include "World.h"

/*
* class Sector
*/
class Sector
{
public:
	Sector(int Index, int Row, int Col)
		: index_(Index)
		, row_(Row)
		, col_(Col)
	{
	}

	~Sector(void) = default;

public:
	// 주변 섹터 추가하기
	void AddSector(std::shared_ptr< Sector > SectorPtr)
	{
		std::lock_guard< std::recursive_mutex > Locker(lock_);

		sectorGroup_.insert(SectorPtr);
	}

	// 섹터 파괴하기
	void Destroy(void)
	{
		std::lock_guard< std::recursive_mutex > Locker(lock_);

		sectorGroup_.clear();
	}

	// 섹터 입장
	bool Enter(std::shared_ptr< DummySession >& DummyPtr)
	{
		std::lock_guard< std::recursive_mutex > Locker(lock_);

		if (DummyPtr == nullptr)
			return false;

		if (dummyGroup_.find(DummyPtr) != dummyGroup_.end())
			return false;

		dummyGroup_.insert(DummyPtr);

		return true;
	}

	// 섹터 퇴장
	bool Leave(std::shared_ptr< DummySession >& DummyPtr)
	{
		std::lock_guard< std::recursive_mutex > Locker(lock_);

		if (DummyPtr == nullptr)
			return false;

		if (dummyGroup_.find(DummyPtr) == dummyGroup_.end())
			return false;

		dummyGroup_.erase(DummyPtr);

		return true;
	}

public:
	// 섹터에 들어온 더미들 중 시야에 드어온 더미를 추가하기
	void SearchNearBy(std::shared_ptr< DummySession >& DummyPtr, float Length, bool SearchGroup = true)
	{
		lock_.lock();

		for (auto& It : dummyGroup_)
		{
			if (It->GetIndex() == DummyPtr->GetIndex())
				continue;

			if (Distance(It->GetLocation(), DummyPtr->GetLocation()) > Length)
				continue;

			DummyPtr->AddDummy(It);
		}

		lock_.unlock();

		if (SearchGroup == false)
			return;

		// 인근 섹터에서 찾기
		for (auto& It : sectorGroup_)
			It->SearchNearBy(DummyPtr, Length, false);
	}

public:
	inline int GetIndex(void)
	{
		return index_;
	}

	inline int Row(void)
	{
		return row_;
	}

	inline int Col(void)
	{
		return col_;
	}

private:
	int index_ = 0;

	int row_ = 0;

	int col_ = 0;

	std::set< std::shared_ptr< Sector > > sectorGroup_;

	std::set< std::shared_ptr< DummySession > > dummyGroup_;

	std::recursive_mutex lock_;
};

bool World::Init(int MaxRow, int MaxCol, float Distance)
{
	/**
	* 월드 초기화
	* 요청 크기 기준으로 섹터를 생성 및 주변 섹터 지정하기
	*/
	if (MaxRow <= 0 || MaxCol < 0)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Size: %d/%d", MaxRow, MaxCol);
		return false;
	}

	if (Distance <= 0.f)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Distance: %.1f", Distance);
		return false;
	}

	sectorGroup_.reserve(MaxRow * MaxCol);

	int Index = 0;

	// 섹터에 지역 번호 지정하기
	for(int Row: Range(MaxRow))
	{
		for (int Col : Range(MaxCol))
		{
			sectorGroup_.push_back(
				std::make_shared< Sector >(Index++, Row, Col)
			);
		}
	}

	// 8방향의 인근 섹터 추가하기
	for (auto& SectorPtr : sectorGroup_)
	{
		// Top
		if (SectorPtr->Row() > 0 && SectorPtr->Row() <= MaxRow - 1)
		{
			// LeftTop
			if (SectorPtr->Col() > 0 && SectorPtr->Col() <= MaxCol - 1)
				SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() - MaxCol - 1]);

			// Top
			SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() - MaxCol]);

			// RightTop
			if (SectorPtr->Col() >= 0 && SectorPtr->Col() < MaxCol - 1)
				SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() - MaxCol + 1]);
		}

		// Left
		if (SectorPtr->Col() > 0 && SectorPtr->Col() <= MaxCol - 1)
			SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() - 1]);

		// Right
		if (SectorPtr->Col() >= 0 && SectorPtr->Col() < MaxCol - 1)
			SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() + 1]);

		// Bottom
		if (SectorPtr->Row() >= 0 && SectorPtr->Row() < MaxRow - 1)
		{
			// LeftBottom
			if (SectorPtr->Col() > 0 && SectorPtr->Col() <= MaxCol - 1)
				SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() + MaxCol - 1]);

			// Bottom
			SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() + MaxCol]);

			// RightBottom
			if (SectorPtr->Col() >= 0 && SectorPtr->Col() < MaxCol - 1)
				SectorPtr->AddSector(sectorGroup_[SectorPtr->GetIndex() + MaxCol + 1]);
		}
	}


	maxRow_ = MaxRow, maxCol_ = MaxCol;
	distance_ = Distance;
	width_ = MaxCol * Distance;
	height_ = MaxRow * Distance;

	NetPlay::Logging(PLAY_LOG_INFO, "Width: %.1f, Height: %.1f", width_, height_);
	NetPlay::Logging(PLAY_LOG_INFO, "Rows: %d, Cols: %d", maxRow_, maxCol_);
	NetPlay::Logging(PLAY_LOG_INFO, "Distance: %.1f", distance_);
	NetPlay::Logging(PLAY_LOG_INFO, "Sector: %d", sectorGroup_.size());

	return true;
}

void World::Release(void)
{
	// 모든 내용 정리 및 종료하기
	for (auto& SectorPtr : sectorGroup_)
		SectorPtr->Destroy();

	sectorGroup_.clear();

	maxRow_ = 0, maxCol_ = 0;
	distance_ = 0.f;
	width_ = 0.f;
	height_ = 0.f;
}

bool World::OnParsing(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet, void* UserData)
{
	// 프로토콜 파싱하기
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

	std::shared_ptr< DummySession > DummyPtr = RemoteID->UserData< DummySession >()->SharedPtr();

	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Dummy");
		return false;
	}

	Protocol ProtoType = Protocol::Unknown;

	if (Packet->Read(&ProtoType) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Packet: %5d, Dummy: %5d", ProtoType, DummyPtr->GetIndex());
		return false;
	}

	bool Result = false;

	switch (ProtoType)
	{
		case Protocol::ReqDummyInfo:	// 더미 정보
			Result = DummyInfo(DummyPtr, Packet);
			break;

		case Protocol::MoveToLocation:	// 위치 정보
			Result = MoveToLocation(DummyPtr, Packet);
			break;

		case Protocol::Chat:			// 채팅
			Result = Chat(DummyPtr, Packet);
			break;
	}

	if (Result == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Unknown Parsing: %5d, Dummy: %5d", ProtoType, DummyPtr->GetIndex());
		return false;
	}

	return Result;
}

bool World::Enter(class NetPlay::RemoteID* RemoteID)
{
	// 월드 입장하면 더미 생성하기
	if (RemoteID == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Unknown RemoteID");
		return false;
	}

	std::shared_ptr< DummySession > DummyPtr = std::make_shared< DummySession >(RemoteID, width_, height_);

	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Unknown Dummy");
		return false;
	}

	// 초기 위치 기준으로 입장할 섹터 찾고 입장하기
	std::shared_ptr < Sector > SectorPtr = Where(DummyPtr->GetLocation());

	if (SectorPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Unknown Sector, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	if (SectorPtr->Enter(DummyPtr) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Enter World, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	return true;
}

bool World::Leave(class NetPlay::RemoteID* RemoteID)
{
	// 월드에 퇴장하고 더미 종료하기
	if (RemoteID == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Leave World, Unknown RemoteID");
		return false;
	}

	std::shared_ptr< DummySession > DummyPtr = RemoteID->UserData< DummySession >()->SharedPtr();
	
	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Leave World, Unknown Dummy");
		return false;
	}

	// 퇴장할 섹터 찾고 퇴장하기
	std::shared_ptr < Sector > SectorPtr = Where(DummyPtr->GetLocation());

	if (SectorPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Leave World, Unknown Sector, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	if (SectorPtr->Leave(DummyPtr) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Leave World, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	// 더미 삭제
	DummyPtr->Dead();

	return true;
}

bool World::DummyInfo(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet)
{
	// 클라이언트가 접속 후 더미 정보가 전달된다
	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "DummyInfo, Unknown Dummy");
		return false;
	}

	if (Packet == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "DummyInfo, Unknown Packet, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	if (DummyPtr->ReqDummyInfo(Packet) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "DummyInfo, ReqDummyInfo, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	return true;
}

bool World::MoveToLocation(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet)
{
	// 클라이언트로 온 위치 정보 가지고 섹터 처리 및 인근 위치 정보 가져오기
	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Unknown Dummy");
		return false;
	}

	if (Packet == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Unknown Packet, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}
	
	// 현재 섹터 확인하기
	std::shared_ptr< class Sector > CurSectorPtr = Where(DummyPtr->GetLocation());

	if (CurSectorPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Unknown Sector, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	// 갱신된 위치
	if (DummyPtr->MoveToLocation(Packet) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	// 갱신된 위치로 부터 새로운 섹터 확인하기
	std::shared_ptr< class Sector > NewSectorPtr = Where(DummyPtr->GetLocation());

	if (NewSectorPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Unknown Sector, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	// 이전 섹터와 새로운 섹터가 다를 경우 퇴장, 입장 하기
	if (CurSectorPtr != NewSectorPtr)
	{
		if (CurSectorPtr->Leave(DummyPtr) == false)
		{
			NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Can not leave to Sector, Dummy: %5d", DummyPtr->GetIndex());
			return false;
		}

		if (NewSectorPtr->Enter(DummyPtr) == false)
		{
			NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Can not enter to Sector, Dummy: %5d", DummyPtr->GetIndex());
			return false;
		}

		CurSectorPtr = NewSectorPtr;
	}

	// 시야에서 멀어진 더미 정보 알려주기
	if (DummyPtr->SendFaraway(distance_) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Check Far away, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	// 현재 섹터에서 시야에 들어온 더미 정보 정보 가져오기
	CurSectorPtr->SearchNearBy(DummyPtr, distance_);

	// 시야에 들어온 더미 알려주기
	if (DummyPtr->SendNearBy() == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Move to Location, Check Nearby, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}
	
	return true;
}

bool World::Chat(std::shared_ptr< class DummySession >& DummyPtr, class NetPlay::Packet* Packet)
{
	// 채팅
	if (DummyPtr == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Relay to Chat, Unknown Dummy");
		return false;
	}

	if (Packet == nullptr)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Relay to Chat, Unknown Packet, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	// 새로 패킷 구성이 아닌 릴레이를 위해 앞부분으로 이동(길이 + 패킷 타입 = 8Bytes)
	if (Packet->Rewind(8Bytes) == false)
	{
		NetPlay::Logging(PLAY_LOG_ERROR, "Relay to Chat, Rewind Packet, Dummy: %5d", DummyPtr->GetIndex());
		return false;
	}

	if (DummyPtr->SendToNearBy(Packet) > 0)
		NetPlay::Logging(PLAY_LOG_WARNING, "Relay to Chat, SendNearBy, Dummy: %5d", DummyPtr->GetIndex());

	return true;
}

std::shared_ptr< class Sector > World::Where(Vector3& Location)
{
	int Row = int(Location.Y / distance_);
	int Col = int(Location.X / distance_);

	int Index = (Row * maxCol_) + Col;

	std::shared_ptr< class Sector > SectorPtr;

	if (Index >= 0 && Index < sectorGroup_.size())
		SectorPtr = sectorGroup_[Index];

	if (Index < 0 || Index >= sectorGroup_.size())
	{
		NetPlay::Logging(PLAY_LOG_INFO, "%5d/%5d -> %+05.5f/%+05.5f/%+05.5f",
			Index,
			sectorGroup_.size(),
			Location.X,
			Location.Y,
			Location.Z
		);
	}

	return SectorPtr;
}