import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetLanguage 1.0
import WisenetStyle 1.0
import "qrc:/"

Popup{
    id: filterTooltip

    height: 48
    width: 372
    property var filterModel
    property int filterDefaultWidth: 198
    property bool closeY : false

    signal resetIcon()

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

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

    component FilterSeparator: Rectangle{
        width: 1
        height: 14

        color: WisenetGui.contrast_07_grey
    }

    function setFilterVisible(meta){
        filterModel.setFilterVisible(meta)
    }

    Rectangle{
        id: bestShotProperties

        anchors.fill: parent

        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1

        // 사람
        Rectangle
        {
            id: personObjectContentRect

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 10
            anchors.leftMargin: 9

            height: 28
            visible: filterModel.objectType === 0

            // Gender
            Flow {
                id: personObjectGenderFlow
                anchors.top: parent.top
                anchors.left: parent.left
                width: 32
                height: 28

                ObjectOptionButton{
                    id: personGenderOptionMaleButton

                    tooltipText: WisenetLinguist.male
                    checked: filterModel.isPersonGenderMaleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_male_default
                    imageHover: WisenetImage.object_search_filter_male_hover
                    imageSelected: WisenetImage.object_search_filter_male_selected
                    imageSelectedHover: WisenetImage.object_search_filter_male_selected_hover

                    onClicked: {
                        filterModel.isPersonGenderMaleButtonChecked = !filterModel.isPersonGenderMaleButtonChecked
                    }
                }

                ObjectOptionButton{
                    id: personGenderOptionFemaleButton

                    tooltipText: WisenetLinguist.female
                    checked: filterModel.isPersonGenderFemaleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_female_default
                    imageHover: WisenetImage.object_search_filter_female_hover
                    imageSelected: WisenetImage.object_search_filter_female_selected
                    imageSelectedHover: WisenetImage.object_search_filter_female_selected_hover

                    onClicked: {
                        filterModel.isPersonGenderFemaleButtonChecked = !filterModel.isPersonGenderFemaleButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: personGenderSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: personObjectGenderFlow.right
                anchors.leftMargin: 2
            }

            // Clothing Top
            Flow {
                id: personObjectClothingTopFlow
                anchors.top: parent.top
                anchors.left: personGenderSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: personClothingTopBlackButton

                    tooltipText: WisenetLinguist.black
                    checked: filterModel.isPersonClothingTopBlackButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_black_default
                    imageHover: WisenetImage.object_search_filter_color_black_hover
                    imageSelected: WisenetImage.object_search_filter_color_black_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_black_hover

                    onClicked: {
                        filterModel.isPersonClothingTopBlackButtonChecked = !filterModel.isPersonClothingTopBlackButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopGrayButton

                    tooltipText: WisenetLinguist.gray
                    checked: filterModel.isPersonClothingTopGrayButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_gray_default
                    imageHover: WisenetImage.object_search_filter_color_gray_hover
                    imageSelected: WisenetImage.object_search_filter_color_gray_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_gray_hover

                    onClicked: {
                        filterModel.isPersonClothingTopGrayButtonChecked = !filterModel.isPersonClothingTopGrayButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopWhiteButton

                    tooltipText: WisenetLinguist.white
                    checked: filterModel.isPersonClothingTopWhiteButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_white_default
                    imageHover: WisenetImage.object_search_filter_color_white_hover
                    imageSelected: WisenetImage.object_search_filter_color_white_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_white_hover

                    onClicked: {
                        filterModel.isPersonClothingTopWhiteButtonChecked = !filterModel.isPersonClothingTopWhiteButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopRedButton

                    tooltipText: WisenetLinguist.red
                    checked: filterModel.isPersonClothingTopRedButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_red_default
                    imageHover: WisenetImage.object_search_filter_color_red_hover
                    imageSelected: WisenetImage.object_search_filter_color_red_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_red_hover

                    onClicked: {
                        filterModel.isPersonClothingTopRedButtonChecked = !filterModel.isPersonClothingTopRedButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopOrangeButton

                    tooltipText: WisenetLinguist.orange
                    checked: filterModel.isPersonClothingTopOrangeButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_orange_default
                    imageHover: WisenetImage.object_search_filter_color_orange_hover
                    imageSelected: WisenetImage.object_search_filter_color_orange_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_orange_hover

                    onClicked: {
                        filterModel.isPersonClothingTopOrangeButtonChecked = !filterModel.isPersonClothingTopOrangeButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopYellowButton

                    tooltipText: WisenetLinguist.yellow
                    checked: filterModel.isPersonClothingTopYellowButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_yellow_default
                    imageHover: WisenetImage.object_search_filter_color_yellow_hover
                    imageSelected: WisenetImage.object_search_filter_color_yellow_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_yellow_hover

                    onClicked: {
                        filterModel.isPersonClothingTopYellowButtonChecked = !filterModel.isPersonClothingTopYellowButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopGreenButton

                    tooltipText: WisenetLinguist.green
                    checked: filterModel.isPersonClothingTopGreenButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_green_default
                    imageHover: WisenetImage.object_search_filter_color_green_hover
                    imageSelected: WisenetImage.object_search_filter_color_green_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_green_hover

                    onClicked: {
                        filterModel.isPersonClothingTopGreenButtonChecked = !filterModel.isPersonClothingTopGreenButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopBlueButton

                    tooltipText: WisenetLinguist.blue
                    checked: filterModel.isPersonClothingTopBlueButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_blue_default
                    imageHover: WisenetImage.object_search_filter_color_blue_hover
                    imageSelected: WisenetImage.object_search_filter_color_blue_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_blue_hover

                    onClicked: {
                        filterModel.isPersonClothingTopBlueButtonChecked = !filterModel.isPersonClothingTopBlueButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingTopPurpleButton

                    tooltipText: WisenetLinguist.purple
                    checked: filterModel.isPersonClothingTopPurpleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_purple_default
                    imageHover: WisenetImage.object_search_filter_color_purple_hover
                    imageSelected: WisenetImage.object_search_filter_color_purple_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_purple_hover

                    onClicked: {
                        filterModel.isPersonClothingTopPurpleButtonChecked = !filterModel.isPersonClothingTopPurpleButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: personClothingTopSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: personObjectClothingTopFlow.right
                anchors.leftMargin: 2
            }

            //Clothing bottom
            Flow {
                id: personObjectClothingBottomFlow
                anchors.top: parent.top
                anchors.left: personClothingTopSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: personClothingBottomBlackButton
                    tooltipText: WisenetLinguist.black
                    checked: filterModel.isPersonClothingBottomBlackButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_black_default
                    imageHover: WisenetImage.object_search_filter_color_black_hover
                    imageSelected: WisenetImage.object_search_filter_color_black_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_black_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomBlackButtonChecked = !filterModel.isPersonClothingBottomBlackButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomGrayButton
                    tooltipText: WisenetLinguist.gray
                    checked: filterModel.isPersonClothingBottomGrayButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_gray_default
                    imageHover: WisenetImage.object_search_filter_color_gray_hover
                    imageSelected: WisenetImage.object_search_filter_color_gray_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_gray_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomGrayButtonChecked = !filterModel.isPersonClothingBottomGrayButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomWhiteButton
                    tooltipText: WisenetLinguist.white
                    checked: filterModel.isPersonClothingBottomWhiteButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_white_default
                    imageHover: WisenetImage.object_search_filter_color_white_hover
                    imageSelected: WisenetImage.object_search_filter_color_white_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_white_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomWhiteButtonChecked = !filterModel.isPersonClothingBottomWhiteButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomRedButton
                    tooltipText: WisenetLinguist.red
                    checked: filterModel.isPersonClothingBottomRedButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_red_default
                    imageHover: WisenetImage.object_search_filter_color_red_hover
                    imageSelected: WisenetImage.object_search_filter_color_red_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_red_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomRedButtonChecked = !filterModel.isPersonClothingBottomRedButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomOrangeButton
                    tooltipText: WisenetLinguist.orange
                    checked: filterModel.isPersonClothingBottomOrangeButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_orange_default
                    imageHover: WisenetImage.object_search_filter_color_orange_hover
                    imageSelected: WisenetImage.object_search_filter_color_orange_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_orange_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomOrangeButtonChecked = !filterModel.isPersonClothingBottomOrangeButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomYellowButton
                    tooltipText: WisenetLinguist.yellow
                    checked: filterModel.isPersonClothingBottomYellowButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_yellow_default
                    imageHover: WisenetImage.object_search_filter_color_yellow_hover
                    imageSelected: WisenetImage.object_search_filter_color_yellow_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_yellow_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomYellowButtonChecked = !filterModel.isPersonClothingBottomYellowButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomGreenButton
                    tooltipText: WisenetLinguist.green
                    checked: filterModel.isPersonClothingBottomGreenButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_green_default
                    imageHover: WisenetImage.object_search_filter_color_green_hover
                    imageSelected: WisenetImage.object_search_filter_color_green_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_green_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomGreenButtonChecked = !filterModel.isPersonClothingBottomGreenButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomBlueButton
                    tooltipText: WisenetLinguist.blue
                    checked: filterModel.isPersonClothingBottomBlueButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_blue_default
                    imageHover: WisenetImage.object_search_filter_color_blue_hover
                    imageSelected: WisenetImage.object_search_filter_color_blue_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_blue_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomBlueButtonChecked = !filterModel.isPersonClothingBottomBlueButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personClothingBottomPurpleButton
                    tooltipText: WisenetLinguist.purple
                    checked: filterModel.isPersonClothingBottomPurpleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_purple_default
                    imageHover: WisenetImage.object_search_filter_color_purple_hover
                    imageSelected: WisenetImage.object_search_filter_color_purple_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_purple_hover

                    onClicked: {
                        filterModel.isPersonClothingBottomPurpleButtonChecked = !filterModel.isPersonClothingBottomPurpleButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: personClothingBottomSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: personObjectClothingBottomFlow.right
                anchors.leftMargin: 2
            }

            //Bag
            Flow {
                id: personObjectBagFlow
                anchors.top: parent.top
                anchors.left: personClothingBottomSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: personBagOptionWearButton
                    tooltipText: WisenetLinguist.bag
                    checked: filterModel.isPersonBagWearButtonChecked

                    imageNormal: WisenetImage.object_search_filter_bag_default
                    imageHover: WisenetImage.object_search_filter_bag_hover
                    imageSelected: WisenetImage.object_search_filter_bag_selected
                    imageSelectedHover: WisenetImage.object_search_filter_bag_selected_hover

                    onClicked: {
                        filterModel.isPersonBagWearButtonChecked = !filterModel.isPersonBagWearButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: personBagOptionNoWearButton
                    tooltipText: WisenetLinguist.noBag
                    checked: filterModel.isPersonBagNoWearButtonChecked

                    imageNormal: WisenetImage.object_search_filter_no_bag_default
                    imageHover: WisenetImage.object_search_filter_no_bag_hover
                    imageSelected: WisenetImage.object_search_filter_no_bag_selected
                    imageSelectedHover: WisenetImage.object_search_filter_no_bag_selected_hover

                    onClicked: {
                        filterModel.isPersonBagNoWearButtonChecked = !filterModel.isPersonBagNoWearButtonChecked
                    }
                }
            }
        }

        // 얼굴
        Rectangle
        {
            id: faceObjectContentRect

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 10
            anchors.leftMargin: 9

            height: 28
            visible: filterModel.objectType === 2

            // Gender
            Flow {
                id: faceObjectGenderFlow
                anchors.top: parent.top
                anchors.left: parent.left
                width: 32
                height: 28

                ObjectOptionButton{
                    id: faceGenderOptionMaleButton
                    tooltipText: WisenetLinguist.male
                    checked: filterModel.isFaceGenderMaleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_male_default
                    imageHover: WisenetImage.object_search_filter_male_hover
                    imageSelected: WisenetImage.object_search_filter_male_selected
                    imageSelectedHover: WisenetImage.object_search_filter_male_selected_hover

                    onClicked: {
                        filterModel.isFaceGenderMaleButtonChecked = !filterModel.isFaceGenderMaleButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: faceGenderOptionFemaleButton
                    tooltipText: WisenetLinguist.female
                    checked: filterModel.isFaceGenderFemaleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_female_default
                    imageHover: WisenetImage.object_search_filter_female_hover
                    imageSelected: WisenetImage.object_search_filter_female_selected
                    imageSelectedHover: WisenetImage.object_search_filter_female_selected_hover

                    onClicked: {
                        filterModel.isFaceGenderFemaleButtonChecked = !filterModel.isFaceGenderFemaleButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: faceGenderSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: faceObjectGenderFlow.right
                anchors.leftMargin: 2
            }

            // Age : Young, Adult, Middle, Senior
            Flow {
                id: faceObjectAgeTypeFlow
                anchors.top: parent.top
                anchors.left: faceGenderSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: faceAgeTypeYoungButton
                    tooltipText: WisenetLinguist.young
                    checked: filterModel.isFaceAgeTypeYoungButtonChecked

                    imageNormal: WisenetImage.object_search_filter_young_default
                    imageHover: WisenetImage.object_search_filter_young_hover
                    imageSelected: WisenetImage.object_search_filter_young_selected
                    imageSelectedHover: WisenetImage.object_search_filter_young_selected_hover

                    onClicked: {
                        filterModel.isFaceAgeTypeYoungButtonChecked = !filterModel.isFaceAgeTypeYoungButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: faceAgeTypeAdultButton
                    tooltipText: WisenetLinguist.adult
                    checked: filterModel.isFaceAgeTypeAdultButtonChecked

                    imageNormal: WisenetImage.object_search_filter_adult_default
                    imageHover: WisenetImage.object_search_filter_adult_hover
                    imageSelected: WisenetImage.object_search_filter_adult_selected
                    imageSelectedHover: WisenetImage.object_search_filter_adult_selected_hover

                    onClicked: {
                        filterModel.isFaceAgeTypeAdultButtonChecked = !filterModel.isFaceAgeTypeAdultButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: faceAgeTypeMiddleButton
                    tooltipText: WisenetLinguist.middle
                    checked: filterModel.isFaceAgeTypeMiddleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_middle_default
                    imageHover: WisenetImage.object_search_filter_middle_hover
                    imageSelected: WisenetImage.object_search_filter_middle_selected
                    imageSelectedHover: WisenetImage.object_search_filter_middle_selected_hover

                    onClicked: {
                        filterModel.isFaceAgeTypeMiddleButtonChecked = !filterModel.isFaceAgeTypeMiddleButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: faceAgeTypeSeniorButton
                    tooltipText: WisenetLinguist.senior
                    checked: filterModel.isFaceAgeTypeSeniorButtonChecked

                    imageNormal: WisenetImage.object_search_filter_senior_default
                    imageHover: WisenetImage.object_search_filter_senior_hover
                    imageSelected: WisenetImage.object_search_filter_senior_selected
                    imageSelectedHover: WisenetImage.object_search_filter_senior_selected_hover

                    onClicked: {
                        filterModel.isFaceAgeTypeSeniorButtonChecked = !filterModel.isFaceAgeTypeSeniorButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: faceAgeSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: faceObjectAgeTypeFlow.right
                anchors.leftMargin: 2
            }

            // Glasses
            Flow {
                id: faceObjectGlassesFlow
                anchors.top: parent.top
                anchors.left: faceAgeSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: faceOpticalsOptionWearButton
                    tooltipText: WisenetLinguist.glasses
                    checked: filterModel.isFaceOpticalsWearButtonChecked

                    imageNormal: WisenetImage.object_search_filter_glasses_default
                    imageHover: WisenetImage.object_search_filter_glasses_hover
                    imageSelected: WisenetImage.object_search_filter_glasses_selected
                    imageSelectedHover: WisenetImage.object_search_filter_glasses_selected_hover

                    onClicked: {
                        filterModel.isFaceOpticalsWearButtonChecked = !filterModel.isFaceOpticalsWearButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: faceOpticalsOptionNoWearButton
                    tooltipText: WisenetLinguist.noGlasses
                    checked: filterModel.isFaceOpticalsNoWearButtonChecked

                    imageNormal: WisenetImage.object_search_filter_no_glasses_default
                    imageHover: WisenetImage.object_search_filter_no_glasses_hover
                    imageSelected: WisenetImage.object_search_filter_no_glasses_selected
                    imageSelectedHover: WisenetImage.object_search_filter_no_glasses_selected_hover

                    onClicked: {
                        filterModel.isFaceOpticalsNoWearButtonChecked = !filterModel.isFaceOpticalsNoWearButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: faceGlassesSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: faceObjectGlassesFlow.right
                anchors.leftMargin: 2
            }

            // Mask : Wear, No
            Flow {
                id: faceObjectMaskFlow
                anchors.top: parent.top
                anchors.left: faceGlassesSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: faceMaskOptionWearButton
                    tooltipText: WisenetLinguist.faceMask
                    checked: filterModel.isFaceMaskWearButtonChecked

                    imageNormal: WisenetImage.object_search_filter_mask_default
                    imageHover: WisenetImage.object_search_filter_mask_hover
                    imageSelected: WisenetImage.object_search_filter_mask_selected
                    imageSelectedHover: WisenetImage.object_search_filter_mask_selected_hover

                    onClicked: {
                        filterModel.isFaceMaskWearButtonChecked = !filterModel.isFaceMaskWearButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: faceMaskOptionNoWearButton
                    tooltipText: WisenetLinguist.noFaceMask
                    checked: filterModel.isFaceMaskNoWearButtonChecked

                    imageNormal: WisenetImage.object_search_filter_no_mask_default
                    imageHover: WisenetImage.object_search_filter_no_mask_hover
                    imageSelected: WisenetImage.object_search_filter_no_mask_selected
                    imageSelectedHover: WisenetImage.object_search_filter_no_mask_selected_hover

                    onClicked: {
                        filterModel.isFaceMaskNoWearButtonChecked = !filterModel.isFaceMaskNoWearButtonChecked
                    }
                }
            }

        }

        // 차량
        Rectangle
        {
            id: vehicleObjectContentRect

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 10
            anchors.leftMargin: 9

            height: 28

            visible: filterModel.objectType === 1

            // Type : Car, Bus, Truck, Motorcycle, Bicycle
            Flow {
                id: vehicleObjectTypeFlow
                anchors.top: parent.top
                anchors.left: parent.left
                width: 32
                height: 28

                ObjectOptionButton{
                    id: vehicleTypeOptionCarButton
                    tooltipText: WisenetLinguist.car
                    checked: filterModel.isVehicleTypeCarButtonChecked

                    imageNormal: WisenetImage.object_search_filter_car_default
                    imageHover: WisenetImage.object_search_filter_car_hover
                    imageSelected: WisenetImage.object_search_filter_car_selected
                    imageSelectedHover: WisenetImage.object_search_filter_car_selected_hover

                    onClicked: {
                        filterModel.isVehicleTypeCarButtonChecked = !filterModel.isVehicleTypeCarButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleTypeOptionBusButton
                    tooltipText: WisenetLinguist.bus
                    checked: filterModel.isVehicleTypeBusButtonChecked

                    imageNormal: WisenetImage.object_search_filter_bus_default
                    imageHover: WisenetImage.object_search_filter_bus_hover
                    imageSelected: WisenetImage.object_search_filter_bus_selected
                    imageSelectedHover: WisenetImage.object_search_filter_bus_selected_hover

                    onClicked: {
                        filterModel.isVehicleTypeBusButtonChecked = !filterModel.isVehicleTypeBusButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleTypeOptionTruckButton
                    tooltipText: WisenetLinguist.truck
                    checked: filterModel.isVehicleTypeTruckButtonChecked

                    imageNormal: WisenetImage.object_search_filter_truck_default
                    imageHover: WisenetImage.object_search_filter_truck_hover
                    imageSelected: WisenetImage.object_search_filter_truck_selected
                    imageSelectedHover: WisenetImage.object_search_filter_truck_selected_hover

                    onClicked: {
                        filterModel.isVehicleTypeTruckButtonChecked = !filterModel.isVehicleTypeTruckButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleTypeOptionMotorcycleButton
                    tooltipText: WisenetLinguist.motorcycle
                    checked: filterModel.isVehicleTypeMotorcycleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_motorcycle_default
                    imageHover: WisenetImage.object_search_filter_motorcycle_hover
                    imageSelected: WisenetImage.object_search_filter_motorcycle_selected
                    imageSelectedHover: WisenetImage.object_search_filter_motorcycle_selected_hover

                    onClicked: {
                        filterModel.isVehicleTypeMotorcycleButtonChecked = !filterModel.isVehicleTypeMotorcycleButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleTypeOptionBicycleButton
                    tooltipText: WisenetLinguist.bicycle
                    checked: filterModel.isVehicleTypeBicycleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_bicycle_default
                    imageHover: WisenetImage.object_search_filter_bicycle_hover
                    imageSelected: WisenetImage.object_search_filter_bicycle_selected
                    imageSelectedHover: WisenetImage.object_search_filter_bicycle_selected_hover

                    onClicked: {
                        filterModel.isVehicleTypeBicycleButtonChecked = !filterModel.isVehicleTypeBicycleButtonChecked
                    }
                }
            }

            FilterSeparator{
                id: vehicleTypeSeparator
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.left: vehicleObjectTypeFlow.right
                anchors.leftMargin: 2
            }

            // 차량 색상 : Any, Black, Gray, White, Red, Orange, Yellow, Green, Blue, Purple
            Flow {
                id: vehicleObjectColorFlow
                anchors.top: parent.top
                anchors.left: vehicleTypeSeparator.right
                anchors.leftMargin: 4
                height: 28

                ObjectOptionButton{
                    id: vehicleColorBlackButton
                    tooltipText: WisenetLinguist.black
                    checked: filterModel.isVehicleColorBlackButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_black_default
                    imageHover: WisenetImage.object_search_filter_color_black_hover
                    imageSelected: WisenetImage.object_search_filter_color_black_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_black_hover

                    onClicked: {
                        filterModel.isVehicleColorBlackButtonChecked = !filterModel.isVehicleColorBlackButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorGrayButton
                    tooltipText: WisenetLinguist.gray
                    checked: filterModel.isVehicleColorGrayButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_gray_default
                    imageHover: WisenetImage.object_search_filter_color_gray_hover
                    imageSelected: WisenetImage.object_search_filter_color_gray_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_gray_hover

                    onClicked: {
                        filterModel.isVehicleColorGrayButtonChecked = !filterModel.isVehicleColorGrayButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorWhiteButton
                    tooltipText: WisenetLinguist.white
                    checked: filterModel.isVehicleColorWhiteButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_white_default
                    imageHover: WisenetImage.object_search_filter_color_white_hover
                    imageSelected: WisenetImage.object_search_filter_color_white_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_white_hover

                    onClicked: {
                        filterModel.isVehicleColorWhiteButtonChecked = !filterModel.isVehicleColorWhiteButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorRedButton
                    tooltipText: WisenetLinguist.red
                    checked: filterModel.isVehicleColorRedButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_red_default
                    imageHover: WisenetImage.object_search_filter_color_red_hover
                    imageSelected: WisenetImage.object_search_filter_color_red_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_red_hover

                    onClicked: {
                        filterModel.isVehicleColorRedButtonChecked = !filterModel.isVehicleColorRedButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorOrangeButton
                    tooltipText: WisenetLinguist.orange
                    checked: filterModel.isVehicleColorOrangeButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_orange_default
                    imageHover: WisenetImage.object_search_filter_color_orange_hover
                    imageSelected: WisenetImage.object_search_filter_color_orange_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_orange_hover

                    onClicked: {
                        filterModel.isVehicleColorOrangeButtonChecked = !filterModel.isVehicleColorOrangeButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorYellowButton
                    tooltipText: WisenetLinguist.yellow
                    checked: filterModel.isVehicleColorYellowButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_yellow_default
                    imageHover: WisenetImage.object_search_filter_color_yellow_hover
                    imageSelected: WisenetImage.object_search_filter_color_yellow_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_yellow_hover

                    onCheckedChanged: {
                        console.log("personGenderOptionFemaleButton.checked",this.checked)
                        filterModel.isVehicleColorYellowButtonChecked = this.checked
                    }
                    onClicked: {
                        filterModel.isVehicleColorYellowButtonChecked = !filterModel.isPersonClothingBottomGrayButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorGreenButton
                    tooltipText: WisenetLinguist.green
                    checked: filterModel.isVehicleColorGreenButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_green_default
                    imageHover: WisenetImage.object_search_filter_color_green_hover
                    imageSelected: WisenetImage.object_search_filter_color_green_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_green_hover

                    onCheckedChanged: {
                        console.log("vehicleColorGreenButton.checked",this.checked)
                        filterModel.isVehicleColorGreenButtonChecked = this.checked
                    }
                    onClicked: {
                        filterModel.isVehicleColorGreenButtonChecked = !filterModel.isVehicleColorGreenButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorBlueButton
                    tooltipText: WisenetLinguist.blue
                    checked: filterModel.isVehicleColorBlueButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_blue_default
                    imageHover: WisenetImage.object_search_filter_color_blue_hover
                    imageSelected: WisenetImage.object_search_filter_color_blue_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_blue_hover

                    onClicked: {
                        filterModel.isVehicleColorBlueButtonChecked = !filterModel.isVehicleColorBlueButtonChecked
                    }
                }
                ObjectOptionButton{
                    id: vehicleColorPurpleButton
                    tooltipText: WisenetLinguist.purple
                    checked: filterModel.isVehicleColorPurpleButtonChecked

                    imageNormal: WisenetImage.object_search_filter_color_purple_default
                    imageHover: WisenetImage.object_search_filter_color_purple_hover
                    imageSelected: WisenetImage.object_search_filter_color_purple_default
                    imageSelectedHover: WisenetImage.object_search_filter_color_purple_hover

                    onClicked: {
                        filterModel.isVehicleColorPurpleButtonChecked = !filterModel.isVehicleColorPurpleButtonChecked
                    }
                }
            }
        }

        /*
        Rectangle{
            id: ocrObjectContentRect

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 10
            anchors.leftMargin: 9

            height: 28

            visible: filterModel.objectType === 3

            WisenetTextBox{
                id : ocrTextBox
                width: 180
                height: 26
                placeholderText: WisenetLinguist.number + ", ?, *"
                focus: true
            }
        }*/

        WisenetButton2{
            id: applyButton
            width: 120
            height: 28
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 10
            anchors.rightMargin: 9

            text: WisenetLinguist.apply

            onClicked: {
                filterModel.applyFilter()
                filterTooltip.close()
            }
        }
    }

    onYChanged: {
        if(closeY)
            close()
    }

    Connections{
        target: filterModel

        function onObjectTypeChanged(objectType){

            personObjectContentRect.visible = false
            faceObjectContentRect.visible = false
            vehicleObjectContentRect.visible = false
            if(objectType === 0)
                personObjectContentRect.visible = true
            else if(objectType === 1)
                vehicleObjectContentRect.visible = true
            else if(objectType === 2)
                faceObjectContentRect.visible = true
            //else
                //ocrObjectContentRect.visible = true
        }

        // visible
        // Person - Gender
        function onPersonGenderMaleButtonVisibleChanged(isPersonGenderMaleButtonVisible){
            personGenderOptionMaleButton.visible = isPersonGenderMaleButtonVisible
        }
        function onPersonGenderFemaleButtonVisibleChanged(isPersonGenderFemaleButtonVisible){
            personGenderOptionFemaleButton.visible = isPersonGenderFemaleButtonVisible
        }

        // Person - ClothingTop
        function onPersonClothingTopBlackButtonVisibleChanged(isPersonClothingTopBlackButtonVisible){
            personClothingTopBlackButton.visible = isPersonClothingTopBlackButtonVisible
        }
        function onPersonClothingTopGrayButtonVisibleChanged(isPersonClothingTopGrayButtonVisible){
            personClothingTopGrayButton.visible = isPersonClothingTopGrayButtonVisible
        }
        function onPersonClothingTopWhiteButtonVisibleChanged(isPersonClothingTopWhiteButtonVisible){
            personClothingTopWhiteButton.visible = isPersonClothingTopWhiteButtonVisible
        }
        function onPersonClothingTopRedButtonVisibleChanged(isPersonClothingTopRedButtonVisible){
            personClothingTopRedButton.visible = isPersonClothingTopRedButtonVisible
        }
        function onPersonClothingTopOrangeButtonVisibleChanged(isPersonClothingTopOrangeButtonVisible){
            personClothingTopOrangeButton.visible = isPersonClothingTopOrangeButtonVisible
        }
        function onPersonClothingTopYellowButtonVisibleChanged(isPersonClothingTopYellowButtonVisible){
            personClothingTopYellowButton.visible = isPersonClothingTopYellowButtonVisible
        }
        function onPersonClothingTopGreenButtonVisibleChanged(isPersonClothingTopGreenButtonVisible){
            personClothingTopGreenButton.visible = isPersonClothingTopGreenButtonVisible
        }
        function onPersonClothingTopBlueButtonVisibleChanged(isPersonClothingTopBlueButtonVisible){
            personClothingTopBlueButton.visible = isPersonClothingTopBlueButtonVisible
        }
        function onPersonClothingTopPurpleButtonVisibleChanged(isPersonClothingTopPurpleButtonVisible){
            personClothingTopPurpleButton.visible = isPersonClothingTopPurpleButtonVisible
        }

        // Person - ClothingBottom
        function onPersonClothingBottomBlackButtonVisibleChanged(isPersonClothingBottomBlackButtonVisible){
            personClothingBottomBlackButton.visible = isPersonClothingBottomBlackButtonVisible
        }
        function onPersonClothingBottomGrayButtonVisibleChanged(isPersonClothingBottomGrayButtonVisible){
            personClothingBottomGrayButton.visible = isPersonClothingBottomGrayButtonVisible
        }
        function onPersonClothingBottomWhiteButtonVisibleChanged(isPersonClothingBottomWhiteButtonVisible){
            personClothingBottomWhiteButton.visible = isPersonClothingBottomWhiteButtonVisible
        }
        function onPersonClothingBottomRedButtonVisibleChanged(isPersonClothingBottomRedButtonVisible){
            personClothingBottomRedButton.visible = isPersonClothingBottomRedButtonVisible
        }
        function onPersonClothingBottomOrangeButtonVisibleChanged(isPersonClothingBottomOrangeButtonVisible){
            personClothingBottomOrangeButton.visible = isPersonClothingBottomOrangeButtonVisible
        }
        function onPersonClothingBottomYellowButtonVisibleChanged(isPersonClothingBottomYellowButtonVisible){
            personClothingBottomYellowButton.visible = isPersonClothingBottomYellowButtonVisible
        }
        function onPersonClothingBottomGreenButtonVisibleChanged(isPersonClothingBottomGreenButtonVisible){
            personClothingBottomGreenButton.visible = isPersonClothingBottomGreenButtonVisible
        }
        function onPersonClothingBottomBlueButtonVisibleChanged(isPersonClothingBottomBlueButtonVisible){
            personClothingBottomBlueButton.visible = isPersonClothingBottomBlueButtonVisible
        }
        function onPersonClothingBottomPurpleButtonVisibleChanged(isPersonClothingBottomPurpleButtonVisible){
            personClothingBottomPurpleButton.visible = isPersonClothingBottomPurpleButtonVisible
        }

        // Person - Bag
        function onPersonBagWearButtonVisibleChanged(isPersonBagWearButtonVisible){
            personBagOptionWearButton.visible = isPersonBagWearButtonVisible
        }
        function onPersonBagNoWearButtonVisibleChanged(isPersonBagNoWearButtonVisible){
            personBagOptionNoWearButton.visible = isPersonBagNoWearButtonVisible
        }

        // Face - Gender
        function onFaceGenderMaleButtonVisibleChanged(isFaceGenderMaleButtonVisible){
            faceGenderOptionMaleButton.visible = isFaceGenderMaleButtonVisible
        }
        function onFaceGenderFemaleButtonVisibleChanged(isFaceGenderFemaleButtonVisible){
            faceGenderOptionFemaleButton.visible = isFaceGenderFemaleButtonVisible
        }

        // Face - AgeType
        function onFaceAgeTypeYoungButtonVisibleChanged(isFaceAgeTypeYoungButtonVisible){
            faceAgeTypeYoungButton.visible = isFaceAgeTypeYoungButtonVisible
        }
        function onFaceAgeTypeAdultButtonVisibleChanged(isFaceAgeTypeAdultButtonVisible){
            faceAgeTypeAdultButton.visible = isFaceAgeTypeAdultButtonVisible
        }
        function onFaceAgeTypeMiddleButtonVisibleChanged(isFaceAgeTypeMiddleButtonVisible){
            faceAgeTypeMiddleButton.visible = isFaceAgeTypeMiddleButtonVisible
        }
        function onFaceAgeTypeSeniorButtonVisibleChanged(isFaceAgeTypeSeniorButtonVisible){
            faceAgeTypeSeniorButton.visible = isFaceAgeTypeSeniorButtonVisible
        }

        // Face - Opticals
        function onFaceOpticalsWearButtonVisibleChanged(isFaceOpticalsWearButtonVisible){
            faceOpticalsOptionWearButton.visible = isFaceOpticalsWearButtonVisible
        }
        function onFaceOpticalsNoWearButtonVisibleChanged(isFaceOpticalsNoWearButtonVisible){
            faceOpticalsOptionNoWearButton.visible = isFaceOpticalsNoWearButtonVisible
        }

        // Face - Mask
        function onFaceMaskWearButtonVisibleChanged(isFaceMaskWearButtonVisible){
            faceMaskOptionWearButton.visible = isFaceMaskWearButtonVisible
        }
        function onFaceMaskNoWearButtonVisibleChanged(isFaceMaskNoWearButtonVisible){
            faceMaskOptionNoWearButton.visible = isFaceMaskNoWearButtonVisible
        }

        // Vehicle - Type
        function onVehicleTypeCarButtonVisibleChanged(isVehicleTypeCarButtonVisible){
            vehicleTypeOptionCarButton.visible = isVehicleTypeCarButtonVisible
        }
        function onVehicleTypeBusButtonVisibleChanged(isVehicleTypeBusButtonVisible){
            vehicleTypeOptionBusButton.visible = isVehicleTypeBusButtonVisible
        }
        function onVehicleTypeTruckButtonVisibleChanged(isVehicleTypeTruckButtonVisible){
            vehicleTypeOptionTruckButton.visible = isVehicleTypeTruckButtonVisible
        }
        function onVehicleTypeMotorcycleButtonVisibleChanged(isVehicleTypeMotorcycleButtonVisible){
            vehicleTypeOptionMotorcycleButton.visible = isVehicleTypeMotorcycleButtonVisible
        }
        function onVehicleTypeBicycleButtonVisibleChanged(isVehicleTypeBicycleButtonVisible){
            vehicleTypeOptionBicycleButton.visible = isVehicleTypeBicycleButtonVisible
        }

        // Vehicle - Color
        function onVehicleColorBlackButtonVisibleChanged(isVehicleColorBlackButtonVisible){
            vehicleColorBlackButton.visible = isVehicleColorBlackButtonVisible
        }
        function onVehicleColorGrayButtonVisibleChanged(isVehicleColorGrayButtonVisible){
            vehicleColorGrayButton.visible = isVehicleColorGrayButtonVisible
        }
        function onVehicleColorWhiteButtonVisibleChanged(isVehicleColorWhiteButtonVisible){
            vehicleColorWhiteButton.visible = isVehicleColorWhiteButtonVisible
        }
        function onVehicleColorRedButtonVisibleChanged(isVehicleColorRedButtonVisible){
            vehicleColorRedButton.visible = isVehicleColorRedButtonVisible
        }
        function onVehicleColorOrangeButtonVisibleChanged(isVehicleColorOrangeButtonVisible){
            vehicleColorOrangeButton.visible = isVehicleColorOrangeButtonVisible
        }
        function onVehicleColorYellowButtonVisibleChanged(isVehicleColorYellowButtonVisible){
            vehicleColorYellowButton.visible = isVehicleColorYellowButtonVisible
        }
        function onVehicleColorGreenButtonVisibleChanged(isVehicleColorGreenButtonVisible){
            vehicleColorGreenButton.visible = isVehicleColorGreenButtonVisible
        }
        function onVehicleColorBlueButtonVisibleChanged(isVehicleColorBlueButtonVisible){
            vehicleColorBlueButton.visible = isVehicleColorBlueButtonVisible
        }
        function onVehicleColorPurpleButtonVisibleChanged(isVehicleColorPurpleButtonVisible){
            vehicleColorPurpleButton.visible = isVehicleColorPurpleButtonVisible
        }

        // checked
        // Person - Gender
        function onPersonGenderMaleButtonCheckedChanged(isPersonGenderMaleButtonChecked){
            personGenderOptionMaleButton.checked = isPersonGenderMaleButtonChecked
        }
        function onPersonGenderFemaleButtonCheckedChanged(isPersonGenderFemaleButtonChecked){
            personGenderOptionFemaleButton.checked = isPersonGenderFemaleButtonChecked
        }

        // Person - ClothingTop
        function onPersonClothingTopBlackButtonCheckedChanged(isPersonClothingTopBlackButtonChecked){
            personClothingTopBlackButton.checked = isPersonClothingTopBlackButtonChecked
        }
        function onPersonClothingTopGrayButtonCheckedChanged(isPersonClothingTopGrayButtonChecked){
            personClothingTopGrayButton.checked = isPersonClothingTopGrayButtonChecked
        }
        function onPersonClothingTopWhiteButtonCheckedChanged(isPersonClothingTopWhiteButtonChecked){
            personClothingTopWhiteButton.checked = isPersonClothingTopWhiteButtonChecked
        }
        function onPersonClothingTopRedButtonCheckedChanged(isPersonClothingTopRedButtonChecked){
            personClothingTopRedButton.checked = isPersonClothingTopRedButtonChecked
        }
        function onPersonClothingTopOrangeButtonCheckedChanged(isPersonClothingTopOrangeButtonChecked){
            personClothingTopOrangeButton.checked = isPersonClothingTopOrangeButtonChecked
        }
        function onPersonClothingTopYellowButtonCheckedChanged(isPersonClothingTopYellowButtonChecked){
            personClothingTopYellowButton.checked = isPersonClothingTopYellowButtonChecked
        }
        function onPersonClothingTopGreenButtonCheckedChanged(isPersonClothingTopGreenButtonChecked){
            personClothingTopGreenButton.checked = isPersonClothingTopGreenButtonChecked
        }
        function onPersonClothingTopBlueButtonCheckedChanged(isPersonClothingTopBlueButtonChecked){
            personClothingTopBlueButton.checked = isPersonClothingTopBlueButtonChecked
        }
        function onPersonClothingTopPurpleButtonCheckedChanged(isPersonClothingTopPurpleButtonChecked){
            personClothingTopPurpleButton.checked = isPersonClothingTopPurpleButtonChecked
        }

        // Person - ClothingBottom
        function onPersonClothingBottomBlackButtonCheckedChanged(isPersonClothingBottomBlackButtonChecked){
            personClothingBottomBlackButton.checked = isPersonClothingBottomBlackButtonChecked
        }
        function onPersonClothingBottomGrayButtonCheckedChanged(isPersonClothingBottomGrayButtonChecked){
            personClothingBottomGrayButton.checked = isPersonClothingBottomGrayButtonChecked
        }
        function onPersonClothingBottomWhiteButtonCheckedChanged(isPersonClothingBottomWhiteButtonChecked){
            personClothingBottomWhiteButton.checked = isPersonClothingBottomWhiteButtonChecked
        }
        function onPersonClothingBottomRedButtonCheckedChanged(isPersonClothingBottomRedButtonChecked){
            personClothingBottomRedButton.checked = isPersonClothingBottomRedButtonChecked
        }
        function onPersonClothingBottomOrangeButtonCheckedChanged(isPersonClothingBottomOrangeButtonChecked){
            personClothingBottomOrangeButton.checked = isPersonClothingBottomOrangeButtonChecked
        }
        function onPersonClothingBottomYellowButtonCheckedChanged(isPersonClothingBottomYellowButtonChecked){
            personClothingBottomYellowButton.checked = isPersonClothingBottomYellowButtonChecked
        }
        function onPersonClothingBottomGreenButtonCheckedChanged(isPersonClothingBottomGreenButtonChecked){
            personClothingBottomGreenButton.checked = isPersonClothingBottomGreenButtonChecked
        }
        function onPersonClothingBottomBlueButtonCheckedChanged(isPersonClothingBottomBlueButtonChecked){
            personClothingBottomBlueButton.checked = isPersonClothingBottomBlueButtonChecked
        }
        function onPersonClothingBottomPurpleButtonCheckedChanged(isPersonClothingBottomPurpleButtonChecked){
            personClothingBottomPurpleButton.checked = isPersonClothingBottomPurpleButtonChecked
        }

        // Person - Bag
        function onPersonBagWearButtonCheckedChanged(isPersonBagWearButtonChecked){
            personBagOptionWearButton.checked = isPersonBagWearButtonChecked
        }
        function onPersonBagNoWearButtonCheckedChanged(isPersonBagNoWearButtonChecked){
            personBagOptionNoWearButton.checked = isPersonBagNoWearButtonChecked
        }

        // Face - Gender
        function onFaceGenderMaleButtonCheckedChanged(isFaceGenderMaleButtonChecked){
            faceGenderOptionMaleButton.checked = isFaceGenderMaleButtonChecked
        }
        function onFaceGenderFemaleButtonCheckedChanged(isFaceGenderFemaleButtonChecked){
            faceGenderOptionFemaleButton.checked = isFaceGenderFemaleButtonChecked
        }

        // Face - AgeType
        function onFaceAgeTypeYoungButtonCheckedChanged(isFaceAgeTypeYoungButtonChecked){
            faceAgeTypeYoungButton.checked = isFaceAgeTypeYoungButtonChecked
        }
        function onFaceAgeTypeAdultButtonCheckedChanged(isFaceAgeTypeAdultButtonChecked){
            faceAgeTypeAdultButton.checked = isFaceAgeTypeAdultButtonChecked
        }
        function onFaceAgeTypeMiddleButtonCheckedChanged(isFaceAgeTypeMiddleButtonChecked){
            faceAgeTypeMiddleButton.checked = isFaceAgeTypeMiddleButtonChecked
        }
        function onFaceAgeTypeSeniorButtonCheckedChanged(isFaceAgeTypeSeniorButtonChecked){
            faceAgeTypeSeniorButton.checked = isFaceAgeTypeSeniorButtonChecked
        }

        // Face - Opticals
        function onFaceOpticalsWearButtonCheckedChanged(isFaceOpticalsWearButtonChecked){
            faceOpticalsOptionWearButton.checked = isFaceOpticalsWearButtonChecked
        }
        function onFaceOpticalsNoWearButtonCheckedChanged(isFaceOpticalsNoWearButtonChecked){
            faceOpticalsOptionNoWearButton.checked = isFaceOpticalsNoWearButtonChecked
        }

        // Face - Mask
        function onFaceMaskWearButtonCheckedChanged(isFaceMaskWearButtonChecked){
            faceMaskOptionWearButton.checked = isFaceMaskWearButtonChecked
        }
        function onFaceMaskNoWearButtonCheckedChanged(isFaceMaskNoWearButtonChecked){
            faceMaskOptionNoWearButton.checked = isFaceMaskNoWearButtonChecked
        }

        // Vehicle - Type
        function onVehicleTypeCarButtonCheckedChanged(isVehicleTypeCarButtonChecked){
            vehicleTypeOptionCarButton.checked = isVehicleTypeCarButtonChecked
        }
        function onVehicleTypeBusButtonCheckedChanged(isVehicleTypeBusButtonChecked){
            vehicleTypeOptionBusButton.checked = isVehicleTypeBusButtonChecked
        }
        function onVehicleTypeTruckButtonCheckedChanged(isVehicleTypeTruckButtonChecked){
            vehicleTypeOptionTruckButton.checked = isVehicleTypeTruckButtonChecked
        }
        function onVehicleTypeMotorcycleButtonCheckedChanged(isVehicleTypeMotorcycleButtonChecked){
            vehicleTypeOptionMotorcycleButton.checked = isVehicleTypeMotorcycleButtonChecked
        }
        function onVehicleTypeBicycleButtonCheckedChanged(isVehicleTypeBicycleButtonChecked){
            vehicleTypeOptionBicycleButton.checked = isVehicleTypeBicycleButtonChecked
        }

        // Vehicle - Color
        function onVehicleColorBlackButtonCheckedChanged(isVehicleColorBlackButtonChecked){
            vehicleColorBlackButton.checked = isVehicleColorBlackButtonChecked
        }
        function onVehicleColorGrayButtonCheckedChanged(isVehicleColorGrayButtonChecked){
            vehicleColorGrayButton.checked = isVehicleColorGrayButtonChecked
        }
        function onVehicleColorWhiteButtonCheckedChanged(isVehicleColorWhiteButtonChecked){
            vehicleColorWhiteButton.checked = isVehicleColorWhiteButtonChecked
        }
        function onVehicleColorRedButtonCheckedChanged(isVehicleColorRedButtonChecked){
            vehicleColorRedButton.checked = isVehicleColorRedButtonChecked
        }
        function onVehicleColorOrangeButtonCheckedChanged(isVehicleColorOrangeButtonChecked){
            vehicleColorOrangeButton.checked = isVehicleColorOrangeButtonChecked
        }
        function onVehicleColorYellowButtonCheckedChanged(isVehicleColorYellowButtonChecked){
            vehicleColorYellowButton.checked = isVehicleColorYellowButtonChecked
        }
        function onVehicleColorGreenButtonCheckedChanged(isVehicleColorGreenButtonChecked){
            vehicleColorGreenButton.checked = isVehicleColorGreenButtonChecked
        }
        function onVehicleColorBlueButtonCheckedChanged(isVehicleColorBlueButtonChecked){
            vehicleColorBlueButton.checked = isVehicleColorBlueButtonChecked
        }
        function onVehicleColorPurpleButtonCheckedChanged(isVehicleColorPurpleButtonChecked){
            vehicleColorPurpleButton.checked = isVehicleColorPurpleButtonChecked
        }
    }
}
