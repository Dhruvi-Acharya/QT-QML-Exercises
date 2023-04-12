/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
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

#include "ControllerModel.h"


class ControllerModel;

class ControllerSpc2000 : public ControllerModel
{
    Q_OBJECT
public:
    enum ControllerButtonInput{
        None = 0,
        // 모드 변경 동작
        PtzMode = 1, // 9번째 버튼
        ScreenMode = 2, // 10번째 버튼
        // PTZ모드일 때 버튼 동작
        Preset1 = 1,
        Preset2 = 2,
        Preset3 = 4,
        Preset4 = 8,
        Preset5 = 16,
        Preset6 = 32,
        Preset7 = 64,
        Preset8 = 128,
        SingleChannelMode = 4, // 11번째 버튼
        MultiChannelMode = 8, // 12번째 버튼
        // Screen모드일 때 버튼 동작
        LayoutTab1 = 1,
        LayoutTab2 = 2,
        LayoutTab3 = 4,
        LayoutTab4 = 8,
        LayoutTab5 = 16,
        LayoutTab6 = 32,
        LayoutTab7 = 64,
        LayoutTab8 = 128,
        LeftWindow = 4, // 11번째 버튼
        RightWindow = 8, // 12번째 버튼
    };

    enum class ControllerButtonAction{
        None = 0,
        // 모드 변경 동작
        SetPtzMode, // 9번째 버튼
        SetScreenMode, // 10번째 버튼
        // PTZ모드일 때 버튼 동작
        DoPreset1,
        DoPreset2,
        DoPreset3,
        DoPreset4,
        DoPreset5,
        DoPreset6,
        DoPreset7,
        DoPreset8,
        SwitchSingleChannelMode, // 11번째 버튼
        SwitchMultiChannelMode, // 12번째 버튼
        // Screen모드일 때 버튼 동작
        MoveLayoutTab1,
        MoveLayoutTab2,
        MoveLayoutTab3,
        MoveLayoutTab4,
        MoveLayoutTab5,
        MoveLayoutTab6,
        MoveLayoutTab7,
        MoveLayoutTab8,
        MoveLeftWindow, // 11번째 버튼
        MoveRightWindow, // 12번째 버튼
    };
    Q_ENUM(ControllerButtonAction)

    ControllerSpc2000();
    ~ControllerSpc2000();
    /*
     * m_controllerReceivedData : int형 8개 구성
     * [0] : pan value (0~255)
     * [1] : pan level (0~3)
     * [2] : tilt value (0~255)
     * [3] : tilt level (0~3)
     * [4] : zoom value (0~255)
     * [5] : zoom level (0~3)
     * [6] : button input (0(의미x), 1(1), 2(2), 4(3), 8(4), 16(5), 32(6), 64(7), 128(8))
     * [7] : button input (0(의미x), 1(9), 2(10), 4(11), 8(12번째 버튼))
     */
    void processControllerData(unsigned char *controllerReceivedData);
    void processPtz(unsigned char *controllerReceivedData);
    void processButton(unsigned char *controllerReceivedData);

    bool m_isPtzChangedEmit = false;
    bool m_isButtonChangedEmit = false;

    int m_panDirectionSpeed = 0;
    int m_tiltDirectionSpeed = 0;
    int m_zoomDirectionSpeed = 0;

    ControllerButtonAction m_pressedButtonAction = ControllerButtonAction::None;

    ControllerModel::ControllerMode m_controllerMode = ControllerModel::ControllerMode::ScreenMode;

private:
    const int m_minPan = 5;
    const int m_minTilt = 5;
    const int m_minZoom = 5;
    const int m_zoomValue = 255;
    const int m_zoomLevel = 1;
    const int m_panValue = 255;
    const int m_panLevel = 1;
    const int m_tiltValue = 255;
    const int m_tiltLevel = 1;

    const int m_panTiltSpeedArray[4] = {20, 10, 10, 20}; // Level 0~3에 따라 speed 조절
    const int m_zoomSpeedArray[4] = {80, 40, 40, 80}; // Level 0~3에 따라 speed 조절

    int m_prevPanDirectionSpeed = 0;
    int m_prevTiltDirectionSpeed = 0;
    int m_prevZoomDirectionSpeed = 0;
};

Q_DECLARE_METATYPE(ControllerSpc2000::ControllerButtonAction)
