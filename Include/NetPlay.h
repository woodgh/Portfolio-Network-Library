/*
 * NetPlay.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _NET_PLAY_H_
#define _NET_PLAY_H_

#if defined(_WIN32)
#pragma warning(disable : 4455)

#else
#pragma GCC diagnostic ignored "-Wliteral-suffix"

#endif

#if defined(_WIN32)
#define PLAY_CALL __stdcall

#else
#define PLAY_CALL

#endif

#if defined(_WIN32)
#define PLAY_EXPORT __declspec(dllexport)

#elif defined(__APPLE__) || defined(__ANDROID__) || defined(__linux__)
#define PLAY_EXPORT __attribute__((visibility("default")))

#endif

#ifdef DLL_EXPORTS
#define PLAY_API PLAY_EXPORT PLAY_CALL

#else
#define PLAY_API PLAY_CALL

#endif

#define PLAY_DEFAULT_THREADS (-1)

constexpr unsigned long operator"" Bytes(unsigned long long Length)
{
	return (unsigned long)Length;
}

constexpr unsigned long operator"" KBytes(unsigned long long Length)
{
	return (unsigned long)Length * 1024;
}

constexpr unsigned long operator"" MBytes(unsigned long long Length)
{
	return (unsigned long)Length * 1024 * 1024;
}

constexpr unsigned long operator"" GBytes(unsigned long long Length)
{
	return (unsigned long)Length * 1024 * 1024 * 1024;
}

/*
* namespace NetPlay
*/
namespace NetPlay
{
	void PLAY_API Logging(const char* Fmt, ...);

	bool PLAY_API CreatePacket(class Packet** Packet, unsigned long Capacity = 4KBytes);
	bool PLAY_API CreateServer(class Server** Server, unsigned int NumThreads = PLAY_DEFAULT_THREADS);
	bool PLAY_API CreateClient(class Client** Client, unsigned int NumThreads = 0);

	/*
	* class IEventHandler
	*/
	class IEventHandler
	{
	public:
		virtual bool OnJoin(class RemoteID* RemoteID) { return true; }
		virtual bool OnDelivery(class RemoteID* RemoteID, class Packet* Packet, void* UserData) { return true; }
		virtual bool OnLeave(class RemoteID* RemoteID, int Reason) { return true; }
	};

	/*
	* class Packet
	*/
	class Packet
	{
	private:
		Packet(void) = default;
		Packet(const Packet&) = default;

	public:
		bool PLAY_API WritableBytes(unsigned long& Bytes);
		bool PLAY_API Write(char* Value, unsigned long Length);

		template< typename T >
		bool Write(T Value) { return Write((char*)&Value, sizeof(T)); }

		bool PLAY_API ReadableBytes(unsigned long& Bytes);
		bool PLAY_API Read(char* Value, unsigned long Length);

		template< typename T >
		bool Read(T* Value) { return Read((char*)Value, sizeof(T)); }

		bool PLAY_API  Rewind(unsigned long Size);

	public:
		template< typename T >
		inline Packet& operator << (T Value) { Write(Value); return *this; }

		template< typename T >
		inline Packet& operator >> (T* Value) { Read(Value); return *this; }
	};

	/*
	* class RemoteID
	*/
	class RemoteID
	{
	private:
		RemoteID(void) = default;
		RemoteID(const RemoteID&) = default;

	public:
		bool PLAY_API GetNativeSocket(int& Socket);
		void PLAY_API Disconnect(void);
		bool PLAY_API Send(class Packet* Packet);
		
		bool PLAY_API SetRecvBufferSize(unsigned long Size);
		bool PLAY_API SetSendBufferSize(unsigned long Size);
		bool PLAY_API SetUserData(void* UserData);
		bool PLAY_API GetUserData(void** UserData);

	public:
		template< class T >
		T* UserData(void)
		{
			void* Pointer = nullptr;
			GetUserData((void**)&Pointer);
			return reinterpret_cast< T* >(Pointer);
		}
	};

	/*
	* class Server
	*/
	class Server
	{
	private:
		Server(void) = default;
		Server(const Server&) = default;

	public:
		void PLAY_API Release(void);

		bool PLAY_API Shutdown(void);
		bool PLAY_API Start(const char* Address, unsigned short Port, class IEventHandler* Handler);
		void PLAY_API Update(void);
		bool PLAY_API SetRecvBufferSize(unsigned long Size);
		bool PLAY_API SetSendBufferSize(unsigned long Size);
	};

	/*
	* class Client
	*/
	class Client
	{
	private:
		Client(void) = default;
		Client(const Client&) = default;

	public:
		void PLAY_API Release(void);

		bool PLAY_API Shutdown(void);
		bool PLAY_API Connect(const char* Address, unsigned short Port, class IEventHandler* Handler);
		void PLAY_API Update(void);
	};
};

#endif//_NET_PLAY_H_
