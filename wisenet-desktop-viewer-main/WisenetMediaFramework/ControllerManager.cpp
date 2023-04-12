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
#include "ControllerManager.h"
#include <QProcess>

ControllerManager *ControllerManager::Instance()
{
//    qDebug() << "ControllerManager::Instance()";
    static QPointer<ControllerManager> gInstance;
    static QBasicMutex mutex;

    const QMutexLocker lock(&mutex);
    if (gInstance.isNull() && !QCoreApplication::closingDown()) {
        gInstance = new ControllerManager();
    }
    return gInstance;
}

void ControllerManager::Initialize()
{
//    qDebug() << "ControllerManager::Initialize()";
    ControllerManager::Instance();
}

ControllerManager::ControllerManager()
    : m_isRunning(false)
    , m_tasker(nullptr)
    , m_verbose(0)
{
//    qDebug() << "ControllerManager::ControllerManager()";
    StartTask();
}

ControllerManager::~ControllerManager()
{
    StopTask();
}

void ControllerManager::Task()
{
//    qDebug() << "ControllerManager::Task()";
    while (m_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_CONNECT_CONTROLLER));

        if(m_useController)
        {
            connectController();
        }
    }
}

void ControllerManager::StartTask()
{
    qDebug() << "ControllerManager::StartTask()";
    if (!m_tasker && !m_isRunning)
    {
        m_isRunning = true;
        m_tasker = std::make_shared<std::thread>(&ControllerManager::Task, this);
    }
}

void ControllerManager::StopTask()
{
    qDebug() << "ControllerManager::StopTask()";
    if (m_tasker && m_isRunning)
    {
        m_isRunning = false;

        m_tasker->detach();

        if (m_tasker->joinable())
        {
            m_tasker->join();

            m_tasker.reset();
        }
    }
}

void ControllerManager::SetUseController(const bool useController)
{
//    qDebug() << "ControllerManager::SetUseController() useController=" << useController;
    m_useController = useController;
}

void ControllerManager::connectController()
{
//    struct libusb_device_descriptor desc;
    libusb_device **devs;
    ssize_t cnt;
    int i, r;

    r = libusb_init(NULL);
    if (r < 0) {
        qDebug() << "ControllerManager::connectController() - libusb_init - r =" << r;
        return;
    }


    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0) {
        libusb_exit(NULL);
        return;
    }

    for (i = 0; devs[i]; i++) {
        qDebug() << "usb[" << i << "]";

        struct libusb_device_descriptor desc;
        unsigned char productString[256] ={0};

//            print_device(devs[i], NULL); // Desktop USB 포트에 연결된 디바이스 정보 출력

        devh = NULL;

        r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            qDebug() << "ControllerManager::connectController() - failed to get device descriptor r=" << libusb_error_name(r);
            continue;
        }

        if (!devh) {
            r = libusb_open(devs[i], &devh);
            if (r != LIBUSB_SUCCESS) {
                qDebug() << "Cannot open device:" << libusb_error_name(r);
            }
        }

        if (devh) {
            if (desc.iProduct) {
                r = libusb_get_string_descriptor_ascii(devh, desc.iProduct, productString, sizeof(productString));
            }
        }

        // SPC-2000
        if(desc.idVendor == 1678 && (desc.idProduct == 91 || desc.idProduct == 202)) {
            m_connectedControllerType = ControllerType::Spc2000;
#ifdef Q_OS_LINUX
            //리눅스 usb 연결 리셋 : 컨트롤러 인터페이스 접근 권한이 바뀌었으므로 컨트롤러의 인터페이스 모드 리셋
            QProcess::execute("usb_modeswitch", QStringList() << "-v" << "068e" << "-p" << "00ca" << "--reset-usb"); // 테스트 기기
            QProcess::execute("usb_modeswitch", QStringList() << "-v" << "068e" << "-p" << "005b" << "--reset-usb"); // 양산 기기
#endif
        }
        // SPC-2001
        else if(desc.idVendor == 2105 && desc.idProduct == 8193) {
            m_connectedControllerType = ControllerType::Spc2000;
#ifdef Q_OS_LINUX
            //리눅스 usb 연결 리셋 : 컨트롤러 인터페이스 접근 권한이 바뀌었으므로 컨트롤러의 인터페이스 모드 리셋
            QProcess::execute("usb_modeswitch", QStringList() << "-v" << "0839" << "-p" << "2001" << "--reset-usb");
#endif
        }
        else {
            // SPC-7000, ...
            continue;
        }

        r = findDpfpDevice(desc.idVendor, desc.idProduct);
        if (r < 0) {
            qDebug() << "ControllerManager::connectController() - findDpfpDevice - r =" << libusb_error_name(r);
            continue;
        }

        r = libusb_set_auto_detach_kernel_driver(devh, 1);
        if (r != LIBUSB_SUCCESS) {
            qDebug() << "Cannot set auto-detach kernel driver:" << libusb_error_name(r);
        }

        r = libusb_claim_interface(devh, 0);
        if (r < 0) {
            qDebug() << "ControllerManager::connectController() - libusb_claim_interface - r =" << libusb_error_name(r);
        }

        // 설정 창에 컨트롤러 연결됨 토스트 메시지
        emit controllerConnectStatusChanged();

        int isProcessData = doProcess;
        while(m_useController) {
            r = readControllerData(); // 컨트롤러로부터 데이터 읽음
            if (r < 0) {
                qDebug() << "ControllerManager::connectController() - readControllerData - r =" << libusb_error_name(r);
                break;
            }

            if(isProcessData == doProcess) {
                processControllerData(); // 읽어온 데이터 처리하여 VideoItem에 시그널 발생
                isProcessData = 0;
            }
            isProcessData += 1;
        }
    }

    libusb_free_device_list(devs, 1);
}

int ControllerManager::findDpfpDevice(uint16_t idVendor, uint16_t idProduct)
{
    qDebug() << "ControllerManager::findDpfpDevice() - idVendor:"<<idVendor<<"idProduct:"<<idProduct;
    devh = libusb_open_device_with_vid_pid(NULL, idVendor, idProduct);
    if (!devh) {
        errno = ENODEV;
        return -1;
    }
    return 0;
}

int ControllerManager::readControllerData()
{
    int r;
    int actual_length = 0;

    memset(m_currentControllerReceivedData, 0, sizeof(m_currentControllerReceivedData));

    r = libusb_interrupt_transfer(devh, INTERRUPT_ENDPOINT_IN, m_currentControllerReceivedData, sizeof(m_currentControllerReceivedData), &actual_length, INTERVAL_GET_CONTROLLER_DATA);

    if (r < 0) {
        qDebug() << "ControllerManager::readControllerData() - F0 error :" << r << "actual_length="<<actual_length;
        return r;
    }

//    SPDLOG_DEBUG("---- Data from controller ----\n");
//    SPDLOG_DEBUG("data[0~3]: {} {} {} {}\n", m_currentControllerReceivedData[0], m_currentControllerReceivedData[1], m_currentControllerReceivedData[2], m_currentControllerReceivedData[3]);
//    SPDLOG_DEBUG("data[4~7]: {} {} {} {}\n", m_currentControllerReceivedData[4], m_currentControllerReceivedData[5], m_currentControllerReceivedData[6], m_currentControllerReceivedData[7]);

    return 0;
}

void ControllerManager::processControllerData()
{
    switch(m_connectedControllerType)
    {
    case ControllerType::Spc2000:
        m_controllerSpc2000.processControllerData(m_currentControllerReceivedData);

        if(m_controllerSpc2000.m_isPtzChangedEmit) {
//            qDebug() << "ControllerManager::processControllerData() - emit ptzChanged()";
            emit ptzChanged(m_controllerSpc2000.m_controllerMode, m_controllerSpc2000.m_panDirectionSpeed, m_controllerSpc2000.m_tiltDirectionSpeed, m_controllerSpc2000.m_zoomDirectionSpeed);
        }
        if(m_controllerSpc2000.m_isButtonChangedEmit) {
//            qDebug() << "ControllerManager::processControllerData() - emit buttonChanged()";
            emit buttonChanged(m_controllerSpc2000.m_controllerMode, m_controllerSpc2000.m_pressedButtonAction);
        }
        break;
    case ControllerType::Spc7000:
        // for SPC-7000 feature
        break;
    }
}

void ControllerManager::print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
    printf("      USB 3.0 Endpoint Companion:\n");
    printf("        bMaxBurst:           %u\n", ep_comp->bMaxBurst);
    printf("        bmAttributes:        %02xh\n", ep_comp->bmAttributes);
    printf("        wBytesPerInterval:   %u\n", ep_comp->wBytesPerInterval);
}

void ControllerManager::print_endpoint(const struct libusb_endpoint_descriptor *endpoint)
{
    int i, ret;

    printf("      Endpoint:\n");
    printf("        bEndpointAddress:    %02xh\n", endpoint->bEndpointAddress);
    printf("        bmAttributes:        %02xh\n", endpoint->bmAttributes);
    printf("        wMaxPacketSize:      %u\n", endpoint->wMaxPacketSize);
    printf("        bInterval:           %u\n", endpoint->bInterval);
    printf("        bRefresh:            %u\n", endpoint->bRefresh);
    printf("        bSynchAddress:       %u\n", endpoint->bSynchAddress);

    for (i = 0; i < endpoint->extra_length;) {
        if (LIBUSB_DT_SS_ENDPOINT_COMPANION == endpoint->extra[i + 1]) {
            struct libusb_ss_endpoint_companion_descriptor *ep_comp;

            ret = libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
            if (LIBUSB_SUCCESS != ret)
                continue;

            print_endpoint_comp(ep_comp);

            libusb_free_ss_endpoint_companion_descriptor(ep_comp);
        }

        i += endpoint->extra[i];
    }
}

void ControllerManager::print_altsetting(const struct libusb_interface_descriptor *interface)
{
    uint8_t i;

    printf("    Interface:\n");
    printf("      bInterfaceNumber:      %u\n", interface->bInterfaceNumber);
    printf("      bAlternateSetting:     %u\n", interface->bAlternateSetting);
    printf("      bNumEndpoints:         %u\n", interface->bNumEndpoints);
    printf("      bInterfaceClass:       %u\n", interface->bInterfaceClass);
    printf("      bInterfaceSubClass:    %u\n", interface->bInterfaceSubClass);
    printf("      bInterfaceProtocol:    %u\n", interface->bInterfaceProtocol);
    printf("      iInterface:            %u\n", interface->iInterface);

    for (i = 0; i < interface->bNumEndpoints; i++)
        print_endpoint(&interface->endpoint[i]);
}

void ControllerManager::print_interface(const struct libusb_interface *interface)
{
    int i;

    for (i = 0; i < interface->num_altsetting; i++)
        print_altsetting(&interface->altsetting[i]);
}

void ControllerManager::print_configuration(struct libusb_config_descriptor *config)
{
    uint8_t i;

    printf("  Configuration:\n");
    printf("    wTotalLength:            %u\n", config->wTotalLength);
    printf("    bNumInterfaces:          %u\n", config->bNumInterfaces);
    printf("    bConfigurationValue:     %u\n", config->bConfigurationValue);
    printf("    iConfiguration:          %u\n", config->iConfiguration);
    printf("    bmAttributes:            %02xh\n", config->bmAttributes);
    printf("    MaxPower:                %u\n", config->MaxPower);

    for (i = 0; i < config->bNumInterfaces; i++)
        print_interface(&config->interface[i]);
}

void ControllerManager::print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap)
{
    printf("    USB 2.0 Extension Capabilities:\n");
    printf("      bDevCapabilityType:    %u\n", usb_2_0_ext_cap->bDevCapabilityType);
    printf("      bmAttributes:          %08xh\n", usb_2_0_ext_cap->bmAttributes);
}

void ControllerManager::print_ss_usb_cap(struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap)
{
    printf("    USB 3.0 Capabilities:\n");
    printf("      bDevCapabilityType:    %u\n", ss_usb_cap->bDevCapabilityType);
    printf("      bmAttributes:          %02xh\n", ss_usb_cap->bmAttributes);
    printf("      wSpeedSupported:       %u\n", ss_usb_cap->wSpeedSupported);
    printf("      bFunctionalitySupport: %u\n", ss_usb_cap->bFunctionalitySupport);
    printf("      bU1devExitLat:         %u\n", ss_usb_cap->bU1DevExitLat);
    printf("      bU2devExitLat:         %u\n", ss_usb_cap->bU2DevExitLat);
}

void ControllerManager::print_bos(libusb_device_handle *handle)
{
    struct libusb_bos_descriptor *bos;
    uint8_t i;
    int ret;

    ret = libusb_get_bos_descriptor(handle, &bos);
    if (ret < 0)
        return;

    printf("  Binary Object Store (BOS):\n");
    printf("    wTotalLength:            %u\n", bos->wTotalLength);
    printf("    bNumDeviceCaps:          %u\n", bos->bNumDeviceCaps);

    for (i = 0; i < bos->bNumDeviceCaps; i++) {
        struct libusb_bos_dev_capability_descriptor *dev_cap = bos->dev_capability[i];

        if (dev_cap->bDevCapabilityType == LIBUSB_BT_USB_2_0_EXTENSION) {
            struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension;

            ret = libusb_get_usb_2_0_extension_descriptor(NULL, dev_cap, &usb_2_0_extension);
            if (ret < 0)
                return;

            print_2_0_ext_cap(usb_2_0_extension);
            libusb_free_usb_2_0_extension_descriptor(usb_2_0_extension);
        } else if (dev_cap->bDevCapabilityType == LIBUSB_BT_SS_USB_DEVICE_CAPABILITY) {
            struct libusb_ss_usb_device_capability_descriptor *ss_dev_cap;

            ret = libusb_get_ss_usb_device_capability_descriptor(NULL, dev_cap, &ss_dev_cap);
            if (ret < 0)
                return;

            print_ss_usb_cap(ss_dev_cap);
            libusb_free_ss_usb_device_capability_descriptor(ss_dev_cap);
        }
    }

    libusb_free_bos_descriptor(bos);
}

void ControllerManager::print_device(libusb_device *dev, libusb_device_handle *handle)
{
    struct libusb_device_descriptor desc;
    unsigned char data[256];
    const char *speed;
//    libusb_device **devs;
//    ssize_t cnt;
    int ret;
    uint8_t i;

    switch (libusb_get_device_speed(dev)) {
    case LIBUSB_SPEED_LOW:		speed = "1.5M"; break;
    case LIBUSB_SPEED_FULL:		speed = "12M"; break;
    case LIBUSB_SPEED_HIGH:		speed = "480M"; break;
    case LIBUSB_SPEED_SUPER:	speed = "5G"; break;
    case LIBUSB_SPEED_SUPER_PLUS:	speed = "10G"; break;
    default:			speed = "Unknown";
    }

    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
        qDebug() << "failed to get device descriptor";
        return;
    }

    qDebug() << "ControllerManager::print_device() - Dev (bus"<<libusb_get_bus_number(dev)<<", device" << libusb_get_device_address(dev) << "): idVendor" << desc.idVendor << "- idProduct" << desc.idProduct << " speed:" << QString::fromUtf8(speed);

    if (!handle)
        libusb_open(dev, &handle);

    if (handle) {
        if (desc.iManufacturer) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, data, sizeof(data));
            if (ret > 0) {
                qDebug() << "ControllerManager::print_device() - Manufacturer:" << QString::fromUtf8((char*)data);
            }
        }

        if (desc.iProduct) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, data, sizeof(data));
            if (ret > 0) {
                qDebug() << "ControllerManager::print_device() - Product:" << QString::fromUtf8((char*)data);
            }
        }

        if (desc.iSerialNumber && m_verbose) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, data, sizeof(data));
            if (ret > 0) {
                qDebug() << "ControllerManager::print_device() - Serial Number:" << QString::fromUtf8((char*)data);
            }
        }
    }

    if (m_verbose) {
        for (i = 0; i < desc.bNumConfigurations; i++) {
            struct libusb_config_descriptor *config;

            ret = libusb_get_config_descriptor(dev, i, &config);
            if (LIBUSB_SUCCESS != ret) {
                qDebug() << "Couldn't retrieve descriptors";
                continue;
            }

            print_configuration(config);

            libusb_free_config_descriptor(config);
        }

        if (handle && desc.bcdUSB >= 0x0201)
            print_bos(handle);
    }

    if (handle) {
        libusb_close(handle);
    }
}
