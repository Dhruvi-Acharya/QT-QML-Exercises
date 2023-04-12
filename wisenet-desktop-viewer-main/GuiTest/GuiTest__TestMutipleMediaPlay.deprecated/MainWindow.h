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
#pragma once

#include "VideoWidget.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr, bool useYuv = false, int tileCount = 4);

    void useDevice(QString &ip, QString &pw, int channelIndex, int profileIndex);
signals:

public slots:
    void btnPauseClicked();
    void btnPlayClicked();
    void btnForwardClicked();
    void btnBackwardClicked();

private:
    QVector<VideoWidget *> m_glWidgets;
    QGridLayout *m_layout;

    std::string m_deviceUUID;
    std::string m_mediaUUID;
    float m_speed = 1;
    QPushButton* m_btnPause;
    QPushButton* m_btnPlay;
    QPushButton* m_btnForward;
    QPushButton* m_btnBackward;
};
