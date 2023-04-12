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
#include "ControllerSpc2000.h"

ControllerSpc2000::ControllerSpc2000()
{

}

ControllerSpc2000::~ControllerSpc2000()
{

}

void ControllerSpc2000::processControllerData(unsigned char *controllerReceivedData) {
    processPtz(controllerReceivedData);
    processButton(controllerReceivedData);
}

void ControllerSpc2000::processPtz(unsigned char *controllerReceivedData) {
    /////// PTZ ///////
    m_isPtzChangedEmit = false;
    m_panDirectionSpeed = 0;
    m_tiltDirectionSpeed = 0;
    m_zoomDirectionSpeed = 0;

    const int maxPtzValue = 255 * 4;

    // pan value get
    int defaultPanValue = m_panValue + m_panLevel*255;
    int currentPanValue = (int)controllerReceivedData[0] + (int)controllerReceivedData[1]*255;
    int panValueDiff = (currentPanValue - defaultPanValue)*100 / maxPtzValue;

    if(abs(panValueDiff) > m_minPan) {
        // pan request
        m_panDirectionSpeed = panValueDiff;
    }

    // tilt value get
    int defaultTiltValue = m_tiltValue + m_tiltLevel*255;
    int currentTiltValue = (int)controllerReceivedData[2] + (int)controllerReceivedData[3]*255;
    int tiltValueDiff = (currentTiltValue - defaultTiltValue)*100 / maxPtzValue;

    if(abs(tiltValueDiff) > m_minTilt) {
        // tilt request
        m_tiltDirectionSpeed = -tiltValueDiff;
    }

    // zoom value get
    int defaultZoomValue = m_zoomValue + m_zoomLevel*255;
    int currentZoomValue = (int)controllerReceivedData[4] + (int)controllerReceivedData[5]*255;
    int zoomValueDiff = (currentZoomValue - defaultZoomValue)*100 / maxPtzValue;
    zoomValueDiff *= 2;

    if(abs(zoomValueDiff) > m_minZoom) {
        // zoom request
        m_zoomDirectionSpeed = zoomValueDiff;
    }

    if(m_panDirectionSpeed != 0 || m_tiltDirectionSpeed != 0 || m_zoomDirectionSpeed !=0) {
        m_isPtzChangedEmit = true;
    }
    else { // ptz모두 값이 0이라면
        if(m_prevPanDirectionSpeed!=0 || m_prevTiltDirectionSpeed!=0 || m_prevZoomDirectionSpeed!=0) { // 이전 ptz값 중 하나라도 0이 아니었다면 ptz멈춤 요청
            m_isPtzChangedEmit = true;
        }
    }

//    if(m_isPtzChangedEmit) {
//        printf("ControllerSpc2000::processPtz() - m_panDirectionSpeed=%d m_tiltDirectionSpeed=%d m_zoomDirectionSpeed=%d\n",m_panDirectionSpeed,m_tiltDirectionSpeed,m_zoomDirectionSpeed);
//        fflush(stdout);
//    }

    m_prevPanDirectionSpeed = m_panDirectionSpeed;
    m_prevTiltDirectionSpeed = m_tiltDirectionSpeed;
    m_prevZoomDirectionSpeed = m_zoomDirectionSpeed;
}

void ControllerSpc2000::processButton(unsigned char *controllerReceivedData) {
    /////// Button ///////
    m_isButtonChangedEmit = false;
    int pressedButton = 0;

    // 1~8번 버튼 입력에 대해 데이터 처리
    if((int)controllerReceivedData[6] != 0) {
        pressedButton = (int)controllerReceivedData[6];

        m_isButtonChangedEmit = true;

        // 스크린 모드
        if(m_controllerMode == ControllerMode::ScreenMode) {
            switch (pressedButton) {
            case ControllerButtonInput::LayoutTab1:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab1;
                break;
            case ControllerButtonInput::LayoutTab2:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab2;
                break;
            case ControllerButtonInput::LayoutTab3:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab3;
                break;
            case ControllerButtonInput::LayoutTab4:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab4;
                break;
            case ControllerButtonInput::LayoutTab5:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab5;
                break;
            case ControllerButtonInput::LayoutTab6:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab6;
                break;
            case ControllerButtonInput::LayoutTab7:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab7;
                break;
            case ControllerButtonInput::LayoutTab8:
                m_pressedButtonAction = ControllerButtonAction::MoveLayoutTab8;
                break;
            default:
                break;
            }
        }
        // PTZ 모드
        else if(m_controllerMode == ControllerMode::PtzMode) {
            switch (pressedButton) {
            case ControllerButtonInput::Preset1:
                m_pressedButtonAction = ControllerButtonAction::DoPreset1;
                break;
            case ControllerButtonInput::Preset2:
                m_pressedButtonAction = ControllerButtonAction::DoPreset2;
                break;
            case ControllerButtonInput::Preset3:
                m_pressedButtonAction = ControllerButtonAction::DoPreset3;
                break;
            case ControllerButtonInput::Preset4:
                m_pressedButtonAction = ControllerButtonAction::DoPreset4;
                break;
            case ControllerButtonInput::Preset5:
                m_pressedButtonAction = ControllerButtonAction::DoPreset5;
                break;
            case ControllerButtonInput::Preset6:
                m_pressedButtonAction = ControllerButtonAction::DoPreset6;
                break;
            case ControllerButtonInput::Preset7:
                m_pressedButtonAction = ControllerButtonAction::DoPreset7;
                break;
            case ControllerButtonInput::Preset8:
                m_pressedButtonAction = ControllerButtonAction::DoPreset8;
                break;
            default:
                break;
            }
        }
    }

    // 9~12번 버튼 입력에 대해 데이터 처리
    if((int)controllerReceivedData[7] != 0) {
        pressedButton = (int)controllerReceivedData[7];
        m_isButtonChangedEmit = true;

//        printf("(int)controllerReceivedData[7]=%d\n",pressedButton);
//        fflush(stdout);

        // 스크린 모드
        if(m_controllerMode == ControllerMode::ScreenMode) {
            switch (pressedButton) {
            case ControllerButtonInput::LeftWindow:
                m_pressedButtonAction = ControllerButtonAction::MoveLeftWindow;
                break;
            case ControllerButtonInput::RightWindow:
                m_pressedButtonAction = ControllerButtonAction::MoveRightWindow;
                break;
            default:
                break;
            }
        }
        // PTZ 모드
        else if(m_controllerMode == ControllerMode::PtzMode) {
            switch (pressedButton) {
            case ControllerButtonInput::SingleChannelMode:
                m_pressedButtonAction = ControllerButtonAction::SwitchSingleChannelMode;
                break;
            case ControllerButtonInput::MultiChannelMode:
                m_pressedButtonAction = ControllerButtonAction::SwitchMultiChannelMode;
                break;
            default:
                break;
            }
        }

        switch (pressedButton) {
        case ControllerButtonInput::PtzMode:
            m_pressedButtonAction = ControllerButtonAction::SetPtzMode;
            break;
        case ControllerButtonInput::ScreenMode:
            m_pressedButtonAction = ControllerButtonAction::SetScreenMode;
            break;
        default:
            break;
        }
    }

}
