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
#include "MediaSourceFrameQueue.h"


void MediaSourceFrameQueue::push(const Wisenet::Media::MediaSourceFrameBaseSharedPtr &data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (data->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        m_videoSize++;
    }
    else if (data->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        m_audioSize++;
    }
    else if (data->getMediaType() == Wisenet::Media::MediaType::META_DATA) {
        m_metaSize++;
    }
    m_queue.push(data);
}

void MediaSourceFrameQueue::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::queue<Wisenet::Media::MediaSourceFrameBaseSharedPtr> empty;
    std::swap(m_queue, empty);
    m_videoSize = 0;
    m_audioSize = 0;
    m_metaSize = 0;
}

bool MediaSourceFrameQueue::empty()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

bool MediaSourceFrameQueue::pop_front(Wisenet::Media::MediaSourceFrameBaseSharedPtr &value)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
        return false;

    value = m_queue.front();
    m_queue.pop();
    if (value->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        m_videoSize--;
    }
    else if (value->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        m_audioSize--;
    }
    else if (value->getMediaType() == Wisenet::Media::MediaType::META_DATA) {
        m_metaSize--;
    }
    return true;
}

bool MediaSourceFrameQueue::front(Wisenet::Media::MediaSourceFrameBaseSharedPtr &value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty())
        return false;

    value = m_queue.front();
    return true;
}

bool MediaSourceFrameQueue::pop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty())
        return false;

    auto value = m_queue.front();
    m_queue.pop();

    if (value->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        m_videoSize--;
    }
    else if (value->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        m_audioSize--;
    }
    else if (value->getMediaType() == Wisenet::Media::MediaType::META_DATA) {
        m_metaSize--;
    }
    return true;
}

size_t MediaSourceFrameQueue::size()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
}

size_t MediaSourceFrameQueue::videoSize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_videoSize;
}

size_t MediaSourceFrameQueue::audioSize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_audioSize;
}

size_t MediaSourceFrameQueue::metaSize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metaSize;
}
