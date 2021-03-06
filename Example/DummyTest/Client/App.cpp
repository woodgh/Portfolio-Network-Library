/*
 * App.cpp is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#include "Pch.h"
#include "Dummy.h"
#include "DummyClient.h"
#include "World.h"

/*
* class ClientApp
*/
class ClientApp : public wxApp
{
public:
	const static int kMaxClients = 1000;

public:
	/*
	* class MainFrame
	*/
	class MainFrame : public wxFrame, public NetPlay::IEventHandler
	{
	public:
		MainFrame(const wxString& Name, int MaxRow, int MaxCol, float Distance, float ReduceScale)
			: wxFrame(NULL, wxID_ANY, Name, wxDefaultPosition, wxSize(MaxCol * Distance + 35 + 200, MaxRow * Distance + 60), wxCAPTION | wxCLOSE_BOX)
			, world_(new World(this, MaxRow, MaxCol, Distance, ReduceScale))
			, dummySelector_(new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(200, 0)))
			, sizer_(new wxBoxSizer(wxHORIZONTAL))
			, timer_(new wxTimer(this))
		{
			// Layout
			if (sizer_)
			{
				sizer_->Add(world_, 0, wxEXPAND | wxALL, 10);
				sizer_->Add(dummySelector_, 0, wxEXPAND | wxALL, 10);

				SetSizer(sizer_);
			}

			if (timer_)
				timer_->Start(1);

			Connect(wxEVT_TIMER, wxCommandEventHandler(MainFrame::OnUpdate));

			Centre();
		}

		virtual ~MainFrame(void) = default;

	public:
		bool Initialize(int MaxClients, int MaxThreads)
		{
			// 월드 생성 및 클라이언트 생성하기
			if (world_->Init(MaxClients) == false)
				return false;

			if (NetPlay::CreateClient(&client_, MaxThreads) == false)
				return false;

			int NumFailure = 0;

			for (int Num : Range(MaxClients))
			{
				if (client_->Connect("127.0.0.1", 20000, this) == false)
					++NumFailure;
			}

			isActive_ = true;

			return true;
		}

		virtual void OnUpdate(wxCommandEvent& event)
		{
			// 월드 업데이트 및 클라이어늩 업데이트
			if (isActive_ == false)
				return;

			if (world_)
				world_->OnUpdate();

			// 리스트 박스의 더미 목록 중 선택된 더미 정보 업데이트하기
			if (dummySelector_)
			{
				int Sel = dummySelector_->GetSelection();

				if (Sel != -1)
				{
					if (world_)
						world_->Select(reinterpret_cast< class NetPlay::RemoteID* >(dummySelector_->GetClientData(Sel)));
				}
			}

			world_->Refresh();
		}

		virtual bool Destroy() override
		{
			// 종료하기
			isActive_ = false;

			if (timer_)
				timer_->Stop();

			if (client_->Shutdown() == false)
				return false;

			client_->Release();

			if (world_)
				world_->Release();

			if (wxFrame::Destroy() == false)
				return false;

			return true;
		}

	public:
		virtual bool OnJoin(class NetPlay::RemoteID* RemoteID)
		{
			// 월드 입장
			if (RemoteID == nullptr)
				return false;

			if (world_->Enter(RemoteID) == false)
				return false;

			// 리스트 박스에 접속한 더미 목록 추가하기
			int Socket = 0;

			if (RemoteID->GetNativeSocket(Socket) == false)
				return false;

			if (dummySelector_)
				dummySelector_->Append(wxString::Format(wxT("Client(%d)"), Socket), RemoteID);

			return true;
		}

		virtual bool OnLeave(class NetPlay::RemoteID* RemoteID, int Reason)
		{
			// 월드 퇴장
			if (RemoteID == nullptr)
				return false;

			if (world_->Leave(RemoteID) == false)
				return false;

			return true;
		}

		virtual bool OnDelivery(class NetPlay::RemoteID* RemoteID, class NetPlay::Packet* Packet)
		{
			// 프로토콜 파싱하기
			if (RemoteID == nullptr)
				return false;

			if (Packet == nullptr)
				return false;

			unsigned int Length = 0;

			if (Packet->Read(&Length) == false)
				return false;

			if (Length <= 0)
				return false;

			if (world_->OnParsing(RemoteID, Packet) == false)
				return false;

			return true;
		}

	private:
		World* world_= nullptr;

		wxListBox* dummySelector_ = nullptr;

		wxBoxSizer* sizer_ = nullptr;

		wxTimer* timer_ = nullptr;

		NetPlay::Client* client_ = nullptr;

		bool isActive_ = false;
	};

public:
	ClientApp(void)
		: mainFrm_(new MainFrame("DummyClient", kWorldRow, kWorldCol, kWorldDistance > kWorldMaxDistance ? kWorldMaxDistance : kWorldDistance, kWorldDistance > kWorldMaxDistance ? kWorldDistance / kWorldMaxDistance : 1))
	{
		
	}

	virtual ~ClientApp(void) = default;

public:
	virtual bool OnInit() override
	{
		// UI 생성하기
		if (wxApp::OnInit() == false)
			return false;

		if (mainFrm_->Show(true) == false)
			return false;

		if (mainFrm_->Initialize(maxClients_, maxThreads_) == false)
			return false;

		return true;
	}

	virtual void OnInitCmdLine(wxCmdLineParser& Parser) override
	{
		// Cmd Line 명령어 정의하기
		wxApp::OnInitCmdLine(Parser);

		const static wxCmdLineEntryDesc CmdDesc[] = {
			{ wxCMD_LINE_OPTION, "c", "clients", "Number Of Clients", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
			{ wxCMD_LINE_OPTION, "t", "threads", "Number Of Threads", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
			wxCMD_LINE_DESC_END
		};

		Parser.SetDesc(CmdDesc);
	}

	virtual bool OnCmdLineParsed(wxCmdLineParser& Parser) override
	{
		// Cmd Line 명령어 처리하기
		if (Parser.Found("clients", (long*)&maxClients_) == false)
			maxClients_ = kMaxClients;

		if (Parser.Found("threads", (long*)&maxThreads_) == false)
			maxThreads_ = std::thread::hardware_concurrency();

		return wxApp::OnCmdLineParsed(Parser);
	}
	
private:
	MainFrame* mainFrm_ = nullptr;

	int maxClients_ = 0;

	int maxThreads_ = 0;
};

IMPLEMENT_APP(ClientApp)