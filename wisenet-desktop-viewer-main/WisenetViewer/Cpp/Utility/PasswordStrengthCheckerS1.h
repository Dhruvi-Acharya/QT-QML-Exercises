/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#pragma once

#include <string>
#include <vector>
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

#ifdef DEF_S1_2019
#define PASSWD_MAX_LENGTH 16
#else
#define PASSWD_MAX_LENGTH 15
#endif

#define PASSWD_MIN_LENGTH 8
#define LENGTH_CHECK_RANGE_1 10
#define RANGE1_COMBINATION_COUNT 3
#define LENGTH_CHECK_RANGE_2 PASSWD_MAX_LENGTH
#define RANGE2_COMBINATION_COUNT 2
#define MAX_CHECK_STRING_COUNT 3
#define FORBIDDEN_WORDS 4
#define ID_MAX_LENGTH 16

class PasswordStrengthCheckerS1
{
	typedef enum
	{
		COMBINATION_CHECK_OK = 0,
		COMBINATION_CHECK_ERROR_RANGE1,
		COMBINATION_CHECK_ERROR_RANGE2,
		COMBINATION_CHECK_ERROR_MAX
	}COMBINATION_CHECK;

public:
    PasswordStrengthCheckerS1();
    virtual ~PasswordStrengthCheckerS1();

public:
    static PasswordStrengthCheckerS1* mpInstance;
    static PasswordStrengthCheckerS1* getInstance();

	unsigned int GetPasswordLevel(const char*, const char*);	/* Class public member */
	unsigned int GetPasswordLevel(const char*); /* Class public member */
	unsigned int GetIdLevel(const char*); /* Class public member */	

private:
	int strLength;					/* Password Length */
	int checkCompareCount;			/* Cheking count of String Continuity or Repeatability */
    std::string strID, strPasswd;		/* Class ID String, Class Password string */

    std::vector<std::string> forbiddenWords;	/* Class vector : forbiddend string */
    std::vector<std::string> keyboardWords;	/* Class vector : keyboard key layer */

	bool isDigit(const char);		/* Class member isDigit */
	bool isAlphaUpperCase(const char);			/* Class member isAlphaUperCase */
	bool isAlphaLowerCase(const char);	/* Class member isAlphaLowerCase */
	bool isSpecial(const char);		/* Class member isSpecial */

	bool CompareWords(const char, const char);
    bool CheckKeyboardWords(const std::string);

private:
	bool CheckSameAsID();					/* Class member CheckSameAsID */
	int CheckCombination();					/* Class member CheckCombination */
	bool CheckContinuity();					/* Class member CheckContinuity */
	bool CheckRepeatability();				/* Class member CheckRepeatability */
	bool CheckKeyboardContinuity();			/* Class member CheckKeyboardContinuity */
	bool CheckForbiddenWords();				/* Class member CheckForbiddenWords */
};

int get_password_level(const char*, const char*);
int get_backup_password_level(const char*);
int get_id_level(const char*, const char*);
