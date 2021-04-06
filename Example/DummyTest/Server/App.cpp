/*
 * App.cpp is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#include "Pch.h"
#include "Dummy.h"
#include "DummySession.h"
#include "World.h"

/**
* class ServerApp
*/
class ServerApp : public NetPlay::IEventHandler
{
public:
	ServerApp(void) = default;

	~ServerApp(void) = default;

public:
	/**
	* 서버 시작과 종료하기
	*/
	bool LaunchAndDestroy(void)
	{
		if (NetPlay::CreateServer(&server_) == false)
			return false;

		if (world_->Init(kWorldRow, kWorldCol, kWorldDistance) == false)
			return false;

		if (server_->Start("127.0.0.1", 20000, this) == false)
			return false;

		while (true)
			server_->Update();

		if (server_->Shutdown() == false)
			return false;

		server_->Release();

		world_->Release();

		return true;
	}

public:
	/**
	* 서버 입장하기
	*/
	virtual bool OnJoin(class NetPlay::RemoteID* Session) override
	{
		if (Session == nullptr)
			return false;

		if (world_->Enter(Session) == false)
			return false;

		return true;
	}

	/**
	* 서버 퇴장하기
	*/
	virtual bool OnLeave(class NetPlay::RemoteID* Session, int Reason) override
	{
		if (Session == nullptr)
			return false;

		if (world_->Leave(Session) == false)
			return false;

		return true;
	}

	/**
	* 서버로 도착한 패킷 확인하기
	*/
	virtual bool OnDelivery(class NetPlay::RemoteID* Session, class NetPlay::Packet* Packet) override
	{
		if (Session == nullptr)
			return false;

		if (Packet == nullptr)
			return false;

		unsigned int Length = 0;

		if (Packet->Read(&Length) == false)
			return false;

		if (Length <= 0)
			return false;

		if (world_->OnParsing(Session, Packet) == false)
			return false;

		return true;
	}

private:
	NetPlay::Server* server_ = nullptr;

	std::unique_ptr< World > world_ = std::make_unique< World >();
};

/**
* Entry Function
*/
int main(int Argc, char** Argv)
{
	if (ServerApp().LaunchAndDestroy() == false)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
