import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "Popup"
import "Popup/EventSearch"

Item {
    id: searchParent

    Connections{
        target: searchService

        // Event search view
        onEventSearchView_SetVisible: eventSearchView.setVisible(visible)
        onEventSearchView_ResetFilter: eventSearchView.resetFilter()
        onEventSearchView_SetChannelFilter: eventSearchView.setChannelFilter(channelGuids)
        onEventSearchView_SetDateFilter: eventSearchView.setDateFilter(start, end)
        onEventSearchView_SearchByFilter: eventSearchView.searchByFilter()

        onEventSearchPopupOpenShortcutClicked:{
            eventSearchView.setSearchMenu(menu)

            console.log("popupService::onEventSearchPopupOpen() 1")
            if(eventSearchView.visible){
                //if(setupMainView.visible)
                //setupMainView.raise()
                eventSearchView.raise()
                eventSearchView.show()
            }
            else{
                eventSearchView.visible = true
                eventSearchView.resetFilter()
                eventSearchView.searchByFilter()
                console.log("popupService::onEventSearchPopupOpen() 2")
            }
        }

        onCloseAll: eventSearchView.close()

        onInitializeTree: eventSearchView.initializeTree()
    }

    EventSearchView{
        id: eventSearchView

        visible: false
    }
}
