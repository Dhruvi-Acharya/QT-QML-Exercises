#include "BestshotFilterTooltipModel.h"

BestshotFilterTooltipModel::BestshotFilterTooltipModel()
{

}

BestshotFilterTooltipModel::~BestshotFilterTooltipModel()
{

}

int BestshotFilterTooltipModel::objectType()
{
    return m_objectType;
}

void BestshotFilterTooltipModel::setObjectType(int objectType)
{
    m_objectType = objectType;
    emit objectTypeChanged(objectType);
}

void BestshotFilterTooltipModel::setFilterVisible(QMetaAttribute* qMetaAttr)
{
    qDebug() << "BestshotFilterTooltipModel::setFilterVisible()" << qMetaAttr;
    Wisenet::Device::MetaAttribute* metaAttr = qMetaAttr->metaAttribute();

    if(metaAttr == nullptr){
        qDebug() << "BestshotFilterTooltipModel::setFilterVisible() nullptr";
        return;
    }
    clearAllObjectOptionButtons();

    setObjectType((int)metaAttr->attributeType);

    if(metaAttr->attributeType == Wisenet::Device::AiClassType::person)
    {
        // person - gender
        if(metaAttr->personGender.size() > 0)
        {
            if(metaAttr->personGender[0] == "Male" && m_isPersonGenderMaleButtonVisible == false)
            {
                m_isPersonGenderMaleButtonVisible = true;
            }
            else if(metaAttr->personGender[0] == "Female" && m_isPersonGenderFemaleButtonVisible == false)
            {
                m_isPersonGenderFemaleButtonVisible = true;
            }
        }

        // person - clothing top
        if(metaAttr->personClothingTopsColor.size() >= 1)
        {
            if(metaAttr->personClothingTopsColor[0] == "Black" && m_isPersonClothingTopBlackButtonVisible == false)
            {
                m_isPersonClothingTopBlackButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Gray" && m_isPersonClothingTopGrayButtonVisible == false)
            {
                m_isPersonClothingTopGrayButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "White" && m_isPersonClothingTopWhiteButtonVisible == false)
            {
                m_isPersonClothingTopWhiteButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Red" && m_isPersonClothingTopRedButtonVisible == false)
            {
                m_isPersonClothingTopRedButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Orange" && m_isPersonClothingTopOrangeButtonVisible == false)
            {
                m_isPersonClothingTopOrangeButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Yellow" && m_isPersonClothingTopYellowButtonVisible == false)
            {
                m_isPersonClothingTopYellowButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Green" && m_isPersonClothingTopGreenButtonVisible == false)
            {
                m_isPersonClothingTopGreenButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Blue" && m_isPersonClothingTopBlueButtonVisible == false)
            {
                m_isPersonClothingTopBlueButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[0] == "Purple" && m_isPersonClothingTopPurpleButtonVisible == false)
            {
                m_isPersonClothingTopPurpleButtonVisible = true;
            }
        }

        if(metaAttr->personClothingTopsColor.size() >= 2)
        {
            if(metaAttr->personClothingTopsColor[1] == "Black" && m_isPersonClothingTopBlackButtonVisible == false)
            {
                m_isPersonClothingTopBlackButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Gray" && m_isPersonClothingTopGrayButtonVisible == false)
            {
                m_isPersonClothingTopGrayButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "White" && m_isPersonClothingTopWhiteButtonVisible == false)
            {
                m_isPersonClothingTopWhiteButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Red" && m_isPersonClothingTopRedButtonVisible == false)
            {
                m_isPersonClothingTopRedButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Orange" && m_isPersonClothingTopOrangeButtonVisible == false)
            {
                m_isPersonClothingTopOrangeButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Yellow" && m_isPersonClothingTopYellowButtonVisible == false)
            {
                m_isPersonClothingTopYellowButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Green" && m_isPersonClothingTopGreenButtonVisible == false)
            {
                m_isPersonClothingTopGreenButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Blue" && m_isPersonClothingTopBlueButtonVisible == false)
            {
                m_isPersonClothingTopBlueButtonVisible = true;
            }
            else if(metaAttr->personClothingTopsColor[1] == "Purple" && m_isPersonClothingTopPurpleButtonVisible == false)
            {
                m_isPersonClothingTopPurpleButtonVisible = true;
            }
        }

        // person - clothing bottom
        if(metaAttr->personClothingBottomsColor.size() >= 1)
        {
            if(metaAttr->personClothingBottomsColor[0] == "Black" && m_isPersonClothingBottomBlackButtonVisible == false)
            {
                m_isPersonClothingBottomBlackButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Gray" && m_isPersonClothingBottomGrayButtonVisible == false)
            {
                m_isPersonClothingBottomGrayButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "White" && m_isPersonClothingBottomWhiteButtonVisible == false)
            {
                m_isPersonClothingBottomWhiteButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Red" && m_isPersonClothingBottomRedButtonVisible == false)
            {
                m_isPersonClothingBottomRedButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Orange" && m_isPersonClothingBottomOrangeButtonVisible == false)
            {
                m_isPersonClothingBottomOrangeButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Yellow" && m_isPersonClothingBottomYellowButtonVisible == false)
            {
                m_isPersonClothingBottomYellowButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Green" && m_isPersonClothingBottomGreenButtonVisible == false)
            {
                m_isPersonClothingBottomGreenButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Blue" && m_isPersonClothingBottomBlueButtonVisible == false)
            {
                m_isPersonClothingBottomBlueButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[0] == "Purple" && m_isPersonClothingBottomPurpleButtonVisible == false)
            {
                m_isPersonClothingBottomPurpleButtonVisible = true;
            }
        }

        if(metaAttr->personClothingBottomsColor.size() >= 2)
        {
            if(metaAttr->personClothingBottomsColor[1] == "Black" && m_isPersonClothingBottomBlackButtonVisible == false)
            {
                m_isPersonClothingBottomBlackButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Gray" && m_isPersonClothingBottomGrayButtonVisible == false)
            {
                m_isPersonClothingBottomGrayButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "White" && m_isPersonClothingBottomWhiteButtonVisible == false)
            {
                m_isPersonClothingBottomWhiteButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Red" && m_isPersonClothingBottomRedButtonVisible == false)
            {
                m_isPersonClothingBottomRedButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Orange" && m_isPersonClothingBottomOrangeButtonVisible == false)
            {
                m_isPersonClothingBottomOrangeButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Yellow" && m_isPersonClothingBottomYellowButtonVisible == false)
            {
                m_isPersonClothingBottomYellowButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Green" && m_isPersonClothingBottomGreenButtonVisible == false)
            {
                m_isPersonClothingBottomGreenButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Blue" && m_isPersonClothingBottomBlueButtonVisible == false)
            {
                m_isPersonClothingBottomBlueButtonVisible = true;
            }
            else if(metaAttr->personClothingBottomsColor[1] == "Purple" && m_isPersonClothingBottomPurpleButtonVisible == false)
            {
                m_isPersonClothingBottomPurpleButtonVisible = true;
            }
        }

        // person - bag
        if(metaAttr->personBelongingBag.size() > 0)
        {
            if(metaAttr->personBelongingBag[0] == "Wear" && m_isPersonBagWearButtonVisible == false)
            {
                m_isPersonBagWearButtonVisible = true;
            }
            else if(metaAttr->personBelongingBag[0] == "No" && m_isPersonBagNoWearButtonVisible == false)
            {
                m_isPersonBagNoWearButtonVisible = true;
            }
        }

        // Person - Gender
        emit personGenderMaleButtonVisibleChanged(m_isPersonGenderMaleButtonVisible);
        emit personGenderFemaleButtonVisibleChanged(m_isPersonGenderFemaleButtonVisible);
        // Person - ClothingTop
        emit personClothingTopBlackButtonVisibleChanged(m_isPersonClothingTopBlackButtonVisible);
        emit personClothingTopGrayButtonVisibleChanged(m_isPersonClothingTopGrayButtonVisible);
        emit personClothingTopWhiteButtonVisibleChanged(m_isPersonClothingTopWhiteButtonVisible);
        emit personClothingTopRedButtonVisibleChanged(m_isPersonClothingTopRedButtonVisible);
        emit personClothingTopOrangeButtonVisibleChanged(m_isPersonClothingTopOrangeButtonVisible);
        emit personClothingTopYellowButtonVisibleChanged(m_isPersonClothingTopYellowButtonVisible);
        emit personClothingTopGreenButtonVisibleChanged(m_isPersonClothingTopGreenButtonVisible);
        emit personClothingTopBlueButtonVisibleChanged(m_isPersonClothingTopBlueButtonVisible);
        emit personClothingTopPurpleButtonVisibleChanged(m_isPersonClothingTopPurpleButtonVisible);
        // Person - ClothingBottom
        emit personClothingBottomBlackButtonVisibleChanged(m_isPersonClothingBottomBlackButtonVisible);
        emit personClothingBottomGrayButtonVisibleChanged(m_isPersonClothingBottomGrayButtonVisible);
        emit personClothingBottomWhiteButtonVisibleChanged(m_isPersonClothingBottomWhiteButtonVisible);
        emit personClothingBottomRedButtonVisibleChanged(m_isPersonClothingBottomRedButtonVisible);
        emit personClothingBottomOrangeButtonVisibleChanged(m_isPersonClothingBottomOrangeButtonVisible);
        emit personClothingBottomYellowButtonVisibleChanged(m_isPersonClothingBottomYellowButtonVisible);
        emit personClothingBottomGreenButtonVisibleChanged(m_isPersonClothingBottomGreenButtonVisible);
        emit personClothingBottomBlueButtonVisibleChanged(m_isPersonClothingBottomBlueButtonVisible);
        emit personClothingBottomPurpleButtonVisibleChanged(m_isPersonClothingBottomPurpleButtonVisible);
        // Person - Bag
        emit personBagWearButtonVisibleChanged(m_isPersonBagWearButtonVisible);
        emit personBagNoWearButtonVisibleChanged(m_isPersonBagNoWearButtonVisible);
    }
    else if(metaAttr->attributeType == Wisenet::Device::AiClassType::face)
    {
        // face - gender
        if(metaAttr->faceGender.size() > 0)
        {
            if(metaAttr->faceGender[0] == "Male" && m_isFaceGenderMaleButtonVisible == false)
            {
                m_isFaceGenderMaleButtonVisible = true;
            }
            else if(metaAttr->faceGender[0] == "Female" && m_isFaceGenderFemaleButtonVisible == false)
            {
                m_isFaceGenderFemaleButtonVisible = true;
            }
        }
        // face - ageType
        if(metaAttr->faceAgeType.size() > 0)
        {
            if(metaAttr->faceAgeType[0] == "Young" && m_isFaceAgeTypeYoungButtonVisible == false)
            {
                m_isFaceAgeTypeYoungButtonVisible = true;
            }
            else if(metaAttr->faceAgeType[0] == "Adult" && m_isFaceAgeTypeAdultButtonVisible == false)
            {
                m_isFaceAgeTypeAdultButtonVisible = true;
            }
            else if(metaAttr->faceAgeType[0] == "Middle" && m_isFaceAgeTypeMiddleButtonVisible == false)
            {
                m_isFaceAgeTypeMiddleButtonVisible = true;
            }
            else if(metaAttr->faceAgeType[0] == "Senior" && m_isFaceAgeTypeSeniorButtonVisible == false)
            {
                m_isFaceAgeTypeSeniorButtonVisible = true;
            }
        }
        // face - opticals
        if(metaAttr->faceOpticals.size() > 0)
        {
            if(metaAttr->faceOpticals[0] == "Wear" && m_isFaceOpticalsWearButtonVisible == false)
            {
                m_isFaceOpticalsWearButtonVisible = true;
            }
            else if(metaAttr->faceOpticals[0] == "No" && m_isFaceOpticalsNoWearButtonVisible == false)
            {
                m_isFaceOpticalsNoWearButtonVisible = true;
            }
        }
        // face - mask
        if(metaAttr->faceMask.size() > 0)
        {
            if(metaAttr->faceMask[0] == "Wear" && m_isFaceMaskWearButtonVisible == false)
            {
                m_isFaceMaskWearButtonVisible = true;
            }
            else if(metaAttr->faceMask[0] == "No" && m_isFaceMaskNoWearButtonVisible == false)
            {
                m_isFaceMaskNoWearButtonVisible = true;
            }
        }

        // Face - Gender
        emit faceGenderMaleButtonVisibleChanged(m_isFaceGenderMaleButtonVisible);
        emit faceGenderFemaleButtonVisibleChanged(m_isFaceGenderFemaleButtonVisible);
        // Face - AgeType
        emit faceAgeTypeYoungButtonVisibleChanged(m_isFaceAgeTypeYoungButtonVisible);
        emit faceAgeTypeAdultButtonVisibleChanged(m_isFaceAgeTypeAdultButtonVisible);
        emit faceAgeTypeMiddleButtonVisibleChanged(m_isFaceAgeTypeMiddleButtonVisible);
        emit faceAgeTypeSeniorButtonVisibleChanged(m_isFaceAgeTypeSeniorButtonVisible);
        // Face - Opticals
        emit faceOpticalsWearButtonVisibleChanged(m_isFaceOpticalsWearButtonVisible);
        emit faceOpticalsNoWearButtonVisibleChanged(m_isFaceOpticalsNoWearButtonVisible);
        // Face - Mask
        emit faceMaskWearButtonVisibleChanged(m_isFaceMaskWearButtonVisible);
        emit faceMaskNoWearButtonVisibleChanged(m_isFaceMaskNoWearButtonVisible);
    }
    else if(metaAttr->attributeType == Wisenet::Device::AiClassType::vehicle)
    {
        // vehicle - type
        if(metaAttr->vehicleType.size() > 0)
        {
            if(metaAttr->vehicleType[0] == "Car" && m_isVehicleTypeCarButtonVisible == false)
            {
                m_isVehicleTypeCarButtonVisible = true;
            }
            else if(metaAttr->vehicleType[0] == "Bus" && m_isVehicleTypeBusButtonVisible == false)
            {
                m_isVehicleTypeBusButtonVisible = true;
            }
            else if(metaAttr->vehicleType[0] == "Truck" && m_isVehicleTypeTruckButtonVisible == false)
            {
                m_isVehicleTypeTruckButtonVisible = true;
            }
            else if(metaAttr->vehicleType[0] == "Motorcycle" && m_isVehicleTypeMotorcycleButtonVisible == false)
            {
                m_isVehicleTypeMotorcycleButtonVisible = true;
            }
            else if(metaAttr->vehicleType[0] == "Bicycle" && m_isVehicleTypeBicycleButtonVisible == false)
            {
                m_isVehicleTypeBicycleButtonVisible = true;
            }
        }

        // vehicle - type
        if(metaAttr->vehicleColor.size() >= 1)
        {
            if(metaAttr->vehicleColor[0] == "Black" && m_isVehicleColorBlackButtonVisible == false)
            {
                m_isVehicleColorBlackButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Gray" && m_isVehicleColorGrayButtonVisible == false)
            {
                m_isVehicleColorGrayButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "White" && m_isVehicleColorWhiteButtonVisible == false)
            {
                m_isVehicleColorWhiteButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Red" && m_isVehicleColorRedButtonVisible == false)
            {
                m_isVehicleColorRedButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Orange" && m_isVehicleColorOrangeButtonVisible == false)
            {
                m_isVehicleColorOrangeButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Yellow" && m_isVehicleColorYellowButtonVisible == false)
            {
                m_isVehicleColorYellowButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Green" && m_isVehicleColorGreenButtonVisible == false)
            {
                m_isVehicleColorGreenButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Blue" && m_isVehicleColorBlueButtonVisible == false)
            {
                m_isVehicleColorBlueButtonVisible = true;
            }
            if(metaAttr->vehicleColor[0] == "Purple" && m_isVehicleColorPurpleButtonVisible == false)
            {
                m_isVehicleColorPurpleButtonVisible = true;
            }
        }

        if(metaAttr->vehicleColor.size() >= 2)
        {
            if(metaAttr->vehicleColor[1] == "Black" && m_isVehicleColorBlackButtonVisible == false)
            {
                m_isVehicleColorBlackButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Gray" && m_isVehicleColorGrayButtonVisible == false)
            {
                m_isVehicleColorGrayButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "White" && m_isVehicleColorWhiteButtonVisible == false)
            {
                m_isVehicleColorWhiteButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Red" && m_isVehicleColorRedButtonVisible == false)
            {
                m_isVehicleColorRedButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Orange" && m_isVehicleColorOrangeButtonVisible == false)
            {
                m_isVehicleColorOrangeButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Yellow" && m_isVehicleColorYellowButtonVisible == false)
            {
                m_isVehicleColorYellowButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Green" && m_isVehicleColorGreenButtonVisible == false)
            {
                m_isVehicleColorGreenButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Blue" && m_isVehicleColorBlueButtonVisible == false)
            {
                m_isVehicleColorBlueButtonVisible = true;
            }
            if(metaAttr->vehicleColor[1] == "Purple" && m_isVehicleColorPurpleButtonVisible == false)
            {
                m_isVehicleColorPurpleButtonVisible = true;
            }
        }

        // Vehicle - Type
        emit vehicleTypeCarButtonVisibleChanged(m_isVehicleTypeCarButtonVisible);
        emit vehicleTypeBusButtonVisibleChanged(m_isVehicleTypeBusButtonVisible);
        emit vehicleTypeTruckButtonVisibleChanged(m_isVehicleTypeTruckButtonVisible);
        emit vehicleTypeMotorcycleButtonVisibleChanged(m_isVehicleTypeMotorcycleButtonVisible);
        emit vehicleTypeBicycleButtonVisibleChanged(m_isVehicleTypeBicycleButtonVisible);
        // Vehicle - Color
        emit vehicleColorBlackButtonVisibleChanged(m_isVehicleColorBlackButtonVisible);
        emit vehicleColorGrayButtonVisibleChanged(m_isVehicleColorGrayButtonVisible);
        emit vehicleColorWhiteButtonVisibleChanged(m_isVehicleColorWhiteButtonVisible);
        emit vehicleColorRedButtonVisibleChanged(m_isVehicleColorRedButtonVisible);
        emit vehicleColorOrangeButtonVisibleChanged(m_isVehicleColorOrangeButtonVisible);
        emit vehicleColorYellowButtonVisibleChanged(m_isVehicleColorYellowButtonVisible);
        emit vehicleColorGreenButtonVisibleChanged(m_isVehicleColorGreenButtonVisible);
        emit vehicleColorBlueButtonVisibleChanged(m_isVehicleColorBlueButtonVisible);
        emit vehicleColorPurpleButtonVisibleChanged(m_isVehicleColorPurpleButtonVisible);
    }
}

void BestshotFilterTooltipModel::setFilterChecked(Wisenet::Device::MetaFilter& metaFilter)
{
    if(metaFilter.personGender.find("Male") != metaFilter.personGender.end())
        setPersonGenderMaleButtonChecked(true);
    if(metaFilter.personGender.find("Female") != metaFilter.personGender.end())
        setPersonGenderFemaleButtonChecked(true);

    if(metaFilter.personClothingTopsColor.find("Black") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopBlackButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Gray") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopGrayButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("White") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopWhiteButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Red") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopRedButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Orange") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopOrangeButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Yellow") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopYellowButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Green") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopGreenButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Blue") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopBlueButtonChecked(true);
    if(metaFilter.personClothingTopsColor.find("Purple") != metaFilter.personClothingTopsColor.end())
        setPersonClothingTopPurpleButtonChecked(true);

    if(metaFilter.personClothingBottomsColor.find("Black") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomBlackButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Gray") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomGrayButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("White") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomWhiteButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Red") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomRedButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Orange") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomOrangeButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Yellow") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomYellowButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Green") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomGreenButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Blue") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomBlueButtonChecked(true);
    if(metaFilter.personClothingBottomsColor.find("Purple") != metaFilter.personClothingBottomsColor.end())
        setPersonClothingBottomPurpleButtonChecked(true);

    if(metaFilter.personBelongingBag.find("Wear") != metaFilter.personBelongingBag.end())
        setPersonBagWearButtonChecked(true);
    if(metaFilter.personBelongingBag.find("No") != metaFilter.personBelongingBag.end())
        setPersonBagNoWearButtonChecked(true);

    if(metaFilter.faceGender.find("Male") != metaFilter.faceGender.end())
        setFaceGenderMaleButtonChecked(true);
    if(metaFilter.faceGender.find("Female") != metaFilter.faceGender.end())
        setFaceGenderFemaleButtonChecked(true);

    if(metaFilter.faceAgeType.find("Young") != metaFilter.faceAgeType.end())
        setFaceAgeTypeYoungButtonChecked(true);
    if(metaFilter.faceAgeType.find("Adult") != metaFilter.faceAgeType.end())
        setFaceAgeTypeAdultButtonChecked(true);
    if(metaFilter.faceAgeType.find("Middle") != metaFilter.faceAgeType.end())
        setFaceAgeTypeMiddleButtonChecked(true);
    if(metaFilter.faceAgeType.find("Senior") != metaFilter.faceAgeType.end())
        setFaceAgeTypeSeniorButtonChecked(true);

    if(metaFilter.faceOpticals.find("Wear") != metaFilter.faceOpticals.end())
        setFaceOpticalsWearButtonChecked(true);
    if(metaFilter.faceOpticals.find("No") != metaFilter.faceOpticals.end())
        setFaceOpticalsNoWearButtonChecked(true);

    if(metaFilter.faceMask.find("Wear") != metaFilter.faceMask.end())
        setFaceMaskWearButtonChecked(true);
    if(metaFilter.faceMask.find("No") != metaFilter.faceMask.end())
        setFaceMaskNoWearButtonChecked(true);

    if(metaFilter.vehicleType.find("Car") != metaFilter.vehicleType.end())
        setVehicleTypeCarButtonChecked(true);
    if(metaFilter.vehicleType.find("Bus") != metaFilter.vehicleType.end())
        setVehicleTypeBusButtonChecked(true);
    if(metaFilter.vehicleType.find("Truck") != metaFilter.vehicleType.end())
        setVehicleTypeTruckButtonChecked(true);
    if(metaFilter.vehicleType.find("Motorcycle") != metaFilter.vehicleType.end())
        setVehicleTypeMotorcycleButtonChecked(true);
    if(metaFilter.vehicleType.find("Bicycle") != metaFilter.vehicleType.end())
        setVehicleTypeBicycleButtonChecked(true);

    if(metaFilter.vehicleColor.find("Black") != metaFilter.vehicleColor.end())
        setVehicleColorBlackButtonChecked(true);
    if(metaFilter.vehicleColor.find("Gray") != metaFilter.vehicleColor.end())
        setVehicleColorGrayButtonChecked(true);
    if(metaFilter.vehicleColor.find("White") != metaFilter.vehicleColor.end())
        setVehicleColorWhiteButtonChecked(true);
    if(metaFilter.vehicleColor.find("Red") != metaFilter.vehicleColor.end())
        setVehicleColorRedButtonChecked(true);
    if(metaFilter.vehicleColor.find("Orange") != metaFilter.vehicleColor.end())
        setVehicleColorOrangeButtonChecked(true);
    if(metaFilter.vehicleColor.find("Yellow") != metaFilter.vehicleColor.end())
        setVehicleColorYellowButtonChecked(true);
    if(metaFilter.vehicleColor.find("Green") != metaFilter.vehicleColor.end())
        setVehicleColorGreenButtonChecked(true);
    if(metaFilter.vehicleColor.find("Blue") != metaFilter.vehicleColor.end())
        setVehicleColorBlueButtonChecked(true);
    if(metaFilter.vehicleColor.find("Purple") != metaFilter.vehicleColor.end())
        setVehicleColorPurpleButtonChecked(true);
}

void BestshotFilterTooltipModel::applyFilter()
{
    qDebug() << "BestshotFilterTooltipModel::applyFilter()";

    Wisenet::Device::MetaFilter metaFilter;

    // Person Gender
    if(isPersonGenderMaleButtonChecked())
        metaFilter.personGender.insert("Male");

    if(isPersonGenderFemaleButtonChecked())
        metaFilter.personGender.insert("Female");

    // Person Clothing Top
    if(isPersonClothingTopBlackButtonChecked())
        metaFilter.personClothingTopsColor.insert("Black");

    if(isPersonClothingTopGrayButtonChecked())
        metaFilter.personClothingTopsColor.insert("Gray");

    if(isPersonClothingTopWhiteButtonChecked())
        metaFilter.personClothingTopsColor.insert("White");

    if(isPersonClothingTopRedButtonChecked())
        metaFilter.personClothingTopsColor.insert("Red");

    if(isPersonClothingTopOrangeButtonChecked())
        metaFilter.personClothingTopsColor.insert("Orange");

    if(isPersonClothingTopYellowButtonChecked())
        metaFilter.personClothingTopsColor.insert("Yellow");

    if(isPersonClothingTopGreenButtonChecked())
        metaFilter.personClothingTopsColor.insert("Green");

    if(isPersonClothingTopBlueButtonChecked())
        metaFilter.personClothingTopsColor.insert("Blue");

    if(isPersonClothingTopPurpleButtonChecked())
        metaFilter.personClothingTopsColor.insert("Purple");

    // Person Clothing Bottom
    if(isPersonClothingBottomBlackButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Black");

    if(isPersonClothingBottomGrayButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Gray");

    if(isPersonClothingBottomWhiteButtonChecked())
        metaFilter.personClothingBottomsColor.insert("White");

    if(isPersonClothingBottomRedButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Red");

    if(isPersonClothingBottomOrangeButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Orange");

    if(isPersonClothingBottomYellowButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Yellow");

    if(isPersonClothingBottomGreenButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Green");

    if(isPersonClothingBottomBlueButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Blue");

    if(isPersonClothingBottomPurpleButtonChecked())
        metaFilter.personClothingBottomsColor.insert("Purple");

    // Person Bag
    if(isPersonBagWearButtonChecked())
        metaFilter.personBelongingBag.insert("Wear");

    if(isPersonBagNoWearButtonChecked())
        metaFilter.personBelongingBag.insert("No");

    // Face Gender
    if(isFaceGenderMaleButtonChecked())
        metaFilter.faceGender.insert("Male");

    if(isFaceGenderFemaleButtonChecked())
        metaFilter.faceGender.insert("Female");

    // Face Age Type
    if(isFaceAgeTypeYoungButtonChecked())
        metaFilter.faceAgeType.insert("Young");

    if(isFaceAgeTypeAdultButtonChecked())
        metaFilter.faceAgeType.insert("Adult");

    if(isFaceAgeTypeMiddleButtonChecked())
        metaFilter.faceAgeType.insert("Middle");

    if(isFaceAgeTypeSeniorButtonChecked())
        metaFilter.faceAgeType.insert("Senior");

    // Face glasses
    if(isFaceOpticalsWearButtonChecked())
        metaFilter.faceOpticals.insert("Wear");

    if(isFaceOpticalsNoWearButtonChecked())
        metaFilter.faceOpticals.insert("No");

    // Face Mask
    if(isFaceMaskWearButtonChecked())
        metaFilter.faceMask.insert("Wear");

    if(isFaceMaskNoWearButtonChecked())
        metaFilter.faceMask.insert("No");


    // Vehicle Type
    if(isVehicleTypeCarButtonChecked())
        metaFilter.vehicleType.insert("Car");

    if(isVehicleTypeBusButtonChecked())
        metaFilter.vehicleType.insert("Bus");

    if(isVehicleTypeTruckButtonChecked())
        metaFilter.vehicleType.insert("Truck");

    if(isVehicleTypeMotorcycleButtonChecked())
        metaFilter.vehicleType.insert("Motorcycle");

    if(isVehicleTypeBicycleButtonChecked())
        metaFilter.vehicleType.insert("Bicycle");

    // Vehicle Color
    if(isVehicleColorBlackButtonChecked())
        metaFilter.vehicleColor.insert("Black");

    if(isVehicleColorGrayButtonChecked())
        metaFilter.vehicleColor.insert("Gray");

    if(isVehicleColorWhiteButtonChecked())
        metaFilter.vehicleColor.insert("White");

    if(isVehicleColorRedButtonChecked())
        metaFilter.vehicleColor.insert("Red");

    if(isVehicleColorOrangeButtonChecked())
        metaFilter.vehicleColor.insert("Orange");

    if(isVehicleColorYellowButtonChecked())
        metaFilter.vehicleColor.insert("Yellow");

    if(isVehicleColorGreenButtonChecked())
        metaFilter.vehicleColor.insert("Green");

    if(isVehicleColorBlueButtonChecked())
        metaFilter.vehicleColor.insert("Blue");

    if(isVehicleColorPurpleButtonChecked())
        metaFilter.vehicleColor.insert("Purple");

    filterChanged(metaFilter);
}
