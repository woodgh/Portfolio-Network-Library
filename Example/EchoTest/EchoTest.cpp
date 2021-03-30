#include <cstdio>
#include <cstdlib>
#include <NetPlay.h>

/**
* class EchoTest
*/
class EchoTest : public NetPlay::IEventHandler
{
public:
	EchoTest(void) = default;

	~EchoTest(void) = default;

public:
	bool LaunchAndDestroy(void)
	{
		/**
		*	서버 시작 과 종료하기
		*/
		if (NetPlay::CreateServer(&server_, PLAY_DEFAULT_THREADS) == false)
			return false;

		if (server_->Start("127.0.0.1", 20000, this) == false)
			return false;

		while (true)
			server_->Update();

		if (server_->Shutdown() == false)
			return false;

		server_->Release();

		return true;
	}

public:
	virtual bool OnDelivery(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet, void* UserData) override
	{
		/**
		* 받은 패킷 다시 돌려주기
		*/
		if (RemoteID->Send(Packet) == false)
			return false;

		return true;
	}

private:
	NetPlay::Server* server_ = nullptr;
};

/**
* Entry Function
*/
int main(int Argc, char** Argv)
{
	if (EchoTest().LaunchAndDestroy() == false)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
