#include "PasswordStrengthChecker.h"
#include "PasswordStrengthCheckerS1.h"

PasswordStrengthChecker::PasswordStrengthChecker(QObject *parent) : QObject(parent)
{
    qDebug() << "PasswordChecker()";
}

PasswordStrengthChecker::~PasswordStrengthChecker()
{
    qDebug() << "~PasswordChecker()";
}

int PasswordStrengthChecker::test(QString password)
{
#ifdef WISENET_S1_VERSION
    // S1 Viewer
    int result = PasswordStrengthCheckerS1::getInstance()->GetPasswordLevel(password.toStdString().c_str());

    if (result != PasswordStrength::Success)
    {
        return result;
    }
#else
    // Wisenet Viewer
    if(password.isNull() || password.isEmpty())
        return (int)PasswordStrength::Blank;

    if (password.length() < MinimumLength)
    {
        return (int)PasswordStrength::LengthUnderEight;
    }

    bool isContainUpper = password.contains(QRegularExpression(upperLetters));
    bool isContainLower = password.contains(QRegularExpression(lowerLetters));
    bool isContainNumber = password.contains(QRegularExpression(numbers));

    bool isContainSymbol = false;
    foreach(auto& symbol, symbols)
    {
        if(password.contains(symbol))
        {
            isContainSymbol = true;
            break;
        }
    }

    int numberOfSatisfied = 0;

    if (isContainUpper)
        numberOfSatisfied++;

    if(isContainLower)
        numberOfSatisfied++;

    if (isContainNumber)
        numberOfSatisfied++;

    if (isContainSymbol)
        numberOfSatisfied++;

    //qDebug() << "isContainUpper:" << isContainUpper;
    //qDebug() << "isContainLower:" << isContainLower;
    //qDebug() << "isContainNumber:" << isContainNumber;
    //qDebug() << "isContainSymbol:" << isContainSymbol;

    if (password.length() >= MinimumLengthWithoutSymbols && numberOfSatisfied < 2)
    {
        return (int)PasswordStrength::NoNumbers;
    }
    //조합문자 4종 3가지 이상 조합
    else if(password.length() < MinimumLengthWithoutSymbols && numberOfSatisfied < 3)
    {
        return (int)PasswordStrength::LengthUnderTenAndNoSymbols;
    }

    // Check repeat
    if (isRepeatedValue(password, RepeatLimitation))
    {
        return  (int)PasswordStrength::RepeatedValue;
    }

    // Check consecutive of character
    if (isConsecutiveValue(password, ConsecutiveLimitation))
    {
        return (int)PasswordStrength::ConsecutiveValue;
    }
#endif

    return (int)PasswordStrength::Success;
}

bool PasswordStrengthChecker::exportPasswordTest(QString password)
{
    if(password.isNull() || password.isEmpty() || password.length() < MinimumLength)
        return false;

    return password.contains(QRegularExpression(letters)) && password.contains(QRegularExpression(numbers));
}


bool PasswordStrengthChecker::isRepeatedValue(QString input, int count)
{
    int i;
    int j;
    int inputLength = 0;

    if (input.isNull() || input.isEmpty())
    {
        return false;
    }

    inputLength = input.length();

    if (inputLength < count)
    {
        return false;
    }

    for (i = 0; i <= inputLength - count; i++)
    {
        bool isRepeated = true;

        for (j = i; j < i + count - 1; j++)
        {
            if (input.at(j) != input.at(j+1))
            {
                isRepeated = false;
            }
        }

        if (isRepeated)
        {
            return isRepeated;
        }
    }

    return false;
}

bool PasswordStrengthChecker::isConsecutiveValue(QString input, int count)
{
    int i;
    int j;
    int inputLength = 0;

    // null 값 체크
    if (input.isNull() || input.isEmpty())
    {
        return false;
    }

    // Input Lenghth
    inputLength = input.length();

    // 이 조건으로 빠지지는 않음
    if (inputLength < count)
    {
        return false;
    }

    // 정순 체크
    for (i = 0; i <= inputLength - count; i++)
    {
        bool isConsecutive = true;

        for (j = i; j < i + count - 1; j++)
        {
            if (symbols.contains(input[j]))
            {
                isConsecutive = false;
                continue;
            }

            if ((input.at(j).unicode()+1) != input.at(j+1).unicode())
            {
                isConsecutive = false;
            }
        }

        if (isConsecutive)
        {
            return isConsecutive;
        }
    }

    count = count - 1;

    // 역순 체크
    for (i = inputLength - 1; i >= count; i--)
    {
        bool isConsecutive = true;

        for (j = i; j >= i - count + 1; j--)
        {
            if (symbols.contains(input[j]))
            {
                isConsecutive = false;
                continue;
            }
            if ((input.at(j).unicode() + 1) != input.at(j-1).unicode())
            {
                isConsecutive = false;
            }
        }

        if (isConsecutive)
        {
            return isConsecutive;
        }
    }

    return false;
}

