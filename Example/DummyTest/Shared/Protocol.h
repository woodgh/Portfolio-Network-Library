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
	ReqDummyInfo,					// ���� ���� ������
	ResDummyInfo,					// ���� ���� �ޱ�
	MoveToLocation,					// ��ġ ���� ������ �ޱ�
	Chat,							// ä�� ������ �ޱ�
	InSight,						// �þ߿� ����
	OutOfSight,						// �þ߿��� �־���
};

#endif//_PROTOCOL_H_