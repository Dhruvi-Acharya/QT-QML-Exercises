/*********************************************************************************
 * Copyright(c) 2020 by Hanwha Techwin, Inc.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin, Inc.
 * KOREA
 * http://www.hanwha-security.com
 *********************************************************************************/
/**
@file  ExceptionCode.h
@author YoungJoon Park
@date 2020. 2. 16.
@brief
*/

#ifndef CLIENT_EXCEPTIONCODE_H_
#define CLIENT_EXCEPTIONCODE_H_

namespace legosip
{

enum Exception_t
{
	EXC_NO_ERROR,				// no error
	EXC_NEW_FAIL,				// malloc/new fail
	EXC_SESSION_OPEN_FAIL,		// session open fail
	EXC_SESSION_OPEN_FAIL_DEVICE_ERROR,
	EXC_SESSION_OPEN_FAIL_FORBIDDEN,
	EXC_SESSION_OPEN_FAIL_NOT_ACCEPTABLE,
	EXC_SESSION_OPEN_FAIL_NOT_AVAILABLE,
	EXC_SESSION_OPEN_FAIL_DEVICE_BUSY,
	EXC_SESSION_OPEN_FAIL_SERVER_ERROR,
	EXC_SESSION_OPEN_FAIL_KEEP_ALIVE,
	EXC_SESSION_OPEN_FAIL_REGI_FAIL,
	EXC_SESSION_WRITE_ERROR,	// session write fail
	EXC_NO_MORE_DATA,			// data 가 없는 상태에서 대기 풀림
	EXC_RESPONSE_TIMEOUT,		// 응답대기시간 초과
	EXC_STRANGE_RESPONSE,		// 이상한 응답내용
	EXC_SESSION_OPEM_FAIL_BY_DISCONNECTION,    // SIP Signal Server로부터 접속실패 받는경우(ALC_DISCONNECTED)
	EXC_SESSION_CONNECT_DEVICE_ERROR,
	EXC_SESSION_CONNECT_TIMEOUT,
	EXC_SESSION_OPEN_CHANNEL_ERROR,
	EXC_NO_MORE_CHANNEL,
	EXC_SESSION_EXCEED_MAX_CLIENT
};

} /* namespace legosip */

#endif /* CLIENT_EXCEPTIONCODE_H_ */
