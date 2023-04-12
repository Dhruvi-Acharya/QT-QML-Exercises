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
#include <QObject>
#include "QCoreServiceManager.h"
class TranslationSort{
public:
    static TranslationSort* getInstance()
    {
        static TranslationSort instance;
        return &instance;
    }

    explicit TranslationSort(){}
    ~TranslationSort(){}

    template <typename T>
    QStringList sort(T item);
};

template <typename T>
QStringList TranslationSort::sort(T items)
{
    QStringList retList;
    QList<QPair<QString, QString>> tempList;

    for(auto& item : items) {
        tempList.append(qMakePair(QCoreApplication::translate("WisenetLinguist", item.toStdString().c_str()), item));
    }

    std::sort(tempList.begin(), tempList.end());

    for(auto& pair : tempList) {
      retList.append(pair.second);
    }

    return retList;
}
