import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0
import WisenetMediaFramework 1.0
import "qrc:/Masking/"

Item {
    id: maskingParent

    Connections{
        target: maskingService

        onMaskingView_Open: maskingView.show()
        onCloseAll: maskingView.close()
    }

    MaskingView {
        id: maskingView
    }
}
