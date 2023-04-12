#include <iostream>
#include <algorithm>
#include <string>
#include "PasswordStrengthCheckerS1.h"

const static std::vector<std::string> keyboardKeys = {
    "`1234567890-=", "qweryuiop[]", "asdfghjkl;'", "zxcvbnm,./", "~!@#$%^&*()_+", "QWERTYUIOP{}|", "ASDFGHJKL:",  "ZXCVBNM<>?",
    "=-0987654321`", "][poiuytrewq", "';lkjhgfdsa", "/.,mnbvcxz", "+_)(*&^%$#@!~", "|}{POIUYTREWQ", ":LKJHGFDSA",  "?><MNBVCXZ"
};

const static std::vector<std::string> words = {
     "SECOM", "secom", "S1", "s1"
};

PasswordStrengthCheckerS1* PasswordStrengthCheckerS1::mpInstance = nullptr;

PasswordStrengthCheckerS1::PasswordStrengthCheckerS1()
    :strLength(0), checkCompareCount(0), strID("admin"), strPasswd("")
{
    forbiddenWords.assign(words.begin(), words.end());
    keyboardWords.assign(keyboardKeys.begin(), keyboardKeys.end());
}

PasswordStrengthCheckerS1::~PasswordStrengthCheckerS1()
{
    if (nullptr != mpInstance)
    {
        delete mpInstance;
    }
}

PasswordStrengthCheckerS1* PasswordStrengthCheckerS1::getInstance()
{
    if (nullptr == mpInstance)
	{
        mpInstance = new PasswordStrengthCheckerS1();
	}

	return mpInstance;
}

bool PasswordStrengthCheckerS1::isDigit(const char passwdChar)
{
	if((passwdChar >= '0') && (passwdChar <= '9'))
	{
		return true;
	}

	return false;
}

bool PasswordStrengthCheckerS1::isAlphaUpperCase(const char passwdChar)
{
	if(passwdChar >= 'A' && passwdChar <= 'Z')
	{
		return true;
	}

	return false;
}

bool PasswordStrengthCheckerS1::isAlphaLowerCase(const char passwdChar)
{
	if(passwdChar >= 'a' && passwdChar <= 'z')
	{
		return true;
	}

	return false;
}	

bool PasswordStrengthCheckerS1::isSpecial(const char passwdChar)
{
	bool retVal = false;
    std::string specialWord("`-=[];\\',./~!@#$%^&*()_+{}|:\"<>?");

    if(specialWord.find(passwdChar) != std::string::npos)
	{
		retVal = true;
	}

	if(isspace(passwdChar))
	{
		retVal = true;
	}

	return retVal;
}

bool PasswordStrengthCheckerS1::CompareWords(const char compareStr1, const char compareStr2)
{
	if(compareStr1 == compareStr2)
	{
		checkCompareCount++;
		if(checkCompareCount >= (MAX_CHECK_STRING_COUNT-1))
		{
			return false;
		}
	}
	else
	{
		checkCompareCount = 0;
	}

	return true;
}

bool PasswordStrengthCheckerS1::CheckKeyboardWords(const std::string strPasswd)
{
    std::string words;

    for (std::vector<std::string>::iterator itPos = keyboardWords.begin(); itPos != keyboardWords.end(); ++itPos)
	{
		words.assign(*itPos);
        if(words.find(strPasswd) != std::string::npos)
		{
			return false;
		}
	}

	return true;
}

bool PasswordStrengthCheckerS1::CheckSameAsID()
{
	if(strPasswd.compare(strID) == 0)
	{
		return false;
	}

	return true;
}

int PasswordStrengthCheckerS1::CheckCombination()
{
	int retVal = COMBINATION_CHECK_OK;
	int isDigitChar = 0;
	int isUpperCaseChar = 0;
	int isLowerCaseChar = 0;
	int isSpecialChar = 0;
	char passWdChar;

    for(int i=0; i<strLength; i++)
    {
        passWdChar = strPasswd[i];

        if(isDigit(passWdChar))			/* Number */
        {
            isDigitChar = 1;
        }
        else if(isAlphaUpperCase(passWdChar))	/* Upper Case Alphabet */
        {
            isUpperCaseChar = 1;
        }
        else if(isAlphaLowerCase(passWdChar))	/* Lower Case Alphabet */
        {
            isLowerCaseChar = 1;
        }
        else if(isSpecial(passWdChar))	/* Special Character */
        {
        	isSpecialChar = 1;
        }
    }

	int checkCount = isDigitChar + isUpperCaseChar + isLowerCaseChar + isSpecialChar;

    if((strLength >= PASSWD_MIN_LENGTH) && (strLength < LENGTH_CHECK_RANGE_1))
    {
    	if(checkCount < RANGE1_COMBINATION_COUNT)
    	{
    		retVal = COMBINATION_CHECK_ERROR_RANGE1;
    	}
    }
    else if((strLength >= LENGTH_CHECK_RANGE_1) && (strLength <= PASSWD_MAX_LENGTH))
    {
		if(checkCount < RANGE2_COMBINATION_COUNT)
		{
			retVal = COMBINATION_CHECK_ERROR_RANGE2;
		}
    }

    return retVal;
}

bool PasswordStrengthCheckerS1::CheckContinuity()
{
	int checkStrCount = strLength - MAX_CHECK_STRING_COUNT;
	char passwdChar[2] = {0, };

	for(int i=0; i<=checkStrCount; i++)
	{
		checkCompareCount = 0;
		for(int j=0; j<(MAX_CHECK_STRING_COUNT-1); j++)
		{
			passwdChar[0] = strPasswd[i+j] + 1;	/* Increase character of first character */
			passwdChar[1] = strPasswd[i+j+1];	/* Second character */

			if(!CompareWords(passwdChar[0], passwdChar[1]))
			{
				return false;
			}
		}
	}

	for(int i=0; i<=checkStrCount; i++)
	{
		checkCompareCount = 0;
		for(int j=0; j<(MAX_CHECK_STRING_COUNT-1); j++)
		{
			passwdChar[0] = strPasswd[i+j] - 1;	/* Decrease character of first character */
			passwdChar[1] = strPasswd[i+j+1];	/* Second character */

			if(!CompareWords(passwdChar[0], passwdChar[1]))
			{
				return false;
			}
		}
	}

	return true;
}

bool PasswordStrengthCheckerS1::CheckRepeatability()
{
	int checkStrCount = strLength - MAX_CHECK_STRING_COUNT;
	char passwdChar[2] = {0, };

	for(int i=0; i<=checkStrCount; i++)
	{
		checkCompareCount = 0;
		for(int j=0; j<(MAX_CHECK_STRING_COUNT-1); j++)
		{
			passwdChar[0] = strPasswd[i+j];		/* First Character */
			passwdChar[1] = strPasswd[i+j+1];	/* Second Charcter */

			if(!CompareWords(passwdChar[0], passwdChar[1]))
			{
				return false;
			}
		}
	}

	return true;
}

bool PasswordStrengthCheckerS1::CheckKeyboardContinuity()
{
	int checkStrCount = strLength - MAX_CHECK_STRING_COUNT;
    std::string subStrPasswd;

	for(int i=0; i<=checkStrCount; i++)
	{
		subStrPasswd = strPasswd.substr(i, MAX_CHECK_STRING_COUNT);

		if(!CheckKeyboardWords(subStrPasswd))
		{
			return false;
		}

		reverse(subStrPasswd.begin(), subStrPasswd.end());

		if(!CheckKeyboardWords(subStrPasswd))
		{
			return false;
		}
	}

	return true;
}

bool PasswordStrengthCheckerS1::CheckForbiddenWords()
{
    for(std::vector<std::string>::iterator itPos = forbiddenWords.begin(); itPos != forbiddenWords.end(); ++itPos)
	{
        if(strPasswd.find(*itPos) != std::string::npos)
        {
            return false;
		}
	}

	return true;
}

unsigned int PasswordStrengthCheckerS1::GetPasswordLevel(const char* id, const char* passwd)
{
    unsigned int retVal = PasswordStrength::Success;

	strID.assign(id);
	strPasswd.assign(passwd);
    strLength = strPasswd.length();

    if (strLength < PASSWD_MIN_LENGTH)
	{
        retVal |= PasswordStrength::LengthUnderEight;
		return retVal;
	}

    if (strLength > PASSWD_MAX_LENGTH)
    {
        retVal |= PasswordStrength::LengthUnderTenAndNoSymbols;
        return retVal;
    }

    if (!CheckSameAsID())
    {
        retVal |= PasswordStrength::RepeatedValue;
	}

	int checkStatus = CheckCombination();
    if (checkStatus == COMBINATION_CHECK_ERROR_RANGE1)
	{
        retVal |= PasswordStrength::RepeatedValue;
	}
    else if (checkStatus == COMBINATION_CHECK_ERROR_RANGE2)
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

    if (!CheckContinuity())
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

    if (!CheckRepeatability())
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

	// Except Keyboard Continuity from S1
    if (!CheckKeyboardContinuity())
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

    if (!CheckForbiddenWords())
	{
		//cerr << "[Error] CheckForbiddenWords()" << endl;
        retVal |= PasswordStrength::RepeatedValue;
	}

	return retVal;
}

unsigned int PasswordStrengthCheckerS1::GetIdLevel(const char* id)
{
	int isDigitChar = 0;
	int isAlphaChar = 0;
	char idChar = 0;
	strID.assign(id);
	
	for(int i=0; i< (int)strID.length(); i++)
	{
		idChar = strID[i];
		if(isDigit(idChar))			/* Number */
		{
			isDigitChar = 1;
		}
        else if(isAlphaUpperCase(idChar) || isAlphaLowerCase(idChar))	/* Alphabet */
        {
             isAlphaChar = 1;
        }
	}

	/* If ID only use number */
	if(isDigitChar && (isAlphaChar == 0))
	{
		return 1;
	}
	
	return 0;	
}

int get_password_level(const char* id, const char* password)
{
    return PasswordStrengthCheckerS1::getInstance()->GetPasswordLevel(id, password);
}

int get_id_level(const char* id)
{
    return PasswordStrengthCheckerS1::getInstance()->GetIdLevel(id);
}

unsigned int PasswordStrengthCheckerS1::GetPasswordLevel(const char* passwd)
{
    unsigned int retVal = PasswordStrength::Success;

	strPasswd.assign(passwd);
	strLength = strPasswd.length();

    if (strLength < PASSWD_MIN_LENGTH)
	{
        retVal |= PasswordStrength::LengthUnderEight;
		return retVal;
	}

    if (strLength > PASSWD_MAX_LENGTH)
    {
        retVal |= PasswordStrength::LengthUnderTenAndNoSymbols;
        return retVal;
    }

	int checkStatus = CheckCombination();
	if(checkStatus == COMBINATION_CHECK_ERROR_RANGE1)
	{
        retVal |= PasswordStrength::RepeatedValue;
	}
	else if(checkStatus == COMBINATION_CHECK_ERROR_RANGE2)
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

	if(!CheckContinuity())
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

	if(!CheckRepeatability())
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

	if(!CheckKeyboardContinuity())
	{
        retVal |= PasswordStrength::RepeatedValue;
	}

    if (!CheckForbiddenWords())
    {
        //cerr << "[Error] CheckForbiddenWords()" << endl;
        retVal |= PasswordStrength::RepeatedValue;
    }

	return retVal;
}

int get_backup_password_level(const char* password)
{
    return PasswordStrengthCheckerS1::getInstance()->GetPasswordLevel(password);
}

