#include "SafeMediaSourceFrameHandler.h"
#include "WeakCallback.h"
#include "LogSettings.h"

SafeMediaSourceFrameHandler::SafeMediaSourceFrameHandler()
    : m_mediaHandler(nullptr)
{
    SPDLOG_DEBUG("SafeMediaSourceFrameHandler::SafeMediaSourceFrameHandler()");
}

SafeMediaSourceFrameHandler::~SafeMediaSourceFrameHandler()
{
    SPDLOG_DEBUG("SafeMediaSourceFrameHandler::~SafeMediaSourceFrameHandler()");
    m_mediaHandler = nullptr;
}

std::shared_ptr<SafeMediaSourceFrameHandler> SafeMediaSourceFrameHandler::Create()
{
    struct make_shared_enabler : public SafeMediaSourceFrameHandler{};
    return std::make_shared<make_shared_enabler>();
}

void SafeMediaSourceFrameHandler::SetMediaHandler(Wisenet::Media::MediaSourceFrameHandler& handler)
{
    m_mediaHandler = handler;
}

Wisenet::Media::MediaSourceFrameHandler SafeMediaSourceFrameHandler::GetSafeMediaHandler()
{
    auto safeHandler =
            WeakCallback(shared_from_this(),
                         [this](const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
    {
        if (m_mediaHandler) {
            //SPDLOG_DEBUG("MediaCallBack!!!");
            m_mediaHandler(mediaSourceFrame);
        }
//        else {
//            SPDLOG_DEBUG("HANDLER BROKEN!!!");
//        }
    });
    return safeHandler;
}
