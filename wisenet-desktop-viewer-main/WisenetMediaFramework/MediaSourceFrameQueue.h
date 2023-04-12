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

#include <queue>
#include <mutex>
#include "Media/MediaSourceFrame.h"

class MediaSourceFrameQueue
{
public:
    MediaSourceFrameQueue() = default;
    ~MediaSourceFrameQueue() = default;

    void push(const Wisenet::Media::MediaSourceFrameBaseSharedPtr& data);
    void clear();
    bool empty();
    bool pop_front(Wisenet::Media::MediaSourceFrameBaseSharedPtr& value);
    bool front(Wisenet::Media::MediaSourceFrameBaseSharedPtr& value);
    bool pop();
    size_t size();
    size_t videoSize();
    size_t audioSize();
    size_t metaSize();
private:
    std::mutex m_mutex;
    std::queue<Wisenet::Media::MediaSourceFrameBaseSharedPtr> m_queue;
    size_t m_videoSize = 0;
    size_t m_audioSize = 0;
    size_t m_metaSize = 0;
};

