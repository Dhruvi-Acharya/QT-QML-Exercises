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

#include <QCoreApplication>
#include <QPointer>
#include <QBasicMutex>
#include <QDebug>
#include <thread>
#include <chrono>
#include "ControllerSpc2000.h"
#include "libusb-1.0/libusb.h"
#include "LogSettings.h"

/*
 * These are the requests (bRequest) that the bootstrap loader is expected
 * to recognize.  The codes are reserved by Cypress, and these values match
 * what EZ-USB hardware, or "Vend_Ax" firmware (2nd stage loader) uses.
 * Cypress' "a3load" is nice because it supports both FX and FX2, although
 * it doesn't have the EEPROM support (subset of "Vend_Ax").
 */
#define INTERRUPT_ENDPOINT_OUT 0x01
#define INTERRUPT_ENDPOINT_IN 0x81

#define INTERVAL_GET_CONTROLLER_DATA 0 // 데이터 입력 체크 주기
#define INTERVAL_CONNECT_CONTROLLER 5000 // 데이터 연결 체크 주기 : 5s

class ControllerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ptz READ ptz NOTIFY ptzChanged) // controllerManager -> videoItem
    Q_PROPERTY(bool button READ button NOTIFY buttonChanged) // controllerManager -> viewingGrid
    Q_PROPERTY(bool controllerConnectStatus READ controllerConnectStatus NOTIFY controllerConnectStatusChanged) // controllerManager -> systemLocalSettingView
    Q_PROPERTY(ControllerModel::ControllerMode spc2000ControllerMode READ spc2000ControllerMode WRITE setSpc2000ControllerMode)

signals:
    void ptzChanged(ControllerModel::ControllerMode controllerMode, int panValue, int tiltValue, int zoomValue);
    void buttonChanged(ControllerModel::ControllerMode controllerMode, ControllerSpc2000::ControllerButtonAction controllerButtonAction);
    void controllerConnectStatusChanged();

public:
    enum class ControllerType{
        None = 0,
        Spc2000,
        Spc7000,
    };

    static ControllerManager* Instance();
    static void Initialize();

public:
    ControllerManager();
    ~ControllerManager();

    // thread
    void StartTask();
    void StopTask();

    void SetUseController(const bool useController);

    // ptz/button/controllerConnectStatus => 현 구조에서는 return true
    // => View단에서 수정하는 경우에는 return val(동작 시나리오에 필요한 연산) 형태로 수정
    bool ptz() { return true; }
    bool button() { return true; }
    bool controllerConnectStatus() { return true; }
    ControllerModel::ControllerMode spc2000ControllerMode() { return m_controllerSpc2000.m_controllerMode; }
    void setSpc2000ControllerMode(ControllerModel::ControllerMode controllerMode) { m_controllerSpc2000.m_controllerMode = controllerMode; }

private:
    void Task();
    void connectController();
    int findDpfpDevice(uint16_t idVendor, uint16_t idProduct);
    int readControllerData();
    void processControllerData();

    // USB 디바이스 디버깅 전용 함수
    void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp);
    void print_endpoint(const struct libusb_endpoint_descriptor *endpoint);
    void print_altsetting(const struct libusb_interface_descriptor *interface);
    void print_interface(const struct libusb_interface *interface);
    void print_configuration(struct libusb_config_descriptor *config);
    void print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap);
    void print_ss_usb_cap(struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap);
    void print_bos(libusb_device_handle *handle);
    void print_device(libusb_device *dev, libusb_device_handle *handle);

private:
    std::shared_ptr<std::thread> m_tasker;

#ifdef Q_OS_WINDOWS
    const int doProcess = 15;
#endif
#ifdef Q_OS_MACOS
    const int doProcess = 15;
#endif
#ifdef Q_OS_LINUX
    const int doProcess = 5;
#endif

    bool m_isRunning;
    bool m_useController = false;
    int m_verbose = 0;
    libusb_device_handle *devh = NULL;
    ControllerType m_connectedControllerType = ControllerType::None;
    ControllerSpc2000 m_controllerSpc2000;

    /*
     * m_controllerReceivedData : int형 8개 구성
     * [0] : x축 value (0~255)
     * [1] : x축 level (0~3)
     * [2] : y축 value (0~255)
     * [3] : y축 level (0~3)
     * [4] : zoom value (0~255)
     * [5] : zoom level (0~3)
     * [6] : button input (0,1,2,4,8,16,32,64,128)
     * [7] : button input (0,1,2,4,8)
     */
    unsigned char m_currentControllerReceivedData[32];
};

