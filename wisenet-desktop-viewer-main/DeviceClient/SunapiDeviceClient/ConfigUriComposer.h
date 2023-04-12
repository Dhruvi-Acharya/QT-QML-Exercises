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

#include <memory>
#include <boost/optional.hpp>
#include "DeviceClient/DeviceStructure.h"
#include "cgis/SystemCgi.h"
#include "cgis/NetworkCgi.h"
#include "cgis/AttributesCgi.h"
#include "cgis/MediaCgi.h"
#include "cgis/RecordingCgi.h"
#include "cgis/EventSourcesCgi.h"
#include "cgis/IoCgi.h"
#include "cgis/EventStatusCgi.h"
namespace Wisenet
{
namespace Device
{

struct ConfigUriComposer
{
    void setAttributes(const AttributesResult& attributesResult)
    {
        m_setObjectComposeFlag = true;
        m_attributes = attributesResult;
    }
    void setSystemDateView(const SystemDateViewResult& viewResult)
    {
        m_setObjectComposeFlag = true;
        m_dateView = viewResult;
    }
    void setRecordingStorageView(const RecordingStorageViewResult& viewResult)
    {
        m_setObjectComposeFlag = true;
        m_recordingStorageView = viewResult;
    }
    void setRecordingGeneralView(const RecordingGeneralViewResult& viewResult)
    {
        m_setObjectComposeFlag = true;
        m_recordingGeneralView = viewResult;
    }
    void setMediaVideoSourceView(const MediaVideoSourceViewResult& viewResult)
    {
        m_setObjectComposeFlag = true;
        m_videoSourceView = viewResult;
    }
    void setMediaVideoProfileView(const MediaVideoProfileViewResult& viewResult)
    {
        m_setObjectComposeFlag = true;
        m_videoProfileView = viewResult;
    }
    void setMediaVideoProfilePolicyView(const MediaVideoProfilePolicyViewResult& viewResult)
    {
        m_setObjectComposeFlag = true;
        m_videoProfilePolicyView = viewResult;
    }

    void setNetworkRtspView(const NetworkRtspViewResult& viewResult)
    {
        m_networkRtspView = viewResult;
    }

    boost::optional<SystemDateViewResult>                m_dateView;
    boost::optional<MediaVideoSourceViewResult>          m_videoSourceView;
    boost::optional<MediaVideoProfileViewResult>         m_videoProfileView;
    boost::optional<MediaVideoProfilePolicyViewResult>   m_videoProfilePolicyView;
    boost::optional<RecordingStorageViewResult>          m_recordingStorageView;
    boost::optional<RecordingGeneralViewResult>          m_recordingGeneralView;
    boost::optional<AttributesResult>                    m_attributes;

    /* attribute와 관련이 없는 RTSP 연결정보 변경시 */
    boost::optional<NetworkRtspViewResult>               m_networkRtspView;

    bool m_setObjectComposeFlag = false;
};

typedef std::shared_ptr<ConfigUriComposer> ConfigUriComposerPtr;

}
}
