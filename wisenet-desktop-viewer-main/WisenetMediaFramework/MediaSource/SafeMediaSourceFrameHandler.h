#pragma once

#include <memory>
#include "Media/MediaSourceFrame.h"

class SafeMediaSourceFrameHandler : public std::enable_shared_from_this<SafeMediaSourceFrameHandler>
{
public:
    ~SafeMediaSourceFrameHandler();
    static std::shared_ptr<SafeMediaSourceFrameHandler> Create();
    void SetMediaHandler(Wisenet::Media::MediaSourceFrameHandler& handler);
    Wisenet::Media::MediaSourceFrameHandler GetSafeMediaHandler();

private:
    SafeMediaSourceFrameHandler();
    Wisenet::Media::MediaSourceFrameHandler m_mediaHandler;
};

typedef std::shared_ptr<SafeMediaSourceFrameHandler> SafeMediaHandlerSharedPtr;
