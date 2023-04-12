/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "tst_test__coreservice.h"

void Test__CoreService::test_SaveRemoveBookmark()
{
    // Sample Data
    Core::Bookmark bookmark1;
    bookmark1.bookmarkID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
    bookmark1.name = "bookmark1";

    Core::Bookmark bookmark2;
    bookmark2.bookmarkID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
    bookmark2.name = "bookmark2";

    // SAVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, bookmark1, bookmark2](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::SaveBookmarkEventType) {
            auto saveBookmarkEvent = std::static_pointer_cast<Core::SaveBookmarkEvent>(event);
            if(saveBookmarkEvent->bookmark.bookmarkID == bookmark1.bookmarkID ||
                    saveBookmarkEvent->bookmark.bookmarkID == bookmark2.bookmarkID) {
                SPDLOG_DEBUG("Save bookmark event, bookmarkID={}", saveBookmarkEvent->bookmark.bookmarkID);
                EventNotify(true);
            }
            else {
                EventNotify(false);
            }
        }
    });

    // save request and check response
    for(int i=0 ; i<2 ; i++)
    {
        auto saveRequest = std::make_shared<Core::SaveBookmarkRequest>();
        if(i==0)
            saveRequest->bookmark = bookmark1;
        else
            saveRequest->bookmark = bookmark2;

        InitializeNotify();
        m_service->SaveBookmark(saveRequest, [this](const ResponseBaseSharedPtr& response){
            SPDLOG_DEBUG("SaveBookmark() response");
            Notify(response->isSuccess());
        });

        // wait response
        QVERIFY2(Wait(), "SaveBookmark() failed");
        // wait event
        QVERIFY2(EventWait(), "SaveBookmarkEvent() failed");
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // REMOVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, bookmark1, bookmark2](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveBookmarksEventType) {
            auto removeBookmarksEvent = std::static_pointer_cast<Core::RemoveBookmarksEvent>(event);
            if(removeBookmarksEvent->bookmarkIDs.size()==2 &&
                    removeBookmarksEvent->bookmarkIDs[0] == bookmark1.bookmarkID &&
                    removeBookmarksEvent->bookmarkIDs[1] == bookmark2.bookmarkID) {
                SPDLOG_DEBUG("Remove bookmarks event");
                EventNotify(true);
            }
            else {
                EventNotify(false);
            }
        }
    });

    // remove request and check response
    auto removeRequest = std::make_shared<Core::RemoveBookmarksRequest>();
    removeRequest->bookmarkIDs.push_back(bookmark1.bookmarkID);
    removeRequest->bookmarkIDs.push_back(bookmark2.bookmarkID);

    InitializeNotify();
    m_service->RemoveBookmarks(removeRequest, [this](const ResponseBaseSharedPtr& response){
        SPDLOG_DEBUG("RemoveBookmarks() response");
        Notify(response->isSuccess());
    });

    // wait response
    QVERIFY2(Wait(), "RemoveBookmarks() failed");
    // wait event
    QVERIFY2(EventWait(), "RemoveBookmarksEvent failed");

    // RESTORE EVENT HANDLER ----------------------------------------
    SetEventTestHandler(nullptr);
}
