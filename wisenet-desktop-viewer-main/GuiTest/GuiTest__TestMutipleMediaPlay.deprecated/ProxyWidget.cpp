/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
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
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "ProxyWidget.h"
#include "LogSettings.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QDir>
#include <QDebug>

ProxyWidget::ProxyWidget()
    //: QGraphicsProxyWidget(nullptr, Qt::Window)
    : QGraphicsProxyWidget(nullptr, Qt::Widget)
{
}

//void ProxyWidget::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
//{
//    qDebug() << "ProxyWidget::dragEnterEvent::" << event->dropAction();
//    event->acceptProposedAction();
//}

//void ProxyWidget::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
//{
//    event->accept();
//}

//void ProxyWidget::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
//{
//    SPDLOG_DEBUG("dragLeaveEvent");
//    Q_UNUSED(event);
//    //QGraphicsProxyWidget::dragLeaveEvent(event);
//}

//void ProxyWidget::dropEvent(QGraphicsSceneDragDropEvent *event)
//{
//    qDebug() << "ProxyWidget::dropEvent::" << event->dropAction();
//    if (event->mimeData()->hasUrls()) {
//        auto url = event->mimeData()->urls()[0].toLocalFile();
//        emit openMediaFile(url);
//    }

//}
