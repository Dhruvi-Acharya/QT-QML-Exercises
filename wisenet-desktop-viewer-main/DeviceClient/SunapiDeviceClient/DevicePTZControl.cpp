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
#include "SunapiDeviceClientImpl.h"

//Maximum values
constexpr double PAN = 6.0;
constexpr double TILT = 6.0;
constexpr double ZOOM = 4.0;

namespace Wisenet
{
namespace Device
{


class PTZControlContinuous : public BaseCommand
{
public:
    explicit PTZControlContinuous(const std::string& logPrefix, DevicePTZContinuousRequestSharedPtr request, bool normalizedSpeed = false)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request),
         m_normalizedSpeed(normalizedSpeed)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        int pan = m_request->pan;
        int tilt = m_request->tilt;
        int zoom = m_request->zoom;

        builder.CreateCgi("ptzcontrol", "continuous", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        //-100 ~ 100의 값을 각각의 범위에 맞게 normalize
        if(m_normalizedSpeed == false){
            double f_pan = (m_request->pan +100)*PAN/100 - PAN;
            double f_tilt = (m_request->tilt +100)*TILT/100 - TILT;
            double f_zoom = (m_request->zoom +100)*ZOOM/100 - ZOOM;

            //양수는 올림, 음수는 내림
            pan = f_pan > 0.0f ? static_cast<int>(ceil(f_pan)) : static_cast<int>(floor(f_pan));
            tilt = f_tilt > 0.0f ? static_cast<int>(ceil(f_tilt)) : static_cast<int>(floor(f_tilt));
            zoom = f_zoom > 0.0f ? static_cast<int>(ceil(f_zoom)) : static_cast<int>(floor(f_zoom));

        }
        else {
            builder.AddParameter("NormalizedSpeed", m_normalizedSpeed);
        }

        if (pan != 0)
            builder.AddParameter("Pan", pan);
        if (tilt != 0)
            builder.AddParameter("Tilt", tilt);
        if (zoom != 0)
            builder.AddParameter("Zoom", zoom);

        if(m_request->viewModeType != ViewModeType::Unknown){
            if(m_request->viewModeType == ViewModeType::Panorama)
                builder.AddParameter("ViewModeType", "Panorama");
            else if(m_request->viewModeType == ViewModeType::DoublePanorama)
                builder.AddParameter("ViewModeType", "DoublePanorama");
        }
        else if(m_request->subViewIndex != 0)
            builder.AddParameter("SubViewIndex", m_request->subViewIndex);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DevicePTZContinuousRequestSharedPtr m_request;
    bool m_normalizedSpeed;
};

void SunapiDeviceClientImpl::DevicePTZContinuous(DevicePTZContinuousRequestSharedPtr const& request,
                                                    ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlContinuous>(m_logPrefix, request, m_repos->attributes().ptzControlCgi.continous.normalizedSpeed),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}


class PTZControlFocus : public BaseCommand
{
public:
    explicit PTZControlFocus(const std::string& logPrefix, DevicePTZFocusRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }
    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        std::string focus = "";

        switch (m_request->focus) {
        case DevicePTZFocusRequest::Focus::Near: focus = "Near"; break;
        case DevicePTZFocusRequest::Focus::Far: focus = "Far"; break;
        case DevicePTZFocusRequest::Focus::Stop: focus = "Stop"; break;
        default: break;
        }

        builder.CreateCgi("ptzcontrol", "continuous", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Focus", focus);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }
private:
    DevicePTZFocusRequestSharedPtr m_request;
};


void SunapiDeviceClientImpl::DevicePTZFocus(DevicePTZFocusRequestSharedPtr const& request,
                                               ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlFocus>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}


class PTZControlStopControl : public BaseCommand
{
public:
    explicit PTZControlStopControl(const std::string& logPrefix, DevicePTZStopRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "stop", "control")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID))
               .AddParameter("OperationType", std::string("All"));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DevicePTZStopRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DevicePTZStop(DevicePTZStopRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlStopControl>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}


class ImageFocusModeControl : public BaseCommand
{
public:
    explicit ImageFocusModeControl(const std::string& logPrefix, DeviceImageFocusModeRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }
    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        std::string focusMode = "";

        switch (m_request->focusMode) {
        case DeviceImageFocusModeRequest::FocusMode::SimpleFocus: focusMode = "SimpleFocus"; break;
        case DeviceImageFocusModeRequest::FocusMode::Reset: focusMode = "Reset"; break;
        case DeviceImageFocusModeRequest::FocusMode::AutoFocus: focusMode = "AutoFocus"; break;
        default: break;
        }

        builder.CreateCgi("image", "focus", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Mode", focusMode);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }
private:
    DeviceImageFocusModeRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceImageFocusMode(DeviceImageFocusModeRequestSharedPtr const& request,
                                                  ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<ImageFocusModeControl>(m_logPrefix, request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

class PTZControlAux : public BaseCommand
{
public:
    explicit PTZControlAux(const std::string& logPrefix, DevicePTZAuxControlRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("ptzcontrol", "aux", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Command", m_request->command);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DevicePTZAuxControlRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DevicePTZAuxControl(DevicePTZAuxControlRequestSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlAux>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
