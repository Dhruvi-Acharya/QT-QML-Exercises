import QtQuick 2.15
import WisenetStyle 1.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Wisenet.DeviceSelectionTreeSourceModel 1.0
import Wisenet.ResourceTreeProxyModel 1.0
import Wisenet.EventFilterTreeSourceModel 1.0
import WisenetLanguage 1.0
import "qrc:/"
import Qt.labs.qmlmodels 1.0
import Wisenet.Setup 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.impl 2.12
import QtQml.Models 2.15

Rectangle{
    id: aiSearchFilterView
    visible: true
    color: WisenetGui.contrast_09_dark

    property int itemDefaultLeftMargin: 24
    property int objectOptionDefaultRectHeight: 60
    property int filterdefaultWidth: 195
    property int objectOptionTitleTextHeight: 15
    property int classType: ObjectSearchFilterView.AiClassType.Person

    signal resetListPosition()

    enum AiClassType
    {
        Person = 0,
        Vehicle,
        Face,
        Ocr
    }

    component ShortcutButton: WisenetImageButton {
        width: 30
        height: 30
        sourceWidth: 30
        sourceHeight: 30
        property alias tooltipText:shortcutTooltip.text
        WisenetMediaToolTip {
            id: shortcutTooltip
            delay: 500
            visible: parent.hovered && shortcutTooltip.text !== ""
        }
    }

    component ObjectOptionButton: WisenetDualImageCheckButton {
        width: 32
        height: 28
        sourceWidth: 28
        sourceHeight: 28
        checkSourceWidth: 16
        checkSourceHeight: 16

        imageChecked: WisenetImage.object_search_filter_checked
        imageCheckedHover: WisenetImage.object_search_filter_hover

        property alias tooltipText: objectOptionButtonTooltip.text
        WisenetMediaToolTip {
            id: objectOptionButtonTooltip
            delay: 500
            visible: parent.hovered
        }
    }

    function reset(){
        deviceSeletectionTree.reset()
        calendarControlView.resetMultiSelection()

        var currentDateTime = new Date()

        fromTime.setTimeText(currentDateTime.getHours()-1, currentDateTime.getMinutes(), currentDateTime.getSeconds())
        toTime.setTimeText(23,59,59)
        objectTypeList.reset()
        objectOptionRect.reset()

        clearPersonObjectOption()
        clearFaceObjectOption()
        clearVehicleObjectOption()
        clearOcrObjectOption()

        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
    }

    function setChannelFilter(channels)
    {
        console.debug("setchannelFilter: " + channels)
        deviceSeletectionTree.selectChannels(channels)
    }

    function setDateFilter(from, to)
    {
        updateCalendarLimitDate()

        aiSearchFromDateEditor.setDate(from)
        aiSearchToDateEditor.setDate(to)

        fromTime.setTimeText(from.getHours(),from.getMinutes(),from.getSeconds())
        toTime.setTimeText(to.getHours(),to.getMinutes(),to.getSeconds())

        updateCalendar()
    }

    function searchByFilter()
    {
        var from = new Date(aiSearchFromDateEditor.year, aiSearchFromDateEditor.month - 1, aiSearchFromDateEditor.day, fromTime.hour, fromTime.minute, fromTime.second)
        var to = new Date(aiSearchToDateEditor.year, aiSearchToDateEditor.month - 1, aiSearchToDateEditor.day, toTime.hour, toTime.minute, toTime.second)

        var channels = [];

        if(deviceSeletectionTree.selectedDeviceValue !== null && deviceSeletectionTree.selectedDeviceValue.length > 0){
            deviceSeletectionTree.selectedDeviceValue.forEach(function(item){
                channels.push(item.uuid)
            })
        }

        console.log("===========searchByFilter==============")
        console.log("from=",from)
        console.log("to=",to)
        console.log("channels=",channels)
        console.log("classType=",classType)
        console.log("deviceSeletectionTree.allDevice=",deviceSeletectionTree.allDevice)
        console.log("=======================================")

        objectSearchViewModel.objectSearchFilterViewModel.search(from, to, channels, classType, deviceSeletectionTree.allDevice)
    }

    function updateCalendar(){
        var fromDate = new Date(aiSearchFromDateEditor.year,
                                aiSearchFromDateEditor.month-1,
                                aiSearchFromDateEditor.day,
                                0,0,0)
        var toDate = new Date(aiSearchToDateEditor.year,
                              aiSearchToDateEditor.month-1,
                              aiSearchToDateEditor.day,
                              0,0,0)
        calendarControlView.calendarTimelineModel.setSelectionStartDate(fromDate)
        calendarControlView.calendarTimelineModel.setSelectionEndDate(toDate)
        calendarControlView.updateVisibleYearMonth()
    }

    function updateCalendarLimitDate(){
        var currentDate = new Date()
        calendarControlView.minimumDate = new Date(currentDate.getFullYear() - 2,0,1,0,0,0)
        currentDate.setDate(currentDate.getDate() + 1)
        calendarControlView.maximumDate = currentDate
    }

    function initializeTree(){
        aiSearchDeviceTreeSourceModel.initializeTree()
    }

    function clearAllObjectButton() {
        personObjectButton.checked = false
        faceObjectButton.checked = false
        vehicleObjectButton.checked = false
        ocrObjectButton.checked = false
    }

    function clearPersonObjectOption() {
        // Gender
        personGenderOptionMaleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderMaleButtonChecked = false

        personGenderOptionFemaleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderFemaleButtonChecked = false

        // Clothing Top
        personClothingTopBlackButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlackButtonChecked = false

        personClothingTopGrayButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGrayButtonChecked = false

        personClothingTopWhiteButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopWhiteButtonChecked = false

        personClothingTopRedButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopRedButtonChecked = false

        personClothingTopOrangeButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopOrangeButtonChecked = false

        personClothingTopYellowButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopYellowButtonChecked = false

        personClothingTopGreenButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGreenButtonChecked = false

        personClothingTopBlueButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlueButtonChecked = false

        personClothingTopPurpleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopPurpleButtonChecked = false

        // Clothing Bottom
        personClothingBottomBlackButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlackButtonChecked = false

        personClothingBottomGrayButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGrayButtonChecked = false

        personClothingBottomWhiteButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomWhiteButtonChecked = false

        personClothingBottomRedButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomRedButtonChecked = false

        personClothingBottomOrangeButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomOrangeButtonChecked = false

        personClothingBottomYellowButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomYellowButtonChecked = false

        personClothingBottomGreenButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGreenButtonChecked = false

        personClothingBottomBlueButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlueButtonChecked = false

        personClothingBottomPurpleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomPurpleButtonChecked = false

        //Bag
        personBagOptionWearButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonBagWearButtonChecked = false

        personBagOptionNoWearButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isPersonBagWearButtonChecked = false

    }

    function clearFaceObjectOption() {
        // Gender
        faceGenderOptionMaleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderMaleButtonChecked = false
        faceGenderOptionFemaleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderFemaleButtonChecked = false

        // Age
        faceAgeTypeYoungButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeYoungButtonChecked = false
        faceAgeTypeAdultButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeAdultButtonChecked = false
        faceAgeTypeMiddleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeMiddleButtonChecked = false
        faceAgeTypeSeniorButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeSeniorButtonChecked = false

        // Opticals
        faceOpticalsOptionWearButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsWearButtonChecked = false
        faceOpticalsOptionNoWearButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsNoWearButtonChecked = false

        // Mask
        faceMaskOptionWearButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskWearButtonChecked = false
        faceMaskOptionNoWearButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskNoWearButtonChecked = false
    }

    function clearVehicleObjectOption() {
        // Type
        vehicleTypeOptionCarButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeCarButtonChecked = false
        vehicleTypeOptionBusButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBusButtonChecked = false
        vehicleTypeOptionTruckButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeTruckButtonChecked = false
        vehicleTypeOptionMotorcycleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeMotorcycleButtonChecked = false
        vehicleTypeOptionBicycleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBicycleButtonChecked = false

        // Color
        vehicleColorBlackButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlackButtonChecked = false
        vehicleColorGrayButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGrayButtonChecked = false
        vehicleColorWhiteButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorWhiteButtonChecked = false
        vehicleColorRedButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorRedButtonChecked = false
        vehicleColorOrangeButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeCarButtonChecked = false
        vehicleColorYellowButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorYellowButtonChecked = false
        vehicleColorGreenButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGreenButtonChecked = false
        vehicleColorBlueButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlueButtonChecked = false
        vehicleColorPurpleButton.checked = false
        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorPurpleButtonChecked = false

    }

    function clearOcrObjectOption() {
        objectSearchViewModel.objectSearchFilterViewModel.ocrLicensePlateText = ""
        ocrObjectContentLicensePlateTextBox.text = ""
    }

    Component.onCompleted: {
        reset();
    }

    // connect between ObjectSearchFilterViewModel and ObjectSearchFilterView
    Connections{
        target: objectSearchViewModel.objectSearchFilterViewModel

        // checked
        // Person - Gender
        function onPersonGenderMaleButtonCheckedChanged(isPersonGenderMaleButtonChecked){
            personGenderOptionMaleButton.checked = isPersonGenderMaleButtonChecked
            console.log("personGenderOptionMaleButton.checked=",personGenderOptionMaleButton.checked)
        }

        function onPersonGenderFemaleButtonCheckedChanged(isPersonGenderFemaleButtonChecked){
            personGenderOptionFemaleButton.checked = isPersonGenderFemaleButtonChecked
            console.log("personGenderOptionFemaleButton.checked=",personGenderOptionFemaleButton.checked)
        }

        // Person - ClothingTop
        function onPersonClothingTopBlackButtonCheckedChanged(isPersonClothingTopBlackButtonChecked){
            personClothingTopBlackButton.checked = isPersonClothingTopBlackButtonChecked
            console.log("personClothingTopBlackButton.checked=",personClothingTopBlackButton.checked)
        }
        function onPersonClothingTopGrayButtonCheckedChanged(isPersonClothingTopGrayButtonChecked){
            personClothingTopGrayButton.checked = isPersonClothingTopGrayButtonChecked
            console.log("personClothingTopGrayButton.checked=",personClothingTopGrayButton.checked)
        }
        function onPersonClothingTopWhiteButtonCheckedChanged(isPersonClothingTopWhiteButtonChecked){
            personClothingTopWhiteButton.checked = isPersonClothingTopWhiteButtonChecked
            console.log("personClothingTopWhiteButton.checked=",personClothingTopWhiteButton.checked)
        }
        function onPersonClothingTopRedButtonCheckedChanged(isPersonClothingTopRedButtonChecked){
            personClothingTopRedButton.checked = isPersonClothingTopRedButtonChecked
            console.log("personClothingTopRedButton.checked=",personClothingTopRedButton.checked)
        }
        function onPersonClothingTopOrangeButtonCheckedChanged(isPersonClothingTopOrangeButtonChecked){
            personClothingTopOrangeButton.checked = isPersonClothingTopOrangeButtonChecked
            console.log("personClothingTopOrangeButton.checked=",personClothingTopOrangeButton.checked)
        }
        function onPersonClothingTopYellowButtonCheckedChanged(isPersonClothingTopYellowButtonChecked){
            personClothingTopYellowButton.checked = isPersonClothingTopYellowButtonChecked
            console.log("personClothingTopYellowButton.checked=",personClothingTopYellowButton.checked)
        }
        function onPersonClothingTopGreenButtonCheckedChanged(isPersonClothingTopGreenButtonChecked){
            personClothingTopGreenButton.checked = isPersonClothingTopGreenButtonChecked
            console.log("personClothingTopGreenButton.checked=",personClothingTopGreenButton.checked)
        }
        function onPersonClothingTopBlueButtonCheckedChanged(isPersonClothingTopBlueButtonChecked){
            personClothingTopBlueButton.checked = isPersonClothingTopBlueButtonChecked
            console.log("personClothingTopBlueButton.checked=",personClothingTopBlueButton.checked)
        }
        function onPersonClothingTopPurpleButtonCheckedChanged(isPersonClothingTopPurpleButtonChecked){
            personClothingTopPurpleButton.checked = isPersonClothingTopPurpleButtonChecked
            console.log("personClothingTopPurpleButton.checked=",personClothingTopPurpleButton.checked)
        }

        // Person - ClothingBottom
        function onPersonClothingBottomBlackButtonCheckedChanged(isPersonClothingBottomBlackButtonChecked){
            personClothingBottomBlackButton.checked = isPersonClothingBottomBlackButtonChecked
            console.log("personClothingBottomBlackButton.checked=",personClothingBottomBlackButton.checked)
        }
        function onPersonClothingBottomGrayButtonCheckedChanged(isPersonClothingBottomGrayButtonChecked){
            personClothingBottomGrayButton.checked = isPersonClothingBottomGrayButtonChecked
            console.log("personClothingBottomGrayButton.checked=",personClothingBottomGrayButton.checked)
        }
        function onPersonClothingBottomWhiteButtonCheckedChanged(isPersonClothingBottomWhiteButtonChecked){
            personClothingBottomWhiteButton.checked = isPersonClothingBottomWhiteButtonChecked
            console.log("personClothingBottomWhiteButton.checked=",personClothingBottomWhiteButton.checked)
        }
        function onPersonClothingBottomRedButtonCheckedChanged(isPersonClothingBottomRedButtonChecked){
            personClothingBottomRedButton.checked = isPersonClothingBottomRedButtonChecked
            console.log("personClothingBottomRedButton.checked=",personClothingBottomRedButton.checked)
        }
        function onPersonClothingBottomOrangeButtonCheckedChanged(isPersonClothingBottomOrangeButtonChecked){
            personClothingBottomOrangeButton.checked = isPersonClothingBottomOrangeButtonChecked
            console.log("personClothingBottomOrangeButton.checked=",personClothingBottomOrangeButton.checked)
        }
        function onPersonClothingBottomYellowButtonCheckedChanged(isPersonClothingBottomYellowButtonChecked){
            personClothingBottomYellowButton.checked = isPersonClothingBottomYellowButtonChecked
            console.log("personClothingBottomYellowButton.checked=",personClothingBottomYellowButton.checked)
        }
        function onPersonClothingBottomGreenButtonCheckedChanged(isPersonClothingBottomGreenButtonChecked){
            personClothingBottomGreenButton.checked = isPersonClothingBottomGreenButtonChecked
            console.log("personClothingBottomGreenButton.checked=",personClothingBottomGreenButton.checked)
        }
        function onPersonClothingBottomBlueButtonCheckedChanged(isPersonClothingBottomBlueButtonChecked){
            personClothingBottomBlueButton.checked = isPersonClothingBottomBlueButtonChecked
            console.log("personClothingBottomBlueButton.checked=",personClothingBottomBlueButton.checked)
        }
        function onPersonClothingBottomPurpleButtonCheckedChanged(isPersonClothingBottomPurpleButtonChecked){
            personClothingBottomPurpleButton.checked = isPersonClothingBottomPurpleButtonChecked
            console.log("personClothingBottomPurpleButton.checked=",personClothingBottomPurpleButton.checked)
        }

        // Person - Bag
        function onPersonBagWearButtonCheckedChanged(isPersonBagWearButtonChecked){
            personBagOptionWearButton.checked = isPersonBagWearButtonChecked
            console.log("personBagOptionWearButton.checked=",personBagOptionWearButton.checked)
        }
        function onPersonBagNoWearButtonCheckedChanged(isPersonBagNoWearButtonChecked){
            personBagOptionNoWearButton.checked = isPersonBagNoWearButtonChecked
            console.log("personBagOptionNoWearButton.checked=",personBagOptionNoWearButton.checked)
        }

        // Face - Gender
        function onFaceGenderMaleButtonCheckedChanged(isFaceGenderMaleButtonChecked){
            faceGenderOptionMaleButton.checked = isFaceGenderMaleButtonChecked
            console.log("faceGenderOptionMaleButton.checked=",faceGenderOptionMaleButton.checked)
        }
        function onFaceGenderFemaleButtonCheckedChanged(isFaceGenderFemaleButtonChecked){
            faceGenderOptionFemaleButton.checked = isFaceGenderFemaleButtonChecked
            console.log("faceGenderOptionFemaleButton.checked=",faceGenderOptionFemaleButton.checked)
        }
        // Face - AgeType
        function onFaceAgeTypeYoungButtonCheckedChanged(isFaceAgeTypeYoungButtonChecked){
            faceAgeTypeYoungButton.checked = isFaceAgeTypeYoungButtonChecked
            console.log("faceAgeTypeYoungButton.checked=",faceAgeTypeYoungButton.checked)
        }
        function onFaceAgeTypeAdultButtonCheckedChanged(isFaceAgeTypeAdultButtonChecked){
            faceAgeTypeAdultButton.checked = isFaceAgeTypeAdultButtonChecked
            console.log("faceAgeTypeAdultButton.checked=",faceAgeTypeAdultButton.checked)
        }
        function onFaceAgeTypeMiddleButtonCheckedChanged(isFaceAgeTypeMiddleButtonChecked){
            faceAgeTypeMiddleButton.checked = isFaceAgeTypeMiddleButtonChecked
            console.log("faceAgeTypeMiddleButton.checked=",faceAgeTypeMiddleButton.checked)
        }
        function onFaceAgeTypeSeniorButtonCheckedChanged(isFaceAgeTypeSeniorButtonChecked){
            faceAgeTypeSeniorButton.checked = isFaceAgeTypeSeniorButtonChecked
            console.log("faceAgeTypeSeniorButton.checked=",faceAgeTypeSeniorButton.checked)
        }
        // Face - Opticals
        function onFaceOpticalsWearButtonCheckedChanged(isFaceOpticalsWearButtonChecked){
            faceOpticalsOptionWearButton.checked = isFaceOpticalsWearButtonChecked
            console.log("faceOpticalsOptionWearButton.checked=",faceOpticalsOptionWearButton.checked)
        }
        function onFaceOpticalsNoWearButtonCheckedChanged(isFaceOpticalsNoWearButtonChecked){
            faceOpticalsOptionNoWearButton.checked = isFaceOpticalsNoWearButtonChecked
            console.log("faceOpticalsOptionNoWearButton.checked=",faceOpticalsOptionNoWearButton.checked)
        }
        // Face - Mask
        function onFaceMaskWearButtonCheckedChanged(isFaceMaskWearButtonChecked){
            faceMaskOptionWearButton.checked = isFaceMaskWearButtonChecked
            console.log("faceMaskOptionWearButton.checked=",faceMaskOptionWearButton.checked)
        }
        function onFaceMaskNoWearButtonCheckedChanged(isFaceMaskNoWearButtonChecked){
            faceMaskOptionNoWearButton.checked = isFaceMaskNoWearButtonChecked
            console.log("faceMaskOptionNoWearButton.checked=",faceMaskOptionNoWearButton.checked)
        }
        // Vehicle - Type
        function onVehicleTypeCarButtonCheckedChanged(isVehicleTypeCarButtonChecked){
            vehicleTypeOptionCarButton.checked = isVehicleTypeCarButtonChecked
            console.log("vehicleTypeOptionCarButton.checked=",vehicleTypeOptionCarButton.checked)
        }
        function onVehicleTypeBusButtonCheckedChanged(isVehicleTypeBusButtonChecked){
            vehicleTypeOptionBusButton.checked = isVehicleTypeBusButtonChecked
            console.log("vehicleTypeOptionBusButton.checked=",vehicleTypeOptionBusButton.checked)
        }
        function onVehicleTypeTruckButtonCheckedChanged(isVehicleTypeTruckButtonChecked){
            vehicleTypeOptionTruckButton.checked = isVehicleTypeTruckButtonChecked
            console.log("vehicleTypeOptionTruckButton.checked=",vehicleTypeOptionTruckButton.checked)
        }
        function onVehicleTypeMotorcycleButtonCheckedChanged(isVehicleTypeMotorcycleButtonChecked){
            vehicleTypeOptionMotorcycleButton.checked = isVehicleTypeMotorcycleButtonChecked
            console.log("vehicleTypeOptionMotorcycleButton.checked=",vehicleTypeOptionMotorcycleButton.checked)
        }
        function onVehicleTypeBicycleButtonCheckedChanged(isVehicleTypeBicycleButtonChecked){
            vehicleTypeOptionBicycleButton.checked = isVehicleTypeBicycleButtonChecked
            console.log("vehicleTypeOptionBicycleButton.checked=",vehicleTypeOptionBicycleButton.checked)
        }
        // Vehicle - Color
        function onVehicleColorBlackButtonCheckedChanged(isVehicleColorBlackButtonChecked){
            vehicleColorBlackButton.checked = isVehicleColorBlackButtonChecked
            console.log("vehicleColorBlackButton.checked=",vehicleColorBlackButton.checked)
        }
        function onVehicleColorGrayButtonCheckedChanged(isVehicleColorGrayButtonChecked){
            vehicleColorGrayButton.checked = isVehicleColorGrayButtonChecked
            console.log("vehicleColorGrayButton.checked=",vehicleColorGrayButton.checked)
        }
        function onVehicleColorWhiteButtonCheckedChanged(isVehicleColorWhiteButtonChecked){
            vehicleColorWhiteButton.checked = isVehicleColorWhiteButtonChecked
            console.log("vehicleColorWhiteButton.checked=",vehicleColorWhiteButton.checked)
        }
        function onVehicleColorRedButtonCheckedChanged(isVehicleColorRedButtonChecked){
            vehicleColorRedButton.checked = isVehicleColorRedButtonChecked
            console.log("vehicleColorRedButton.checked=",vehicleColorRedButton.checked)
        }
        function onVehicleColorOrangeButtonCheckedChanged(isVehicleColorOrangeButtonChecked){
            vehicleColorOrangeButton.checked = isVehicleColorOrangeButtonChecked
            console.log("vehicleColorOrangeButton.checked=",vehicleColorOrangeButton.checked)
        }
        function onVehicleColorYellowButtonCheckedChanged(isVehicleColorYellowButtonChecked){
            vehicleColorYellowButton.checked = isVehicleColorYellowButtonChecked
            console.log("vehicleColorYellowButton.checked=",vehicleColorYellowButton.checked)
        }
        function onVehicleColorGreenButtonCheckedChanged(isVehicleColorGreenButtonChecked){
            vehicleColorGreenButton.checked = isVehicleColorGreenButtonChecked
            console.log("vehicleColorGreenButton.checked=",vehicleColorGreenButton.checked)
        }
        function onVehicleColorBlueButtonCheckedChanged(isVehicleColorBlueButtonChecked){
            vehicleColorBlueButton.checked = isVehicleColorBlueButtonChecked
            console.log("vehicleColorBlueButton.checked=",vehicleColorBlueButton.checked)
        }
        function onVehicleColorPurpleButtonCheckedChanged(isVehicleColorPurpleButtonChecked){
            vehicleColorPurpleButton.checked = isVehicleColorPurpleButtonChecked
            console.log("vehicleColorPurpleButton.checked=",vehicleColorPurpleButton.checked)
        }
    }

    Column{
        id: searchCondition
        spacing: 0

        Rectangle{
            id: aiSearchFilterViewDeviceMargin
            height: 1
            width: aiSearchFilterView.width
            color: WisenetGui.contrast_08_dark_grey
        }

        //장치
        Rectangle{
            id: deviceRectangle
            width: aiSearchFilterView.width
            height: 68
            Layout.alignment: Qt.AlignTop
            color: WisenetGui.contrast_09_dark

            Text{
                id: deviceText
                width: parent.width - 48
                leftPadding: 24
                topPadding: 12
                elide: Text.ElideRight
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.device
                font.pixelSize: 12
            }

            //all또는 선택된 camera들 가로로 보여주기
            Text{
                id: cameraNames
                width: parent.width - 48
                leftPadding: 24
                topPadding: 39
                elide: Text.ElideRight
                color: WisenetGui.contrast_01_light_grey
                text: WisenetLinguist.all
                font.pixelSize: 12
            }


            MouseArea{
                anchors.fill: parent
                onClicked: {
                    //deviceSeletectionTree.initalize()
                    deviceViewPopup.open()
                }
            }
        }

        Rectangle{
            id: aiSearchFilterViewDateTimeMargin
            width: aiSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        // Date & Time Area
        Rectangle{
            id: dateTimeRectangle
            width: aiSearchFilterView.width
            height: 134
            color: WisenetGui.contrast_09_dark

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    calendarViewPopup.open()
                }
            }

            Text{
                id: dateTimeText
                width: parent.width - 48
                leftPadding: 24
                topPadding: 12
                elide: Text.ElideRight
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.dateAndTime
                font.pixelSize: 12
            }

            Rectangle{
                id: fromRectangle
                x: 24
                y: 40
                width: 88
                height: 76
                color: WisenetGui.contrast_09_dark

                WisenetStateImageButton{
                    id: calendarViewPopupButton
                    x: 0
                    y: 0
                    width: 17
                    height: 17
                    defaultImageSource: WisenetImage.calendar_Default
                    hoverImageSource: WisenetImage.calendar_Hover
                    selectImageSource: WisenetImage.calendar_Select
                    disableImageSource: WisenetImage.calendar_Disable

                    onClicked: {
                        calendarViewPopup.open()
                    }
                }

                Text{
                    id: fromText
                    y:2
                    width: 63
                    leftPadding: 23
                    topPadding: 0
                    elide: Text.ElideRight
                    color: WisenetGui.contrast_04_light_grey
                    text: WisenetLinguist.from
                    font.pixelSize: 12
                }

                WisenetDateEditor{
                    id: aiSearchFromDateEditor
                    x: 7
                    y: 26
                    width: 82

                    onControlFocusChanged: {
                        if(!controlFocus){
                            updateCalendarLimitDate()
                            var fromDate = new Date(year,month-1,day,0,0,0)
                            var toDate = new Date(aiSearchToDateEditor.year,
                                                  aiSearchToDateEditor.month-1,
                                                  aiSearchToDateEditor.day,
                                                  0,0,0)
                            if(fromDate.getTime() > calendarControlView.maximumDate.getTime()){
                                fromDate = calendarControlView.maximumDate; fromDate.setHours(0);fromDate.setMinutes(0);fromDate.setSeconds(0);
                                setDate(fromDate)
                            }
                            if(fromDate.getTime() < calendarControlView.minimumDate.getTime()){
                                fromDate = calendarControlView.minimumDate; fromDate.setHours(0);fromDate.setMinutes(0);fromDate.setSeconds(0);
                                setDate(fromDate)
                            }

                            if(fromDate.getTime() > toDate.getTime()){
                                aiSearchToDateEditor.setDate(fromDate)
                            }

                            if(fromDate.getTime() === toDate.getTime()){
                                var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                if(ftime > ttime){
                                    toTime.setTimeText(fromTime.hour, fromTime.minute,fromTime.second)
                                }
                            }

                            updateCalendar();

                        }
                        console.debug("conrtolFocus changed: " + controlFocus)
                    }
                }

                Rectangle{
                    x: 0
                    y: 42
                    width: fromRectangle.width
                    height: 1
                    color: WisenetGui.contrast_06_grey
                }

                Rectangle{
                    x: 0
                    y: 55
                    width: fromRectangle.width
                    height: 21
                    color: WisenetGui.contrast_09_dark
                    border.color: WisenetGui.contrast_06_grey
                    border.width: 1

                    WisenetTimeEditor{
                        id: fromTime
                        anchors.leftMargin: 16
                        anchors.topMargin: 4
                        anchors.fill: parent
                        buttonVisible: false

                        onControlFocusChanged: {
                            if(!controlFocus){

                                var fromDate = new Date(aiSearchFromDateEditor.year,
                                                        aiSearchFromDateEditor.month-1,
                                                        aiSearchFromDateEditor.day,
                                                        0,0,0)
                                var toDate = new Date(aiSearchToDateEditor.year,
                                                      aiSearchToDateEditor.month-1,
                                                      aiSearchToDateEditor.day,
                                                      0,0,0)

                                if(fromDate.getTime() === toDate.getTime()){
                                    var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                    var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                    if(ftime > ttime){
                                        toTime.setTimeText(fromTime.hour, fromTime.minute,fromTime.second)
                                    }
                                }
                            }
                        }
                    }
                }

            }

            Rectangle{
                id: toRectangle
                x: 130
                y: 40
                width: 88
                height: 76
                color: WisenetGui.contrast_09_dark

                Text{
                    id: toText
                    y:2
                    width: 63
                    leftPadding: 0
                    topPadding: 0
                    elide: Text.ElideRight
                    color: WisenetGui.contrast_04_light_grey
                    text: WisenetLinguist.to
                    font.pixelSize: 12
                }

                WisenetDateEditor{
                    id: aiSearchToDateEditor
                    x: 7
                    y: 26
                    width: 82

                    onControlFocusChanged: {
                        if(!controlFocus){
                            updateCalendarLimitDate()

                            var toDate = new Date(year,month-1,day,0,0,0)
                            var fromDate = new Date(aiSearchFromDateEditor.year,
                                                    aiSearchFromDateEditor.month-1,
                                                    aiSearchFromDateEditor.day,
                                                    0,0,0)

                            if(toDate.getTime() > calendarControlView.maximumDate.getTime()){
                                console.debug("toDate is more then maximumDate. toDate:" + toDate + ", maximumDate" + calendarControlView.maximumDate)
                                toDate = calendarControlView.maximumDate; toDate.setHours(0);toDate.setMinutes(0);toDate.setSeconds(0);
                                setDate(toDate)
                            }
                            if(toDate.getTime() < calendarControlView.minimumDate.getTime()){
                                toDate = calendarControlView.minimumDate; toDate.setHours(0);toDate.setMinutes(0);toDate.setSeconds(0);
                                setDate(toDate)
                            }

                            if(fromDate.getTime() > toDate.getTime()){
                                aiSearchFromDateEditor.setDate(toDate)
                            }

                            if(fromDate.getTime() === toDate.getTime()){
                                var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                if(ftime > ttime){
                                    fromTime.setTimeText(toTime.hour, toTime.minute,toTime.second)
                                }
                            }

                            updateCalendar();
                        }
                    }


                }

                Rectangle{
                    x: 0
                    y: 42
                    width: toRectangle.width
                    height: 1
                    color: WisenetGui.contrast_06_grey
                }

                Rectangle{
                    x: 0
                    y: 55
                    width: toRectangle.width
                    height: 21
                    color: WisenetGui.contrast_09_dark
                    border.color: WisenetGui.contrast_06_grey
                    border.width: 1

                    WisenetTimeEditor{
                        id: toTime
                        anchors.leftMargin: 16
                        anchors.topMargin: 4
                        anchors.fill: parent
                        buttonVisible: false

                        onControlFocusChanged: {
                            if(!controlFocus){

                                var fromDate = new Date(aiSearchFromDateEditor.year,
                                                        aiSearchFromDateEditor.month-1,
                                                        aiSearchFromDateEditor.day,
                                                        0,0,0)
                                var toDate = new Date(aiSearchToDateEditor.year,
                                                      aiSearchToDateEditor.month-1,
                                                      aiSearchToDateEditor.day,
                                                      0,0,0)

                                if(fromDate.getTime() === toDate.getTime()){
                                    var ftime = fromTime.hour*3600 + fromTime.minute*60 + fromTime.second;
                                    var ttime = toTime.hour*3600 + toTime.minute*60 + toTime.second;

                                    if(ftime > ttime){
                                        fromTime.setTimeText(toTime.hour, toTime.minute,toTime.second)
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }

        Rectangle{
            id: aiSearchFilterViewObjectTypeMargin
            width: aiSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id: objectTypeRectangle
            width: aiSearchFilterView.width
            height: 85
            color: WisenetGui.transparent

            Text{
                id: objectTypeText
                //width: 40
                height: 14
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 14
                anchors.leftMargin: 24
                color: WisenetGui.contrast_04_light_grey
                text: WisenetLinguist.objectType
                font.pixelSize: 12
            }

            ShortcutButton{
                id: objectTypeInfoTooltip
                anchors.top: parent.top
                anchors.left: objectTypeText.right
                anchors.topMargin: 16
                anchors.leftMargin: 4
                sourceHeight: 16
                sourceWidth: 16
                height: 16
                width: 16
                checkable: false

                imageNormal: WisenetImage.object_search_filter_info_default
                imageHover: WisenetImage.object_search_filter_info_hover

                tooltipText: WisenetLinguist.objectTypeInfo
            }

            Rectangle{
                id: objectTypeList
                height: 28
                anchors.top: objectTypeText.bottom
                anchors.left: parent.left
                anchors.topMargin: 14
                color: WisenetGui.contrast_09_dark

                function reset() {
                    classType = ObjectSearchFilterView.AiClassType.Person

                    personObjectButton.checked = true
                    faceObjectButton.checked = false
                    vehicleObjectButton.checked = false
                    ocrObjectButton.checked = false

                    personObjectRectLayout.visible = false
                    faceObjectRectLayout.visible = false
                    vehicleObjectRectLayout.visible = false
                    ocrObjectRectLayout.visible = false
                }

                ShortcutButton{
                    id: personObjectButton

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    checkable: true
                    sourceHeight: 28
                    sourceWidth: 28
                    height: 28
                    width: 28

                    imageNormal: WisenetImage.object_search_filter_person_default
                    imageHover: WisenetImage.object_search_filter_person_hover
                    imageSelected: WisenetImage.object_search_filter_person_selected

                    tooltipText: WisenetLinguist.person

                    onClicked: {
                        console.log("personObjectButton : checked=",personObjectButton.checked);

                        classType = ObjectSearchFilterView.AiClassType.Person
                        clearAllObjectButton()
                        personObjectButton.checked = true
                    }
                }

                ShortcutButton{
                    id: faceObjectButton

                    anchors.top: parent.top
                    anchors.left: personObjectButton.right
                    anchors.leftMargin: 8
                    checkable: true
                    sourceHeight: 28
                    sourceWidth: 28
                    height: 28
                    width: 28

                    imageNormal: WisenetImage.object_search_filter_face_default
                    imageHover: WisenetImage.object_search_filter_face_hover
                    imageSelected: WisenetImage.object_search_filter_face_selected

                    tooltipText: WisenetLinguist.face

                    onClicked: {
                        console.log("faceObjectButton : checked=",faceObjectButton.checked);

                        classType = ObjectSearchFilterView.AiClassType.Face
                        clearAllObjectButton()
                        faceObjectButton.checked = true
                    }
                }

                ShortcutButton{
                    id: vehicleObjectButton

                    anchors.top: parent.top
                    anchors.left: faceObjectButton.right
                    anchors.leftMargin: 8
                    checkable: true
                    sourceHeight: 28
                    sourceWidth: 28
                    height: 28
                    width: 28

                    imageNormal: WisenetImage.object_search_filter_vehicle_default
                    imageHover: WisenetImage.object_search_filter_vehicle_hover
                    imageSelected: WisenetImage.object_search_filter_vehicle_selected

                    tooltipText: WisenetLinguist.vehicle

                    onClicked: {
                        console.log("vehicleObjectButton : checked=",vehicleObjectButton.checked);

                        classType = ObjectSearchFilterView.AiClassType.Vehicle
                        clearAllObjectButton()
                        vehicleObjectButton.checked = true
                    }
                }

                ShortcutButton{
                    id: ocrObjectButton

                    anchors.top: parent.top
                    anchors.left: vehicleObjectButton.right
                    anchors.leftMargin: 8
                    checkable: true
                    sourceHeight: 28
                    sourceWidth: 28
                    height: 28
                    width: 28

                    imageNormal: WisenetImage.object_search_filter_ocr_default
                    imageHover: WisenetImage.object_search_filter_ocr_hover
                    imageSelected: WisenetImage.object_search_filter_ocr_selected

                    tooltipText: WisenetLinguist.ocr

                    onClicked: {
                        console.log("ocrObjectButton : checked=",ocrObjectButton.checked);

                        classType = ObjectSearchFilterView.AiClassType.Ocr
                        clearAllObjectButton()
                        ocrObjectButton.checked = true
                    }
                }
            }
        }

        Rectangle{
            id: aiSearchFilterViewSearchButtonMargin
            width: aiSearchFilterView.width
            height: 1
            color: WisenetGui.contrast_08_dark_grey
        }

        Rectangle{
            id: searchRectangle
            width: aiSearchFilterView.width
            height: 58
            color: WisenetGui.transparent

            WisenetOrangeButton{
                id: searchButton
                text: WisenetLinguist.search
                x: 24
                y: 15
                width: 119
                height: 28

                onClicked: {
                    searchByFilter()

                    objectSearchViewModel.objectSearchFilterViewModel.isObjectOptionFlickableVisible = false
                    personObjectRectLayout.visible = personObjectButton.checked
                    faceObjectRectLayout.visible = faceObjectButton.checked
                    vehicleObjectRectLayout.visible = vehicleObjectButton.checked
                    ocrObjectRectLayout.visible = ocrObjectButton.checked

                    verticalScrollbar.position = 0
                    resetListPosition()
                }
            }

            WisenetGrayButton{
                id: resetButton
                x: 153
                y: 15
                width: 66
                height: 28

                Text {
                    id: resetButtonText
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 17
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: lineCount == 1 ? Text.AlignHCenter : Text.AlignLeft
                    width: 66
                    height: 24
                    color: WisenetGui.contrast_00_white
                    font.pixelSize: 12

                    fontSizeMode: Text.HorizontalFit
                    minimumPixelSize: 8
                    wrapMode: Text.WordWrap
                    text: WisenetLinguist.reset
                }

                onClicked: {
                    aiSearchFilterView.reset()
                }
            }

        }

        Rectangle{
            id: divider
            width: aiSearchFilterView.width
            height: 1
            color: WisenetGui.transparent
        }

        Rectangle {
            id: objectOptionRect
            width: aiSearchFilterView.width
            height: aiSearchFilterView.height - aiSearchFilterViewDeviceMargin.height - deviceRectangle.height
                    - aiSearchFilterViewDateTimeMargin.height - dateTimeRectangle.height - aiSearchFilterViewObjectTypeMargin.height
                    - objectTypeRectangle.height - aiSearchFilterViewSearchButtonMargin.height - searchRectangle.height - divider.height
            color: WisenetGui.transparent

            function reset() {
                objectSearchViewModel.objectSearchFilterViewModel.ocrLicensePlateText = "";
                ocrObjectContentLicensePlateTextBox.text = "";
            }

            Flickable {
                id: objectOption
                anchors.fill: objectOptionRect
                anchors.leftMargin: 24
                anchors.rightMargin: 9
                contentWidth: aiSearchFilterView.width - 33
                contentHeight: 450

                visible: objectSearchViewModel.objectSearchFilterViewModel.isObjectOptionFlickableVisible
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: WisenetScrollBar {
                    id: verticalScrollbar
                    anchors.right: objectOption.right
                }

                // 사람
                Rectangle {
                    id: personObjectRectLayout
                    visible: false
                    width: filterdefaultWidth
                    height: personObjectTitleRect.height + personObjectContentRect.height

                    Rectangle {
                        id: personObjectTitleRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_10_dark
                        width: filterdefaultWidth
                        height: 31
                        anchors.top: personObjectRectLayout.top
                        anchors.left: personObjectRectLayout.left
                        anchors.right: personObjectRectLayout.right

                        ShortcutButton{
                            id: personObjectIcon
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: personObjectTitleRect.left
                            anchors.leftMargin: 11
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20
                            checkable: false

                            imageNormal: WisenetImage.object_search_filter_person_title
                        }

                        Text {
                            id: personObjectTitleText
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: personObjectIcon.right
                            anchors.topMargin: 5
                            anchors.leftMargin: 12
                            text: WisenetLinguist.person
                            color: WisenetGui.contrast_02_light_grey
                            font.pixelSize: 12

                        }

                        ShortcutButton{
                            id: personObjectClearButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: personObjectFoldingButton.left
                            anchors.rightMargin: 9
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20

                            imageNormal: WisenetImage.object_search_filter_clear_default
                            imageHover: WisenetImage.object_search_filter_clear_hover
                            imageSelected: WisenetImage.object_search_filter_clear_selected

                            onClicked: {
                                clearPersonObjectOption()
                                objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                            }
                        }

                        ShortcutButton{
                            id: personObjectFoldingButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: personObjectTitleRect.right
                            anchors.rightMargin: 14
                            sourceHeight: 12
                            sourceWidth: 12
                            height: 12
                            width: 12
                            checkable: true

                            imageNormal: WisenetImage.object_search_filter_unfold
                            imageSelected: WisenetImage.object_search_filter_fold

                            onCheckedChanged: {
                                if(personObjectContentRect.visible) {
                                    personObjectRectLayout.height -= personObjectContentRect.height
                                }
                                else {
                                    personObjectRectLayout.height += personObjectContentRect.height
                                }
                                personObjectContentRect.visible = !personObjectContentRect.visible
                                console.log("personObjectFoldingButtonClickedFunction() : personObjectContentRect.visible=",personObjectContentRect.visible)

                            }
                        }
                    }

                    Rectangle
                    {
                        id: personObjectContentRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_08_dark_grey
                        height: personObjectContentGenderRect.height + personObjectContentClothingTopRect.height + personObjectContentClothingBottomRect.height + personObjectContentBagRect.height + 21 + 35*3
                        anchors.top: personObjectTitleRect.bottom
                        anchors.left: personObjectTitleRect.left
                        anchors.right: personObjectTitleRect.right
                        visible: true

                        // 성별
                        Rectangle
                        {
                            id: personObjectContentGenderRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: personObjectContentGenderTitleText.height + personObjectGenderFlow.height//objectOptionDefaultRectHeight
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.topMargin: 14
                            anchors.leftMargin: 15
                            visible: true

                            Text {
                                id: personObjectContentGenderTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.gender
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }

                            Flow {
                                id: personObjectGenderFlow
                                anchors.top: personObjectContentGenderTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: personGenderOptionMaleButton

                                    tooltipText: WisenetLinguist.male
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderMaleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderMaleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_male_default
                                    imageHover: WisenetImage.object_search_filter_male_hover
                                    imageSelected: WisenetImage.object_search_filter_male_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_male_selected_hover

                                    onClicked: {
                                        console.log("personGenderOptionMaleButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderMaleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderMaleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personGenderOptionFemaleButton

                                    tooltipText: WisenetLinguist.female
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderFemaleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderFemaleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_female_default
                                    imageHover: WisenetImage.object_search_filter_female_hover
                                    imageSelected: WisenetImage.object_search_filter_female_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_female_selected_hover

                                    onClicked: {
                                        console.log("isPersonGenderFemaleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderFemaleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonGenderFemaleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        // 상의
                        Rectangle
                        {
                            id: personObjectContentClothingTopRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: personObjectContentClothingTopTitleText.height + personObjectClothingTopFlow.height
                            anchors.top: personObjectContentGenderRect.bottom
                            anchors.left: personObjectContentGenderRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: personObjectContentClothingTopTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.clothing + " (" + WisenetLinguist.top + ")"
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: personObjectClothingTopFlow
                                anchors.top: personObjectContentClothingTopTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: personClothingTopBlackButton

                                    tooltipText: WisenetLinguist.black
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlackButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlackButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_black_default
                                    imageHover: WisenetImage.object_search_filter_color_black_hover
                                    imageSelected: WisenetImage.object_search_filter_color_black_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_black_hover

                                    onClicked: {
                                        console.log("personClothingTopBlackButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlackButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlackButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopGrayButton

                                    tooltipText: WisenetLinguist.gray
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGrayButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGrayButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_gray_default
                                    imageHover: WisenetImage.object_search_filter_color_gray_hover
                                    imageSelected: WisenetImage.object_search_filter_color_gray_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_gray_hover

                                    onClicked: {
                                        console.log("personClothingTopGrayButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGrayButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGrayButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopWhiteButton

                                    tooltipText: WisenetLinguist.white
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopWhiteButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopWhiteButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_white_default
                                    imageHover: WisenetImage.object_search_filter_color_white_hover
                                    imageSelected: WisenetImage.object_search_filter_color_white_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_white_hover

                                    onClicked: {
                                        console.log("personClothingTopWhiteButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopWhiteButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopWhiteButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopRedButton

                                    tooltipText: WisenetLinguist.red
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopRedButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopRedButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_red_default
                                    imageHover: WisenetImage.object_search_filter_color_red_hover
                                    imageSelected: WisenetImage.object_search_filter_color_red_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_red_hover

                                    onClicked: {
                                        console.log("personClothingTopRedButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopRedButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopRedButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopOrangeButton

                                    tooltipText: WisenetLinguist.orange
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopOrangeButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopOrangeButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_orange_default
                                    imageHover: WisenetImage.object_search_filter_color_orange_hover
                                    imageSelected: WisenetImage.object_search_filter_color_orange_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_orange_hover

                                    onClicked: {
                                        console.log("personClothingTopOrangeButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopOrangeButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopOrangeButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopYellowButton

                                    tooltipText: WisenetLinguist.yellow
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopYellowButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopYellowButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_yellow_default
                                    imageHover: WisenetImage.object_search_filter_color_yellow_hover
                                    imageSelected: WisenetImage.object_search_filter_color_yellow_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_yellow_hover

                                    onClicked: {
                                        console.log("personClothingTopYellowButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopYellowButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopYellowButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopGreenButton

                                    tooltipText: WisenetLinguist.green
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGreenButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGreenButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_green_default
                                    imageHover: WisenetImage.object_search_filter_color_green_hover
                                    imageSelected: WisenetImage.object_search_filter_color_green_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_green_hover

                                    onClicked: {
                                        console.log("personClothingTopGreenButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGreenButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopGreenButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopBlueButton

                                    tooltipText: WisenetLinguist.blue
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlueButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlueButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_blue_default
                                    imageHover: WisenetImage.object_search_filter_color_blue_hover
                                    imageSelected: WisenetImage.object_search_filter_color_blue_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_blue_hover

                                    onClicked: {
                                        console.log("personClothingTopBlueButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlueButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopBlueButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingTopPurpleButton

                                    tooltipText: WisenetLinguist.purple
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopPurpleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopPurpleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_purple_default
                                    imageHover: WisenetImage.object_search_filter_color_purple_hover
                                    imageSelected: WisenetImage.object_search_filter_color_purple_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_purple_hover

                                    onClicked: {
                                        console.log("personClothingTopPurpleButton.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopPurpleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingTopPurpleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        //하의
                        Rectangle
                        {
                            id: personObjectContentClothingBottomRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: personObjectContentClothingBottomTitleText.height + personObjectClothingBottomFlow.height
                            anchors.top: personObjectContentClothingTopRect.bottom
                            anchors.left: personObjectContentClothingTopRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: personObjectContentClothingBottomTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.clothing + " (" + WisenetLinguist.bottom +")"
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: personObjectClothingBottomFlow
                                anchors.top: personObjectContentClothingBottomTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: personClothingBottomBlackButton
                                    tooltipText: WisenetLinguist.black
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlackButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlackButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_black_default
                                    imageHover: WisenetImage.object_search_filter_color_black_hover
                                    imageSelected: WisenetImage.object_search_filter_color_black_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_black_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomBlackButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlackButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlackButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomGrayButton
                                    tooltipText: WisenetLinguist.gray
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGrayButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGrayButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_gray_default
                                    imageHover: WisenetImage.object_search_filter_color_gray_hover
                                    imageSelected: WisenetImage.object_search_filter_color_gray_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_gray_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomGrayButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGrayButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGrayButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomWhiteButton
                                    tooltipText: WisenetLinguist.white
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomWhiteButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomWhiteButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_white_default
                                    imageHover: WisenetImage.object_search_filter_color_white_hover
                                    imageSelected: WisenetImage.object_search_filter_color_white_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_white_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomWhiteButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomWhiteButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomWhiteButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomRedButton
                                    tooltipText: WisenetLinguist.red
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomRedButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomRedButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_red_default
                                    imageHover: WisenetImage.object_search_filter_color_red_hover
                                    imageSelected: WisenetImage.object_search_filter_color_red_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_red_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomRedButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomRedButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomRedButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomOrangeButton
                                    tooltipText: WisenetLinguist.orange
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomOrangeButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomOrangeButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_orange_default
                                    imageHover: WisenetImage.object_search_filter_color_orange_hover
                                    imageSelected: WisenetImage.object_search_filter_color_orange_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_orange_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomOrangeButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomOrangeButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomOrangeButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomYellowButton
                                    tooltipText: WisenetLinguist.yellow
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomYellowButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomYellowButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_yellow_default
                                    imageHover: WisenetImage.object_search_filter_color_yellow_hover
                                    imageSelected: WisenetImage.object_search_filter_color_yellow_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_yellow_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomYellowButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomYellowButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomYellowButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomGreenButton
                                    tooltipText: WisenetLinguist.green
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGreenButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGreenButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_green_default
                                    imageHover: WisenetImage.object_search_filter_color_green_hover
                                    imageSelected: WisenetImage.object_search_filter_color_green_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_green_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomGreenButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGreenButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomGreenButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomBlueButton
                                    tooltipText: WisenetLinguist.blue
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlueButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlueButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_blue_default
                                    imageHover: WisenetImage.object_search_filter_color_blue_hover
                                    imageSelected: WisenetImage.object_search_filter_color_blue_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_blue_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomBlueButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlueButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomBlueButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personClothingBottomPurpleButton
                                    tooltipText: WisenetLinguist.purple
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomPurpleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomPurpleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_purple_default
                                    imageHover: WisenetImage.object_search_filter_color_purple_hover
                                    imageSelected: WisenetImage.object_search_filter_color_purple_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_purple_hover

                                    onClicked: {
                                        console.log("isPersonClothingBottomPurpleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomPurpleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonClothingBottomPurpleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        //가방
                        Rectangle
                        {
                            id: personObjectContentBagRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: personObjectContentBagTitleText.height + personObjectBagFlow.height
                            anchors.top: personObjectContentClothingBottomRect.bottom
                            anchors.left: personObjectContentClothingBottomRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: personObjectContentBagTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.bag
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: personObjectBagFlow
                                anchors.top: personObjectContentBagTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: personBagOptionWearButton
                                    tooltipText: WisenetLinguist.bag
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonBagWearButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonBagWearButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_bag_default
                                    imageHover: WisenetImage.object_search_filter_bag_hover
                                    imageSelected: WisenetImage.object_search_filter_bag_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_bag_selected_hover

                                    onClicked: {
                                        console.log("isPersonBagWearButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonBagWearButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonBagWearButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: personBagOptionNoWearButton
                                    tooltipText: WisenetLinguist.noBag
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isPersonBagNoWearButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isPersonBagNoWearButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_no_bag_default
                                    imageHover: WisenetImage.object_search_filter_no_bag_hover
                                    imageSelected: WisenetImage.object_search_filter_no_bag_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_no_bag_selected_hover

                                    onClicked: {
                                        console.log("isPersonBagNoWearButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isPersonBagNoWearButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isPersonBagNoWearButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }
                    }
                }

                // 얼굴
                Rectangle {
                    id: faceObjectRectLayout
                    visible: false
                    width: filterdefaultWidth
                    height: faceObjectTitleRect.height + faceObjectContentRect.height

                    Rectangle {
                        id: faceObjectTitleRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_10_dark
                        width: filterdefaultWidth
                        height: 31
                        anchors.top: faceObjectRectLayout.top
                        anchors.left: faceObjectRectLayout.left
                        anchors.right: faceObjectRectLayout.right

                        ShortcutButton {
                            id: faceObjectIcon
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: faceObjectTitleRect.left
                            anchors.leftMargin: 11
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20
                            checkable: false

                            imageNormal: WisenetImage.object_search_filter_face_title
                        }

                        Text {
                            id: faceObjectTitleText
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: faceObjectIcon.right
                            anchors.leftMargin: 12
                            text: WisenetLinguist.face
                            color: WisenetGui.contrast_02_light_grey
                            font.pixelSize: 12
                        }

                        ShortcutButton{
                            id: faceObjectClearButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: faceObjectFoldingButton.left
                            anchors.rightMargin: 9
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20
                            checkable: true

                            imageNormal: WisenetImage.object_search_filter_clear_default
                            imageHover: WisenetImage.object_search_filter_clear_hover
                            imageSelected: WisenetImage.object_search_filter_clear_selected

                            onClicked: {
                                clearFaceObjectOption()
                                objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                            }
                        }

                        ShortcutButton{
                            id: faceObjectFoldingButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: faceObjectTitleRect.right
                            anchors.rightMargin: 14
                            sourceHeight: 12
                            sourceWidth: 12
                            height: 12
                            width: 12
                            checkable: true

                            imageNormal: WisenetImage.object_search_filter_unfold
                            imageSelected: WisenetImage.object_search_filter_fold

                            onCheckedChanged: {
                                if(faceObjectContentRect.visible) {
                                    faceObjectRectLayout.height -= faceObjectContentRect.height
                                }
                                else {
                                    faceObjectRectLayout.height += faceObjectContentRect.height
                                }
                                faceObjectContentRect.visible = !faceObjectContentRect.visible
                                console.log("faceObjectFoldingButtonClickedFunction() : faceObjectContentRect.visible=",faceObjectContentRect.visible)

                            }
                        }
                    }

                    Rectangle
                    {
                        id: faceObjectContentRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_09_dark
                        height: faceObjectContentGenderRect.height + faceObjectContentAgeTypeRect.height
                                + faceObjectContentOpticalsRect.height + faceObjectContentMaskRect.height + 21 + 35*3
                        anchors.top: faceObjectTitleRect.bottom
                        anchors.left: faceObjectTitleRect.left
                        anchors.right: faceObjectTitleRect.right
                        visible: true

                        // 성별 : Male, Female
                        Rectangle
                        {
                            id: faceObjectContentGenderRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: faceObjectContentGenderTitleText.height + faceObjectGenderFlow.height//objectOptionDefaultRectHeight
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.topMargin: 14
                            anchors.leftMargin: 15
                            visible: true

                            Text {
                                id: faceObjectContentGenderTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.gender
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }


                            Flow {
                                id: faceObjectGenderFlow
                                anchors.top: faceObjectContentGenderTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: faceGenderOptionMaleButton
                                    tooltipText: WisenetLinguist.male
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderMaleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderMaleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_male_default
                                    imageHover: WisenetImage.object_search_filter_male_hover
                                    imageSelected: WisenetImage.object_search_filter_male_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_male_selected_hover

                                    onClicked: {
                                        console.log("isFaceGenderMaleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderMaleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderMaleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: faceGenderOptionFemaleButton
                                    tooltipText: WisenetLinguist.female
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderFemaleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderFemaleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_female_default
                                    imageHover: WisenetImage.object_search_filter_female_hover
                                    imageSelected: WisenetImage.object_search_filter_female_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_female_selected_hover

                                    onClicked: {
                                        console.log("isFaceGenderFemaleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderFemaleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceGenderFemaleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        // 나이대 : Young, Adult, Middle, Senior
                        Rectangle
                        {
                            id: faceObjectContentAgeTypeRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: faceObjectContentAgeTypeTitleText.height + faceObjectAgeTypeFlow.height
                            anchors.top: faceObjectContentGenderRect.bottom
                            anchors.left: faceObjectContentGenderRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: faceObjectContentAgeTypeTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.age
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: faceObjectAgeTypeFlow
                                anchors.top: faceObjectContentAgeTypeTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: faceAgeTypeYoungButton
                                    tooltipText: WisenetLinguist.young
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeYoungButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeYoungButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_young_default
                                    imageHover: WisenetImage.object_search_filter_young_hover
                                    imageSelected: WisenetImage.object_search_filter_young_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_young_selected_hover

                                    onClicked: {
                                        console.log("isFaceAgeTypeYoungButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeYoungButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeYoungButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: faceAgeTypeAdultButton
                                    tooltipText: WisenetLinguist.adult
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeAdultButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeAdultButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_adult_default
                                    imageHover: WisenetImage.object_search_filter_adult_hover
                                    imageSelected: WisenetImage.object_search_filter_adult_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_adult_selected_hover

                                    onClicked: {
                                        console.log("isFaceAgeTypeAdultButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeAdultButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeAdultButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: faceAgeTypeMiddleButton
                                    tooltipText: WisenetLinguist.middle
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeMiddleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeMiddleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_middle_default
                                    imageHover: WisenetImage.object_search_filter_middle_hover
                                    imageSelected: WisenetImage.object_search_filter_middle_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_middle_selected_hover

                                    onClicked: {
                                        console.log("isFaceAgeTypeMiddleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeMiddleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeMiddleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: faceAgeTypeSeniorButton
                                    tooltipText: WisenetLinguist.senior
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeSeniorButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeSeniorButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_senior_default
                                    imageHover: WisenetImage.object_search_filter_senior_hover
                                    imageSelected: WisenetImage.object_search_filter_senior_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_senior_selected_hover

                                    onClicked: {
                                        console.log("isFaceAgeTypeSeniorButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeSeniorButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceAgeTypeSeniorButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        // Opticals : Wear, No
                        Rectangle
                        {
                            id: faceObjectContentOpticalsRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: faceObjectContentOpticalsTitleText.height + faceObjectOpticalsFlow.height
                            anchors.top: faceObjectContentAgeTypeRect.bottom
                            anchors.left: faceObjectContentAgeTypeRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: faceObjectContentOpticalsTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.glasses
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: faceObjectOpticalsFlow
                                anchors.top: faceObjectContentOpticalsTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: faceOpticalsOptionWearButton
                                    tooltipText: WisenetLinguist.glasses
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsWearButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsWearButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_glasses_default
                                    imageHover: WisenetImage.object_search_filter_glasses_hover
                                    imageSelected: WisenetImage.object_search_filter_glasses_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_glasses_selected_hover

                                    onClicked: {
                                        console.log("isFaceOpticalsWearButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsWearButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsWearButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: faceOpticalsOptionNoWearButton
                                    tooltipText: WisenetLinguist.noGlasses
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsNoWearButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsNoWearButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_no_glasses_default
                                    imageHover: WisenetImage.object_search_filter_no_glasses_hover
                                    imageSelected: WisenetImage.object_search_filter_no_glasses_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_no_glasses_selected_hover

                                    onClicked: {
                                        console.log("isFaceOpticalsNoWearButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsNoWearButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceOpticalsNoWearButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        // Mask : Wear, No
                        Rectangle
                        {
                            id: faceObjectContentMaskRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: faceObjectContentMaskTitleText.height + faceObjectMaskFlow.height
                            anchors.top: faceObjectContentOpticalsRect.bottom
                            anchors.left: faceObjectContentOpticalsRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: faceObjectContentMaskTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.faceMask
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: faceObjectMaskFlow
                                anchors.top: faceObjectContentMaskTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: faceMaskOptionWearButton
                                    tooltipText: WisenetLinguist.faceMask
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskWearButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskWearButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_mask_default
                                    imageHover: WisenetImage.object_search_filter_mask_hover
                                    imageSelected: WisenetImage.object_search_filter_mask_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_mask_selected_hover

                                    onClicked: {
                                        console.log("isFaceMaskWearButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskWearButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskWearButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: faceMaskOptionNoWearButton
                                    tooltipText: WisenetLinguist.noFaceMask
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskNoWearButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskNoWearButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_no_mask_default
                                    imageHover: WisenetImage.object_search_filter_no_mask_hover
                                    imageSelected: WisenetImage.object_search_filter_no_mask_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_no_mask_selected_hover

                                    onClicked: {
                                        console.log("isFaceMaskNoWearButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskNoWearButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isFaceMaskNoWearButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }
                    }
                }

                // 차량
                Rectangle {
                    id: vehicleObjectRectLayout
                    visible: false
                    width: filterdefaultWidth
                    height: vehicleObjectTitleRect.height + vehicleObjectContentRect.height

                    Rectangle {
                        id: vehicleObjectTitleRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_10_dark
                        width: filterdefaultWidth
                        height: 31
                        anchors.top: vehicleObjectRectLayout.top
                        anchors.left: vehicleObjectRectLayout.left
                        anchors.right: vehicleObjectRectLayout.right

                        ShortcutButton{
                            id: vehicleObjectIcon
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: vehicleObjectTitleRect.left
                            anchors.leftMargin: 11
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20
                            checkable: false

                            imageNormal: WisenetImage.object_search_filter_vehicle_title
                        }

                        Text {
                            id: vehicleObjectTitleText
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: vehicleObjectIcon.right
                            anchors.leftMargin: 12
                            text: WisenetLinguist.vehicle
                            color: WisenetGui.contrast_02_light_grey
                            font.pixelSize: 12
                        }

                        ShortcutButton{
                            id: vehicleObjectClearButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: vehicleObjectFoldingButton.left
                            anchors.rightMargin: 9
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20

                            imageNormal: WisenetImage.object_search_filter_clear_default
                            imageHover: WisenetImage.object_search_filter_clear_hover
                            imageSelected: WisenetImage.object_search_filter_clear_selected

                            onClicked: {
                                clearVehicleObjectOption()
                                objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                            }
                        }

                        ShortcutButton{
                            id: vehicleObjectFoldingButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: vehicleObjectTitleRect.right
                            anchors.rightMargin: 14
                            sourceHeight: 12
                            sourceWidth: 12
                            height: 12
                            width: 12
                            checkable: true

                            imageNormal: WisenetImage.object_search_filter_unfold
                            imageSelected: WisenetImage.object_search_filter_fold

                            onCheckedChanged: {
                                if(vehicleObjectContentRect.visible) {
                                    vehicleObjectRectLayout.height -= vehicleObjectContentRect.height
                                }
                                else {
                                    vehicleObjectRectLayout.height += vehicleObjectContentRect.height
                                }
                                vehicleObjectContentRect.visible = !vehicleObjectContentRect.visible
                                console.log("vehicleObjectFoldingButtonClickedFunction() : vehicleObjectContentRect.visible=",vehicleObjectContentRect.visible)

                            }
                        }
                    }

                    Rectangle
                    {
                        id: vehicleObjectContentRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_08_dark_grey
                        height: vehicleObjectContentTypeRect.height + vehicleObjectContentColorRect.height + 21 + 35
                        anchors.top: vehicleObjectTitleRect.bottom
                        anchors.left: vehicleObjectTitleRect.left
                        anchors.right: vehicleObjectTitleRect.right
                        visible: true

                        // 차량 타입 : Any, Car, Bus, Truck, Motorcycle, Bicycle
                        Rectangle
                        {
                            id: vehicleObjectContentTypeRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: vehicleObjectContentTypeTitleText.height + vehicleObjectTypeFlow.height
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.topMargin: 14
                            anchors.leftMargin: 15
                            visible: true

                            Text {
                                id: vehicleObjectContentTypeTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.type
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }


                            Flow {
                                id: vehicleObjectTypeFlow
                                anchors.top: vehicleObjectContentTypeTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: vehicleTypeOptionCarButton
                                    tooltipText: WisenetLinguist.car
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeCarButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeCarButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_car_default
                                    imageHover: WisenetImage.object_search_filter_car_hover
                                    imageSelected: WisenetImage.object_search_filter_car_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_car_selected_hover

                                    onClicked: {
                                        console.log("isVehicleTypeCarButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeCarButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeCarButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleTypeOptionBusButton
                                    tooltipText: WisenetLinguist.bus
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBusButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBusButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_bus_default
                                    imageHover: WisenetImage.object_search_filter_bus_hover
                                    imageSelected: WisenetImage.object_search_filter_bus_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_bus_selected_hover

                                    onClicked: {
                                        console.log("isVehicleTypeBusButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBusButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBusButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleTypeOptionTruckButton
                                    tooltipText: WisenetLinguist.truck
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeTruckButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeTruckButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_truck_default
                                    imageHover: WisenetImage.object_search_filter_truck_hover
                                    imageSelected: WisenetImage.object_search_filter_truck_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_truck_selected_hover

                                    onClicked: {
                                        console.log("isVehicleTypeTruckButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeTruckButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeTruckButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleTypeOptionMotorcycleButton
                                    tooltipText: WisenetLinguist.motorcycle
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeMotorcycleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeMotorcycleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_motorcycle_default
                                    imageHover: WisenetImage.object_search_filter_motorcycle_hover
                                    imageSelected: WisenetImage.object_search_filter_motorcycle_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_motorcycle_selected_hover

                                    onClicked: {
                                        console.log("isVehicleTypeMotorcycleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeMotorcycleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeMotorcycleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleTypeOptionBicycleButton
                                    tooltipText: WisenetLinguist.bicycle
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBicycleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBicycleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_bicycle_default
                                    imageHover: WisenetImage.object_search_filter_bicycle_hover
                                    imageSelected: WisenetImage.object_search_filter_bicycle_selected
                                    imageSelectedHover: WisenetImage.object_search_filter_bicycle_selected_hover

                                    onClicked: {
                                        console.log("isVehicleTypeBicycleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBicycleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleTypeBicycleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }

                        // 차량 색상 : Any, Black, Gray, White, Red, Orange, Yellow, Green, Blue, Purple
                        Rectangle
                        {
                            id: vehicleObjectContentColorRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: vehicleObjectContentColorTitleText.height + vehicleObjectColorFlow.height
                            anchors.top: vehicleObjectContentTypeRect.bottom
                            anchors.left: vehicleObjectContentTypeRect.left
                            anchors.topMargin: 28
                            visible: true

                            Text {
                                id: vehicleObjectContentColorTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.color
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }
                            Flow {
                                id: vehicleObjectColorFlow
                                anchors.top: vehicleObjectContentColorTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.topMargin: 7
                                anchors.rightMargin: 15

                                ObjectOptionButton{
                                    id: vehicleColorBlackButton
                                    tooltipText: WisenetLinguist.black
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlackButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlackButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_black_default
                                    imageHover: WisenetImage.object_search_filter_color_black_hover
                                    imageSelected: WisenetImage.object_search_filter_color_black_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_black_hover

                                    onClicked: {
                                        console.log("isVehicleColorBlackButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlackButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlackButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorGrayButton
                                    tooltipText: WisenetLinguist.gray
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGrayButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGrayButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_gray_default
                                    imageHover: WisenetImage.object_search_filter_color_gray_hover
                                    imageSelected: WisenetImage.object_search_filter_color_gray_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_gray_hover

                                    onClicked: {
                                        console.log("isVehicleColorGrayButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGrayButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGrayButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorWhiteButton
                                    tooltipText: WisenetLinguist.white
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorWhiteButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorWhiteButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_white_default
                                    imageHover: WisenetImage.object_search_filter_color_white_hover
                                    imageSelected: WisenetImage.object_search_filter_color_white_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_white_hover

                                    onClicked: {
                                        console.log("isVehicleColorWhiteButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorWhiteButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorWhiteButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorRedButton
                                    tooltipText: WisenetLinguist.red
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorRedButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorRedButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_red_default
                                    imageHover: WisenetImage.object_search_filter_color_red_hover
                                    imageSelected: WisenetImage.object_search_filter_color_red_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_red_hover

                                    onClicked: {
                                        console.log("isVehicleColorRedButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorRedButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorRedButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorOrangeButton
                                    tooltipText: WisenetLinguist.orange
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorOrangeButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorOrangeButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_orange_default
                                    imageHover: WisenetImage.object_search_filter_color_orange_hover
                                    imageSelected: WisenetImage.object_search_filter_color_orange_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_orange_hover

                                    onClicked: {
                                        console.log("isVehicleColorOrangeButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorOrangeButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorOrangeButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorYellowButton
                                    tooltipText: WisenetLinguist.yellow
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorYellowButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorYellowButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_yellow_default
                                    imageHover: WisenetImage.object_search_filter_color_yellow_hover
                                    imageSelected: WisenetImage.object_search_filter_color_yellow_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_yellow_hover

                                    onClicked: {
                                        console.log("isVehicleColorYellowButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorYellowButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorYellowButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorGreenButton
                                    tooltipText: WisenetLinguist.green
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGreenButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGreenButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_green_default
                                    imageHover: WisenetImage.object_search_filter_color_green_hover
                                    imageSelected: WisenetImage.object_search_filter_color_green_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_green_hover

                                    onClicked: {
                                        console.log("isVehicleColorGreenButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGreenButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorGreenButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorBlueButton
                                    tooltipText: WisenetLinguist.blue
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlueButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlueButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_blue_default
                                    imageHover: WisenetImage.object_search_filter_color_blue_hover
                                    imageSelected: WisenetImage.object_search_filter_color_blue_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_blue_hover

                                    onClicked: {
                                        console.log("isVehicleColorBlueButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlueButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorBlueButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                                ObjectOptionButton{
                                    id: vehicleColorPurpleButton
                                    tooltipText: WisenetLinguist.purple
                                    visible: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorPurpleButtonVisible
                                    checked: objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorPurpleButtonChecked

                                    imageNormal: WisenetImage.object_search_filter_color_purple_default
                                    imageHover: WisenetImage.object_search_filter_color_purple_hover
                                    imageSelected: WisenetImage.object_search_filter_color_purple_default
                                    imageSelectedHover: WisenetImage.object_search_filter_color_purple_hover

                                    onClicked: {
                                        console.log("isVehicleColorPurpleButtonChecked.onClicked")
                                        objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorPurpleButtonChecked = !objectSearchViewModel.objectSearchFilterViewModel.isVehicleColorPurpleButtonChecked
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                    }
                                }
                            }
                        }
                    }
                }

                // OCR
                Rectangle {
                    id: ocrObjectRectLayout
                    visible: false
                    width: filterdefaultWidth
                    height: ocrObjectTitleRect.height + ocrObjectContentRect.height

                    Rectangle {
                        id: ocrObjectTitleRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_10_dark
                        width: filterdefaultWidth
                        height: 31
                        anchors.top: ocrObjectRectLayout.top
                        anchors.left: ocrObjectRectLayout.left
                        anchors.right: ocrObjectRectLayout.right

                        ShortcutButton{
                            id: ocrObjectIcon
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: ocrObjectTitleRect.left
                            anchors.leftMargin: 11
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20
                            checkable: false

                            imageNormal: WisenetImage.object_search_filter_lpr_title
                        }

                        Text {
                            id: ocrObjectTitleText
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: ocrObjectIcon.right
                            anchors.leftMargin: 12
                            text: WisenetLinguist.ocr
                            color: WisenetGui.contrast_02_light_grey
                            font.pixelSize: 12
                        }

                        ShortcutButton{
                            id: ocrObjectClearButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: ocrObjectFoldingButton.left
                            anchors.rightMargin: 9
                            sourceHeight: 20
                            sourceWidth: 20
                            height: 20
                            width: 20

                            imageNormal: WisenetImage.object_search_filter_clear_default
                            imageHover: WisenetImage.object_search_filter_clear_hover
                            imageSelected: WisenetImage.object_search_filter_clear_selected

                            onClicked: {
                                clearOcrObjectOption()
                                objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                            }
                        }

                        ShortcutButton{
                            id: ocrObjectFoldingButton
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: ocrObjectTitleRect.right
                            anchors.rightMargin: 9
                            sourceHeight: 12
                            sourceWidth: 12
                            height: 12
                            width: 12
                            checkable: true

                            imageNormal: WisenetImage.object_search_filter_unfold
                            imageSelected: WisenetImage.object_search_filter_fold

                            onCheckedChanged: {
                                if(ocrObjectContentRect.visible) {
                                    ocrObjectRectLayout.height -= ocrObjectContentRect.height
                                }
                                else {
                                    ocrObjectRectLayout.height += ocrObjectContentRect.height
                                }
                                ocrObjectContentRect.visible = !ocrObjectContentRect.visible
                                console.log("ocrObjectFoldingButtonClickedFunction() : ocrObjectContentRect.visible=",ocrObjectContentRect.visible)

                            }
                        }
                    }

                    Rectangle
                    {
                        id: ocrObjectContentRect
                        border.width: 1
                        border.color: WisenetGui.contrast_06_grey
                        color: WisenetGui.contrast_08_dark_grey
                        height: ocrObjectContentLicensePlateRect.height
                        anchors.top: ocrObjectTitleRect.bottom
                        anchors.left: ocrObjectTitleRect.left
                        anchors.right: ocrObjectTitleRect.right
                        visible: true

                        // 번호판
                        Rectangle
                        {
                            id: ocrObjectContentLicensePlateRect
                            color: WisenetGui.transparent
                            width: filterdefaultWidth
                            height: 63
                            anchors.top: parent.top
                            anchors.left: parent.left
                            visible: true

                            Text {
                                id: ocrObjectContentLicensePlateTitleText
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.leftMargin: 17
                                anchors.topMargin: 14
                                height: objectOptionTitleTextHeight
                                text: WisenetLinguist.licensePlate
                                color: WisenetGui.contrast_02_light_grey
                                font.pixelSize: 12
                            }

                            ShortcutButton{
                                id: ocrObjectContentInfoTooltip
                                anchors.top: ocrObjectContentLicensePlateTitleText.top
                                anchors.bottom: ocrObjectContentLicensePlateTitleText.bottom
                                anchors.left: ocrObjectContentLicensePlateTitleText.right
                                anchors.leftMargin: 5
                                sourceHeight: 16
                                sourceWidth: 16
                                height: 16
                                width: 16
                                checkable: false

                                imageNormal: WisenetImage.object_search_filter_info_default
                                imageHover: WisenetImage.object_search_filter_info_hover

                                tooltipText: WisenetLinguist.licensePlateInfo
                            }

                            WisenetTextBox{
                                id : ocrObjectContentLicensePlateTextBox
                                anchors.top: ocrObjectContentLicensePlateTitleText.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 17
                                anchors.rightMargin: 20
                                anchors.topMargin: 9
                                width: filterdefaultWidth
                                height: objectOptionTitleTextHeight
                                text: objectSearchViewModel.objectSearchFilterViewModel.ocrLicensePlateText // OCR Text를 ViewModel 연동
                                placeholderText: "ABC-1234"
                                focus: true

                                Keys.onPressed: {
                                    if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                                        objectSearchViewModel.objectSearchFilterViewModel.ocrLicensePlateText = ocrObjectContentLicensePlateTextBox.text
                                        objectSearchViewModel.objectSearchFilterViewModel.setMetaAttribute()
                                        console.log("ocrObjectContentLicensePlateTextBox typing finished", ocrObjectContentLicensePlateTextBox.text)
                                    }
                                }
                            }

                        }
                    }
                }

            }
        }
    }

    Rectangle{
        id: aiSearchFilterBorder
        x: aiSearchFilterView.width - 1
        y: 0
        width: 1
        height: aiSearchFilterView.height
        color: WisenetGui.contrast_08_dark_grey
    }

    //장치선택뷰
    Popup {
        id: deviceViewPopup
        x: aiSearchFilterView.width
        y: deviceRectangle.y
        width: 300
        height: 580
        modal: true
        focus: true

        ResourceTreeProxyModel{
            id: aiSearchDeviceTreeProxyModel
            sourceModel: DeviceSelectionTreeSourceModel{
                id: aiSearchDeviceTreeSourceModel
                isObjectSearchMode: true

                onInvalidate: aiSearchDeviceTreeProxyModel.invalidate()
            }

            Component.onCompleted: {
                doSort(Qt.AscendingOrder)
            }
        }

        contentItem: WisenetDeviceSelectionTree{
            id: deviceSeletectionTree
            anchors.fill: parent
            treeModel: aiSearchDeviceTreeProxyModel


            onSelectedDeviceChanged: {
                console.debug("onSelectedDeviceChanged .....")
                if(allDevice === true){
                    cameraNames.text = WisenetLinguist.all
                }else if(selectedDeviceValue === null || selectedDeviceValue.length === 0){
                    cameraNames.text = WisenetLinguist.none
                }else {
                    if(selectedDeviceValue.length === 1){
                        cameraNames.text = selectedDeviceValue[0].displayName
                    }else{
                        cameraNames.text = selectedDeviceValue.length.toString() + " " + WisenetLinguist.channels
                    }
                }
            }

        }

        onOpenedChanged: {
            if(opened){
                deviceSeletectionTree.expandAll()
            }
        }
    }

    //날짜선택뷰
    Popup {
        id: calendarViewPopup
        x: aiSearchFilterView.width
        //y: date.y
        width: 321
        height: 317
        modal: true
        focus: true
        background: Rectangle {
            color: WisenetGui.contrast_08_dark_grey
            border.width: 1
            border.color: WisenetGui.contrast_07_grey
        }

        contentItem: WisenetCalendar{
            id: calendarControlView
            anchors.centerIn: parent
            useMultiSelection: true

            onMultiSelectionCompleted:{
                console.debug("onMultiSelectionCompleted -- ")
                aiSearchFromDateEditor.setDate(calendarControlView.calendarTimelineModel.selectionStartDate)
                aiSearchToDateEditor.setDate(calendarControlView.calendarTimelineModel.selectionEndDate)
            }
        }

        onOpenedChanged: {
            if(opened){
                updateCalendarLimitDate()
            }
        }
    }
}

