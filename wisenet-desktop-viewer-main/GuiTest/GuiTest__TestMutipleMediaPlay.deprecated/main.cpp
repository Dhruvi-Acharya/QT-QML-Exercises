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

#include <QApplication>
#include <QMainWindow>
#include <QCommandLineParser>
#include "QCoreServiceManager.h"
#include "DatabaseManager.h"
#include "LogManager.h"
#include "LogSettings.h"
#include "MainWindow.h"
#ifdef WIN32
#include "CudaManager.h"
#endif

void InitService()
{
    Wisenet::Core::DatabaseManager::Delete();
    Wisenet::Core::LogManager::Delete();
    Wisenet::Common::InitializeLogSettings("logs");
    QCoreServiceManager::Instance().Start();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    InitService();

    SPDLOG_INFO("");
    SPDLOG_INFO(">>>>>>>PROGRAM START>>>>>>>>>>::{}", argc);
#ifdef WIN32
    if (!CudaManager::Instance().Start())
        SPDLOG_INFO("NVDEC SETUP FAILED");
#endif
    // command line parser
    // --rgb --view --tile=16
    // --device --tile=16 --ip=192.168.18.198 --pw=5eogus!! --profile=2
    QCommandLineParser parser;
    parser.addOption({"rgb", "use rgb texture"});
    parser.addOption({"view", "use QGraphicsView"});
    parser.addOption({"device", "use device test"});

    QCommandLineOption tileOption("tile", "set tile count", "tile_count", "4");
    parser.addOption(tileOption);

    QCommandLineOption ipOption("ip", "set ip", "ip_addr", "192.168.18.199");
    parser.addOption(ipOption);

    QCommandLineOption pwOption("pw", "set password", "password", "5tkatjd!");
    parser.addOption(pwOption);

    QCommandLineOption profileOption("profile", "set profile index", "profile_index", "2");
    parser.addOption(profileOption);

    parser.process(a);

    int tileCount = 4;
    bool useYuv = !parser.isSet("rgb");
    bool useView = parser.isSet("view");
    bool useDevice = parser.isSet("device");
    bool useTileCount = parser.isSet(tileOption);
    QString ip;
    QString pw;
    int profileIndex = 2;

    if (useTileCount) {
        auto str = parser.value(tileOption);
        SPDLOG_INFO("tile_count={}", str.toStdString());
        tileCount = str.toInt();
    }
    if (useDevice) {
        useView = false;
        ip = parser.value(ipOption);
        pw = parser.value(pwOption);
        auto pf = parser.value(profileOption);
        profileIndex = pf.toInt();
        SPDLOG_INFO("ip={}, pw={}, profile={}", ip.toStdString(), pw.toStdString(), profileIndex);
    }

    SPDLOG_INFO("USE_RGB={}, USE_VIEW={}, USE_DEVICE={}, TILE_COUNT={}", !useYuv, useView, useDevice, tileCount);
    /*
     * QGraphicsView Mode
     * https://doc.qt.io/qt-5/qgraphicsview.html#details
     */
    if (useView) {
        QMainWindow mainWindow;
        ViewingGrid *viewingGrid = new ViewingGrid(nullptr, useYuv, tileCount);

        mainWindow.setCentralWidget(viewingGrid);
        mainWindow.show();

//        if (useDevice) {
//            viewingGrid->useDevice();
//        }
        return a.exec();
    }
    /*
     * QMainWindow and Layout Mode
     */
    else {
        MainWindow mw(nullptr, useYuv, tileCount);
        mw.showMaximized();

        if (useDevice) {
            mw.useDevice(ip, pw, 1, profileIndex);
        }

        return a.exec();
    }

}
