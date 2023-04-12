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
#include "ViewingGrid.h"
#include "VideoWidget.h"
#include "ProxyWidget.h"
#include <QOpenGLWidget>
#include <QGraphicsProxyWidget>
#include "LogSettings.h"

ViewingGrid::ViewingGrid(QWidget *parent, bool useYuv, int tileCount)
    : QGraphicsView(parent)
{
//    auto gl = new QOpenGLWidget();
//    QSurfaceFormat format;
//    format.setSamples(4);
//    gl->setFormat(format);
//    setViewport(gl);

    const int n = (int)sqrt(tileCount);
    const int windowWidth = 1920;
    const int windowHeight = 1080;
    const int vw = windowWidth / n;
    const int vh = windowHeight / n;
    const int wx = -windowWidth/2;
    const int wy = -windowHeight/2;

    auto scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(wx, wy, windowWidth, windowHeight);

    int mx = wx;
    int my = wy;
    for (int x = 0 ; x < n ; x++) {
        for (int j = 0 ; j < n ; j++) {
            auto proxy = new ProxyWidget;
            //auto proxy = scene->addWidget(videoWidget);

            auto videoWidget = new VideoWidget(proxy, useYuv);
            videoWidget->resize(vw, vh);

            scene->addItem(proxy);
            proxy->setPos(mx, my);

            SPDLOG_DEBUG("x={}, y={}", mx, my);
            mx += vw;
        }
        my += vh;
        mx = wx;
    }

    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(800, 600);
    setWindowTitle(tr("Vieweing Grid"));
    setAcceptDrops(true);
}

void ViewingGrid::useDevice()
{

}

//void ViewingGrid::dragEnterEvent(QDragEnterEvent *event)
//{
//    SPDLOG_INFO("ViewingGrid::dragEnterEvent::{}", event->isAccepted());
//    event->accept();
//}
