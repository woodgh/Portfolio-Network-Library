/*
 * Protocol.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

/*
 * enum class Protocol
*/
enum class Protocol
{
	Unknown=				0,
	ReqDummyInfo,					// 더미 정보 보내기
	ResDummyInfo,					// 더미 정보 받기
	MoveToLocation,					// 위치 정보 보내고 받기
	Chat,							// 채팅 보내고 받기
	InSight,						// 시야에 들어옴
	OutOfSight,						// 시야에서 멀어짐
};

#endif//_PROTOCOL_H_