#pragma once

#include <QObject>
#include <QDebug>

class BaseFilterModel : public QObject
{
    Q_OBJECT
    // button visible
    // Person - Gender
    Q_PROPERTY(bool isPersonGenderMaleButtonVisible READ isPersonGenderMaleButtonVisible WRITE setPersonGenderMaleButtonVisible NOTIFY personGenderMaleButtonVisibleChanged)
    Q_PROPERTY(bool isPersonGenderFemaleButtonVisible READ isPersonGenderFemaleButtonVisible WRITE setPersonGenderFemaleButtonVisible NOTIFY personGenderFemaleButtonVisibleChanged)
    // Person - ClothingTop
    Q_PROPERTY(bool isPersonClothingTopBlackButtonVisible READ isPersonClothingTopBlackButtonVisible WRITE setPersonClothingTopBlackButtonVisible NOTIFY personClothingTopBlackButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopGrayButtonVisible READ isPersonClothingTopGrayButtonVisible WRITE setPersonClothingTopGrayButtonVisible NOTIFY personClothingTopGrayButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopWhiteButtonVisible READ isPersonClothingTopWhiteButtonVisible WRITE setPersonClothingTopWhiteButtonVisible NOTIFY personClothingTopWhiteButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopRedButtonVisible READ isPersonClothingTopRedButtonVisible WRITE setPersonClothingTopRedButtonVisible NOTIFY personClothingTopRedButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopOrangeButtonVisible READ isPersonClothingTopOrangeButtonVisible WRITE setPersonClothingTopOrangeButtonVisible NOTIFY personClothingTopOrangeButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopYellowButtonVisible READ isPersonClothingTopYellowButtonVisible WRITE setPersonClothingTopYellowButtonVisible NOTIFY personClothingTopYellowButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopGreenButtonVisible READ isPersonClothingTopGreenButtonVisible WRITE setPersonClothingTopGreenButtonVisible NOTIFY personClothingTopGreenButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopBlueButtonVisible READ isPersonClothingTopBlueButtonVisible WRITE setPersonClothingTopBlueButtonVisible NOTIFY personClothingTopBlueButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingTopPurpleButtonVisible READ isPersonClothingTopPurpleButtonVisible WRITE setPersonClothingTopPurpleButtonVisible NOTIFY personClothingTopPurpleButtonVisibleChanged)
    // Person - ClothingBottom
    Q_PROPERTY(bool isPersonClothingBottomBlackButtonVisible READ isPersonClothingBottomBlackButtonVisible WRITE setPersonClothingBottomBlackButtonVisible NOTIFY personClothingBottomBlackButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomGrayButtonVisible READ isPersonClothingBottomGrayButtonVisible WRITE setPersonClothingBottomGrayButtonVisible NOTIFY personClothingBottomGrayButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomWhiteButtonVisible READ isPersonClothingBottomWhiteButtonVisible WRITE setPersonClothingBottomWhiteButtonVisible NOTIFY personClothingBottomWhiteButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomRedButtonVisible READ isPersonClothingBottomRedButtonVisible WRITE setPersonClothingBottomRedButtonVisible NOTIFY personClothingBottomRedButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomOrangeButtonVisible READ isPersonClothingBottomOrangeButtonVisible WRITE setPersonClothingBottomOrangeButtonVisible NOTIFY personClothingBottomOrangeButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomYellowButtonVisible READ isPersonClothingBottomYellowButtonVisible WRITE setPersonClothingBottomYellowButtonVisible NOTIFY personClothingBottomYellowButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomGreenButtonVisible READ isPersonClothingBottomGreenButtonVisible WRITE setPersonClothingBottomGreenButtonVisible NOTIFY personClothingBottomGreenButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomBlueButtonVisible READ isPersonClothingBottomBlueButtonVisible WRITE setPersonClothingBottomBlueButtonVisible NOTIFY personClothingBottomBlueButtonVisibleChanged)
    Q_PROPERTY(bool isPersonClothingBottomPurpleButtonVisible READ isPersonClothingBottomPurpleButtonVisible WRITE setPersonClothingBottomPurpleButtonVisible NOTIFY personClothingBottomPurpleButtonVisibleChanged)
    // Person - Bag
    Q_PROPERTY(bool isPersonBagWearButtonVisible READ isPersonBagWearButtonVisible WRITE setPersonBagWearButtonVisible NOTIFY personBagWearButtonVisibleChanged)
    Q_PROPERTY(bool isPersonBagNoWearButtonVisible READ isPersonBagNoWearButtonVisible WRITE setPersonBagNoWearButtonVisible NOTIFY personBagNoWearButtonVisibleChanged)
    // Face - Gender
    Q_PROPERTY(bool isFaceGenderMaleButtonVisible READ isFaceGenderMaleButtonVisible WRITE setFaceGenderMaleButtonVisible NOTIFY faceGenderMaleButtonVisibleChanged)
    Q_PROPERTY(bool isFaceGenderFemaleButtonVisible READ isFaceGenderFemaleButtonVisible WRITE setFaceGenderFemaleButtonVisible NOTIFY faceGenderFemaleButtonVisibleChanged)
    // Face - AgeType
    Q_PROPERTY(bool isFaceAgeTypeYoungButtonVisible READ isFaceAgeTypeYoungButtonVisible WRITE setFaceAgeTypeYoungButtonVisible NOTIFY faceAgeTypeYoungButtonVisibleChanged)
    Q_PROPERTY(bool isFaceAgeTypeAdultButtonVisible READ isFaceAgeTypeAdultButtonVisible WRITE setFaceAgeTypeAdultButtonVisible NOTIFY faceAgeTypeAdultButtonVisibleChanged)
    Q_PROPERTY(bool isFaceAgeTypeMiddleButtonVisible READ isFaceAgeTypeMiddleButtonVisible WRITE setFaceAgeTypeMiddleButtonVisible NOTIFY faceAgeTypeMiddleButtonVisibleChanged)
    Q_PROPERTY(bool isFaceAgeTypeSeniorButtonVisible READ isFaceAgeTypeSeniorButtonVisible WRITE setFaceAgeTypeSeniorButtonVisible NOTIFY personBagWearButtonVisibleChanged)
    // Face - Hat
    Q_PROPERTY(bool isFaceHatWearButtonVisible READ isFaceHatWearButtonVisible WRITE setFaceHatWearButtonVisible NOTIFY faceHatWearButtonVisibleChanged)
    Q_PROPERTY(bool isFaceHatNoWearButtonVisible READ isFaceHatNoWearButtonVisible WRITE setFaceHatNoWearButtonVisible NOTIFY faceHatNoWearButtonVisibleChanged)
    // Face - Opticals
    Q_PROPERTY(bool isFaceOpticalsWearButtonVisible READ isFaceOpticalsWearButtonVisible WRITE setFaceOpticalsWearButtonVisible NOTIFY faceOpticalsWearButtonVisibleChanged)
    Q_PROPERTY(bool isFaceOpticalsNoWearButtonVisible READ isFaceOpticalsNoWearButtonVisible WRITE setFaceOpticalsNoWearButtonVisible NOTIFY faceOpticalsNoWearButtonVisibleChanged)
    // Face - Mask
    Q_PROPERTY(bool isFaceMaskWearButtonVisible READ isFaceMaskWearButtonVisible WRITE setFaceMaskWearButtonVisible NOTIFY faceMaskWearButtonVisibleChanged)
    Q_PROPERTY(bool isFaceMaskNoWearButtonVisible READ isFaceMaskNoWearButtonVisible WRITE setFaceMaskNoWearButtonVisible NOTIFY faceMaskNoWearButtonVisibleChanged)
    // Vehicle - Type
    Q_PROPERTY(bool isVehicleTypeCarButtonVisible READ isVehicleTypeCarButtonVisible WRITE setVehicleTypeCarButtonVisible NOTIFY vehicleTypeCarButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleTypeBusButtonVisible READ isVehicleTypeBusButtonVisible WRITE setVehicleTypeBusButtonVisible NOTIFY vehicleTypeBusButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleTypeTruckButtonVisible READ isVehicleTypeTruckButtonVisible WRITE setVehicleTypeTruckButtonVisible NOTIFY vehicleTypeTruckButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleTypeMotorcycleButtonVisible READ isVehicleTypeMotorcycleButtonVisible WRITE setVehicleTypeMotorcycleButtonVisible NOTIFY vehicleTypeMotorcycleButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleTypeBicycleButtonVisible READ isVehicleTypeBicycleButtonVisible WRITE setVehicleTypeBicycleButtonVisible NOTIFY vehicleTypeBicycleButtonVisibleChanged)
    // Vehicle - Color
    Q_PROPERTY(bool isVehicleColorBlackButtonVisible READ isVehicleColorBlackButtonVisible WRITE setVehicleColorBlackButtonVisible NOTIFY vehicleColorBlackButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorGrayButtonVisible READ isVehicleColorGrayButtonVisible WRITE setVehicleColorGrayButtonVisible NOTIFY vehicleColorGrayButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorWhiteButtonVisible READ isVehicleColorWhiteButtonVisible WRITE setVehicleColorGrayButtonVisible NOTIFY vehicleColorGrayButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorRedButtonVisible READ isVehicleColorRedButtonVisible WRITE setVehicleColorRedButtonVisible NOTIFY vehicleColorRedButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorOrangeButtonVisible READ isVehicleColorOrangeButtonVisible WRITE setVehicleColorOrangeButtonVisible NOTIFY vehicleColorOrangeButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorYellowButtonVisible READ isVehicleColorYellowButtonVisible WRITE setVehicleColorYellowButtonVisible NOTIFY vehicleColorYellowButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorGreenButtonVisible READ isVehicleColorGreenButtonVisible WRITE setVehicleColorGreenButtonVisible NOTIFY vehicleColorGreenButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorBlueButtonVisible READ isVehicleColorBlueButtonVisible WRITE setVehicleColorBlueButtonVisible NOTIFY vehicleColorBlueButtonVisibleChanged)
    Q_PROPERTY(bool isVehicleColorPurpleButtonVisible READ isVehicleColorPurpleButtonVisible WRITE setVehicleColorPurpleButtonVisible NOTIFY vehicleColorPurpleButtonVisibleChanged)

    // button checked
    // Person - Gender
    Q_PROPERTY(bool isPersonGenderMaleButtonChecked READ isPersonGenderMaleButtonChecked WRITE setPersonGenderMaleButtonChecked NOTIFY personGenderMaleButtonCheckedChanged)
    Q_PROPERTY(bool isPersonGenderFemaleButtonChecked READ isPersonGenderFemaleButtonChecked WRITE setPersonGenderFemaleButtonChecked NOTIFY personGenderFemaleButtonCheckedChanged)
    // Person - ClothingTop
    Q_PROPERTY(bool isPersonClothingTopBlackButtonChecked READ isPersonClothingTopBlackButtonChecked WRITE setPersonClothingTopBlackButtonChecked NOTIFY personClothingTopBlackButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopGrayButtonChecked READ isPersonClothingTopGrayButtonChecked WRITE setPersonClothingTopGrayButtonChecked NOTIFY personClothingTopGrayButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopWhiteButtonChecked READ isPersonClothingTopWhiteButtonChecked WRITE setPersonClothingTopWhiteButtonChecked NOTIFY personClothingTopWhiteButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopRedButtonChecked READ isPersonClothingTopRedButtonChecked WRITE setPersonClothingTopRedButtonChecked NOTIFY personClothingTopRedButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopOrangeButtonChecked READ isPersonClothingTopOrangeButtonChecked WRITE setPersonClothingTopOrangeButtonChecked NOTIFY personClothingTopOrangeButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopYellowButtonChecked READ isPersonClothingTopYellowButtonChecked WRITE setPersonClothingTopYellowButtonChecked NOTIFY personClothingTopYellowButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopGreenButtonChecked READ isPersonClothingTopGreenButtonChecked WRITE setPersonClothingTopGreenButtonChecked NOTIFY personClothingTopGreenButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopBlueButtonChecked READ isPersonClothingTopBlueButtonChecked WRITE setPersonClothingTopBlueButtonChecked NOTIFY personClothingTopBlueButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingTopPurpleButtonChecked READ isPersonClothingTopPurpleButtonChecked WRITE setPersonClothingTopPurpleButtonChecked NOTIFY personClothingTopPurpleButtonCheckedChanged)
    // Person - ClothingBottom
    Q_PROPERTY(bool isPersonClothingBottomBlackButtonChecked READ isPersonClothingBottomBlackButtonChecked WRITE setPersonClothingBottomBlackButtonChecked NOTIFY personClothingBottomBlackButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomGrayButtonChecked READ isPersonClothingBottomGrayButtonChecked WRITE setPersonClothingBottomGrayButtonChecked NOTIFY personClothingBottomGrayButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomWhiteButtonChecked READ isPersonClothingBottomWhiteButtonChecked WRITE setPersonClothingBottomWhiteButtonChecked NOTIFY personClothingBottomWhiteButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomRedButtonChecked READ isPersonClothingBottomRedButtonChecked WRITE setPersonClothingBottomRedButtonChecked NOTIFY personClothingBottomRedButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomOrangeButtonChecked READ isPersonClothingBottomOrangeButtonChecked WRITE setPersonClothingBottomOrangeButtonChecked NOTIFY personClothingBottomOrangeButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomYellowButtonChecked READ isPersonClothingBottomYellowButtonChecked WRITE setPersonClothingBottomYellowButtonChecked NOTIFY personClothingBottomYellowButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomGreenButtonChecked READ isPersonClothingBottomGreenButtonChecked WRITE setPersonClothingBottomGreenButtonChecked NOTIFY personClothingBottomGreenButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomBlueButtonChecked READ isPersonClothingBottomBlueButtonChecked WRITE setPersonClothingBottomBlueButtonChecked NOTIFY personClothingBottomBlueButtonCheckedChanged)
    Q_PROPERTY(bool isPersonClothingBottomPurpleButtonChecked READ isPersonClothingBottomPurpleButtonChecked WRITE setPersonClothingBottomPurpleButtonChecked NOTIFY personClothingBottomPurpleButtonCheckedChanged)
    // Person - Bag
    Q_PROPERTY(bool isPersonBagWearButtonChecked READ isPersonBagWearButtonChecked WRITE setPersonBagWearButtonChecked NOTIFY personBagWearButtonCheckedChanged)
    Q_PROPERTY(bool isPersonBagNoWearButtonChecked READ isPersonBagNoWearButtonChecked WRITE setPersonBagNoWearButtonChecked NOTIFY personBagNoWearButtonCheckedChanged)
    // Face - Gender
    Q_PROPERTY(bool isFaceGenderMaleButtonChecked READ isFaceGenderMaleButtonChecked WRITE setFaceGenderMaleButtonChecked NOTIFY faceGenderMaleButtonCheckedChanged)
    Q_PROPERTY(bool isFaceGenderFemaleButtonChecked READ isFaceGenderFemaleButtonChecked WRITE setFaceGenderFemaleButtonChecked NOTIFY faceGenderFemaleButtonCheckedChanged)
    // Face - AgeType
    Q_PROPERTY(bool isFaceAgeTypeYoungButtonChecked READ isFaceAgeTypeYoungButtonChecked WRITE setFaceAgeTypeYoungButtonChecked NOTIFY faceAgeTypeYoungButtonCheckedChanged)
    Q_PROPERTY(bool isFaceAgeTypeAdultButtonChecked READ isFaceAgeTypeAdultButtonChecked WRITE setFaceAgeTypeAdultButtonChecked NOTIFY faceAgeTypeAdultButtonCheckedChanged)
    Q_PROPERTY(bool isFaceAgeTypeMiddleButtonChecked READ isFaceAgeTypeMiddleButtonChecked WRITE setFaceAgeTypeMiddleButtonChecked NOTIFY faceAgeTypeMiddleButtonCheckedChanged)
    Q_PROPERTY(bool isFaceAgeTypeSeniorButtonChecked READ isFaceAgeTypeSeniorButtonChecked WRITE setFaceAgeTypeSeniorButtonChecked NOTIFY personBagWearButtonCheckedChanged)
    // Face - Hat
    Q_PROPERTY(bool isFaceHatWearButtonChecked READ isFaceHatWearButtonChecked WRITE setFaceHatWearButtonChecked NOTIFY faceHatWearButtonCheckedChanged)
    Q_PROPERTY(bool isFaceHatNoWearButtonChecked READ isFaceHatNoWearButtonChecked WRITE setFaceHatNoWearButtonChecked NOTIFY faceHatNoWearButtonCheckedChanged)
    // Face - Opticals
    Q_PROPERTY(bool isFaceOpticalsWearButtonChecked READ isFaceOpticalsWearButtonChecked WRITE setFaceOpticalsWearButtonChecked NOTIFY faceOpticalsWearButtonCheckedChanged)
    Q_PROPERTY(bool isFaceOpticalsNoWearButtonChecked READ isFaceOpticalsNoWearButtonChecked WRITE setFaceOpticalsNoWearButtonChecked NOTIFY faceOpticalsNoWearButtonCheckedChanged)
    // Face - Mask
    Q_PROPERTY(bool isFaceMaskWearButtonChecked READ isFaceMaskWearButtonChecked WRITE setFaceMaskWearButtonChecked NOTIFY faceMaskWearButtonCheckedChanged)
    Q_PROPERTY(bool isFaceMaskNoWearButtonChecked READ isFaceMaskNoWearButtonChecked WRITE setFaceMaskNoWearButtonChecked NOTIFY faceMaskNoWearButtonCheckedChanged)
    // Vehicle - Type
    Q_PROPERTY(bool isVehicleTypeCarButtonChecked READ isVehicleTypeCarButtonChecked WRITE setVehicleTypeCarButtonChecked NOTIFY vehicleTypeCarButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleTypeBusButtonChecked READ isVehicleTypeBusButtonChecked WRITE setVehicleTypeBusButtonChecked NOTIFY vehicleTypeBusButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleTypeTruckButtonChecked READ isVehicleTypeTruckButtonChecked WRITE setVehicleTypeTruckButtonChecked NOTIFY vehicleTypeTruckButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleTypeMotorcycleButtonChecked READ isVehicleTypeMotorcycleButtonChecked WRITE setVehicleTypeMotorcycleButtonChecked NOTIFY vehicleTypeMotorcycleButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleTypeBicycleButtonChecked READ isVehicleTypeBicycleButtonChecked WRITE setVehicleTypeBicycleButtonChecked NOTIFY vehicleTypeBicycleButtonCheckedChanged)
    // Vehicle - Color
    Q_PROPERTY(bool isVehicleColorBlackButtonChecked READ isVehicleColorBlackButtonChecked WRITE setVehicleColorBlackButtonChecked NOTIFY vehicleColorBlackButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorGrayButtonChecked READ isVehicleColorGrayButtonChecked WRITE setVehicleColorGrayButtonChecked NOTIFY vehicleColorGrayButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorWhiteButtonChecked READ isVehicleColorWhiteButtonChecked WRITE setVehicleColorWhiteButtonChecked NOTIFY vehicleColorWhiteButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorRedButtonChecked READ isVehicleColorRedButtonChecked WRITE setVehicleColorRedButtonChecked NOTIFY vehicleColorRedButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorOrangeButtonChecked READ isVehicleColorOrangeButtonChecked WRITE setVehicleColorOrangeButtonChecked NOTIFY vehicleColorOrangeButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorYellowButtonChecked READ isVehicleColorYellowButtonChecked WRITE setVehicleColorYellowButtonChecked NOTIFY vehicleColorYellowButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorGreenButtonChecked READ isVehicleColorGreenButtonChecked WRITE setVehicleColorGreenButtonChecked NOTIFY vehicleColorGreenButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorBlueButtonChecked READ isVehicleColorBlueButtonChecked WRITE setVehicleColorBlueButtonChecked NOTIFY vehicleColorBlueButtonCheckedChanged)
    Q_PROPERTY(bool isVehicleColorPurpleButtonChecked READ isVehicleColorPurpleButtonChecked WRITE setVehicleColorPurpleButtonChecked NOTIFY vehicleColorPurpleButtonCheckedChanged)
    // OCR - License Plate Text
    Q_PROPERTY(QString ocrLicensePlateText READ getOcrLicensePlateText WRITE setOcrLicensePlateText NOTIFY ocrLicensePlateTextChanged)


public:
    explicit BaseFilterModel(QObject *parent = nullptr);
    virtual ~BaseFilterModel(){}

    void clearAllObjectOptionButtons();
    void clearChecked();

    // get visible value
    // Person - Gender
    bool isPersonGenderMaleButtonVisible()
    {
        //qDebug() << "isPersonGenderMaleButtonVisible() " << m_isPersonGenderMaleButtonVisible;
        return m_isPersonGenderMaleButtonVisible;
    }
    bool isPersonGenderFemaleButtonVisible()
    {
        //qDebug() << "isPersonGenderFemaleButtonVisible() " << m_isPersonGenderFemaleButtonVisible;
        return m_isPersonGenderFemaleButtonVisible;
    }
    // Person - ClothingTop
    bool isPersonClothingTopBlackButtonVisible()
    {
        //qDebug() << "isPersonClothingTopBlackButtonVisible() " << m_isPersonClothingTopBlackButtonVisible;
        return m_isPersonClothingTopBlackButtonVisible;
    }
    bool isPersonClothingTopGrayButtonVisible()
    {
        //qDebug() << "isPersonClothingTopGrayButtonVisible() " << m_isPersonClothingTopGrayButtonVisible;
        return m_isPersonClothingTopGrayButtonVisible;
    }
    bool isPersonClothingTopWhiteButtonVisible()
    {
        //qDebug() << "isPersonClothingTopWhiteButtonVisible() " << m_isPersonClothingTopWhiteButtonVisible;
        return m_isPersonClothingTopWhiteButtonVisible;
    }
    bool isPersonClothingTopRedButtonVisible()
    {
        //qDebug() << "isPersonClothingTopRedButtonVisible() " << m_isPersonClothingTopRedButtonVisible;
        return m_isPersonClothingTopRedButtonVisible;
    }
    bool isPersonClothingTopOrangeButtonVisible()
    {
        //qDebug() << "isPersonClothingTopOrangeButtonVisible() " << m_isPersonClothingTopOrangeButtonVisible;
        return m_isPersonClothingTopOrangeButtonVisible;
    }
    bool isPersonClothingTopYellowButtonVisible()
    {
        //qDebug() << "isPersonClothingTopYellowButtonVisible() " << m_isPersonClothingTopYellowButtonVisible;
        return m_isPersonClothingTopYellowButtonVisible;
    }
    bool isPersonClothingTopGreenButtonVisible()
    {
        //qDebug() << "isPersonClothingTopGreenButtonVisible() " << m_isPersonClothingTopGreenButtonVisible;
        return m_isPersonClothingTopGreenButtonVisible;
    }
    bool isPersonClothingTopBlueButtonVisible()
    {
        //qDebug() << "isPersonClothingTopBlueButtonVisible() " << m_isPersonClothingTopBlueButtonVisible;
        return m_isPersonClothingTopBlueButtonVisible;
    }
    bool isPersonClothingTopPurpleButtonVisible()
    {
        //qDebug() << "isPersonClothingTopPurpleButtonVisible() " << m_isPersonClothingTopPurpleButtonVisible;
        return m_isPersonClothingTopPurpleButtonVisible;
    }
    // Person - ClothingBottom
    bool isPersonClothingBottomBlackButtonVisible()
    {
        return m_isPersonClothingBottomBlackButtonVisible;
    }
    bool isPersonClothingBottomGrayButtonVisible()
    {
        return m_isPersonClothingBottomGrayButtonVisible;
    }
    bool isPersonClothingBottomWhiteButtonVisible()
    {
        return m_isPersonClothingBottomWhiteButtonVisible;
    }
    bool isPersonClothingBottomRedButtonVisible()
    {
        return m_isPersonClothingBottomRedButtonVisible;
    }
    bool isPersonClothingBottomOrangeButtonVisible()
    {
        return m_isPersonClothingBottomOrangeButtonVisible;
    }
    bool isPersonClothingBottomYellowButtonVisible()
    {
        return m_isPersonClothingBottomYellowButtonVisible;
    }
    bool isPersonClothingBottomGreenButtonVisible()
    {
        return m_isPersonClothingBottomGreenButtonVisible;
    }
    bool isPersonClothingBottomBlueButtonVisible()
    {
        return m_isPersonClothingBottomBlueButtonVisible;
    }
    bool isPersonClothingBottomPurpleButtonVisible()
    {
        return m_isPersonClothingBottomPurpleButtonVisible;
    }
    // Person - Bag
    bool isPersonBagWearButtonVisible()
    {
        return m_isPersonBagWearButtonVisible;
    }
    bool isPersonBagNoWearButtonVisible()
    {
        return m_isPersonBagNoWearButtonVisible;
    }
    // Face - Gender
    bool isFaceGenderMaleButtonVisible()
    {
        return m_isFaceGenderMaleButtonVisible;
    }
    bool isFaceGenderFemaleButtonVisible()
    {
        return m_isFaceGenderFemaleButtonVisible;
    }
    // Face - AgeType
    bool isFaceAgeTypeYoungButtonVisible()
    {
        return m_isFaceAgeTypeYoungButtonVisible;
    }
    bool isFaceAgeTypeAdultButtonVisible()
    {
        return m_isFaceAgeTypeAdultButtonVisible;
    }
    bool isFaceAgeTypeMiddleButtonVisible()
    {
        return m_isFaceAgeTypeMiddleButtonVisible;
    }
    bool isFaceAgeTypeSeniorButtonVisible()
    {
        return m_isFaceAgeTypeSeniorButtonVisible;
    }
    // Face - Hat
    bool isFaceHatWearButtonVisible()
    {
        return m_isFaceHatWearButtonVisible;
    }
    bool isFaceHatNoWearButtonVisible()
    {
        return m_isFaceHatNoWearButtonVisible;
    }
    // Face - Opticals
    bool isFaceOpticalsWearButtonVisible()
    {
        return m_isFaceOpticalsWearButtonVisible;
    }
    bool isFaceOpticalsNoWearButtonVisible()
    {
        return m_isFaceOpticalsNoWearButtonVisible;
    }
    // Face - Mask
    bool isFaceMaskWearButtonVisible()
    {
        return m_isFaceMaskWearButtonVisible;
    }
    bool isFaceMaskNoWearButtonVisible()
    {
        return m_isFaceMaskNoWearButtonVisible;
    }
    // Vehicle - Type
    bool isVehicleTypeCarButtonVisible()
    {
        return m_isVehicleTypeCarButtonVisible;
    }
    bool isVehicleTypeBusButtonVisible()
    {
        return m_isVehicleTypeBusButtonVisible;
    }
    bool isVehicleTypeTruckButtonVisible()
    {
        return m_isVehicleTypeTruckButtonVisible;
    }
    bool isVehicleTypeMotorcycleButtonVisible()
    {
        return m_isVehicleTypeMotorcycleButtonVisible;
    }
    bool isVehicleTypeBicycleButtonVisible()
    {
        return m_isVehicleTypeBicycleButtonVisible;
    }
    // Vehicle - Color
    bool isVehicleColorBlackButtonVisible()
    {
        return m_isVehicleColorBlackButtonVisible;
    }
    bool isVehicleColorGrayButtonVisible()
    {
        return m_isVehicleColorGrayButtonVisible;
    }
    bool isVehicleColorWhiteButtonVisible()
    {
        return m_isVehicleColorWhiteButtonVisible;
    }
    bool isVehicleColorRedButtonVisible()
    {
        return m_isVehicleColorRedButtonVisible;
    }
    bool isVehicleColorOrangeButtonVisible()
    {
        return m_isVehicleColorOrangeButtonVisible;
    }
    bool isVehicleColorYellowButtonVisible()
    {
        return m_isVehicleColorYellowButtonVisible;
    }
    bool isVehicleColorGreenButtonVisible()
    {
        return m_isVehicleColorGreenButtonVisible;
    }
    bool isVehicleColorBlueButtonVisible()
    {
        return m_isVehicleColorBlueButtonVisible;
    }
    bool isVehicleColorPurpleButtonVisible()
    {
        return m_isVehicleColorPurpleButtonVisible;
    }

    // get checked value
    // Person - Gender
    bool isPersonGenderMaleButtonChecked()
    {
        //qDebug() << "isPersonGenderMaleButtonChecked() " << m_isPersonGenderMaleButtonChecked;
        return m_isPersonGenderMaleButtonChecked;
    }
    bool isPersonGenderFemaleButtonChecked()
    {
        //qDebug() << "isPersonGenderFemaleButtonChecked() " << m_isPersonGenderFemaleButtonChecked;
        return m_isPersonGenderFemaleButtonChecked;
    }
    // Person - ClothingTop
    bool isPersonClothingTopBlackButtonChecked()
    {
        //qDebug() << "isPersonClothingTopBlackButtonChecked() " << m_isPersonClothingTopBlackButtonChecked;
        return m_isPersonClothingTopBlackButtonChecked;
    }
    bool isPersonClothingTopGrayButtonChecked()
    {
        //qDebug() << "isPersonClothingTopGrayButtonChecked() " << m_isPersonClothingTopGrayButtonChecked;
        return m_isPersonClothingTopGrayButtonChecked;
    }
    bool isPersonClothingTopWhiteButtonChecked()
    {
        //qDebug() << "isPersonClothingTopWhiteButtonChecked() " << m_isPersonClothingTopWhiteButtonChecked;
        return m_isPersonClothingTopWhiteButtonChecked;
    }
    bool isPersonClothingTopRedButtonChecked()
    {
        //qDebug() << "isPersonClothingTopRedButtonChecked() " << m_isPersonClothingTopRedButtonChecked;
        return m_isPersonClothingTopRedButtonChecked;
    }
    bool isPersonClothingTopOrangeButtonChecked()
    {
        //qDebug() << "isPersonClothingTopOrangeButtonChecked() " << m_isPersonClothingTopOrangeButtonChecked;
        return m_isPersonClothingTopOrangeButtonChecked;
    }
    bool isPersonClothingTopYellowButtonChecked()
    {
        //qDebug() << "isPersonClothingTopYellowButtonChecked() " << m_isPersonClothingTopYellowButtonChecked;
        return m_isPersonClothingTopYellowButtonChecked;
    }
    bool isPersonClothingTopGreenButtonChecked()
    {
        //qDebug() << "isPersonClothingTopGreenButtonChecked() " << m_isPersonClothingTopGreenButtonChecked;
        return m_isPersonClothingTopGreenButtonChecked;
    }
    bool isPersonClothingTopBlueButtonChecked()
    {
        //qDebug() << "isPersonClothingTopBlueButtonChecked() " << m_isPersonClothingTopBlueButtonChecked;
        return m_isPersonClothingTopBlueButtonChecked;
    }
    bool isPersonClothingTopPurpleButtonChecked()
    {
        //qDebug() << "isPersonClothingTopPurpleButtonChecked() " << m_isPersonClothingTopPurpleButtonChecked;
        return m_isPersonClothingTopPurpleButtonChecked;
    }
    // Person - ClothingBottom
    bool isPersonClothingBottomBlackButtonChecked()
    {
        return m_isPersonClothingBottomBlackButtonChecked;
    }
    bool isPersonClothingBottomGrayButtonChecked()
    {
        return m_isPersonClothingBottomGrayButtonChecked;
    }
    bool isPersonClothingBottomWhiteButtonChecked()
    {
        return m_isPersonClothingBottomWhiteButtonChecked;
    }
    bool isPersonClothingBottomRedButtonChecked()
    {
        return m_isPersonClothingBottomRedButtonChecked;
    }
    bool isPersonClothingBottomOrangeButtonChecked()
    {
        return m_isPersonClothingBottomOrangeButtonChecked;
    }
    bool isPersonClothingBottomYellowButtonChecked()
    {
        return m_isPersonClothingBottomYellowButtonChecked;
    }
    bool isPersonClothingBottomGreenButtonChecked()
    {
        return m_isPersonClothingBottomGreenButtonChecked;
    }
    bool isPersonClothingBottomBlueButtonChecked()
    {
        return m_isPersonClothingBottomBlueButtonChecked;
    }
    bool isPersonClothingBottomPurpleButtonChecked()
    {
        return m_isPersonClothingBottomPurpleButtonChecked;
    }
    // Person - Bag
    bool isPersonBagWearButtonChecked()
    {
        return m_isPersonBagWearButtonChecked;
    }
    bool isPersonBagNoWearButtonChecked()
    {
        return m_isPersonBagNoWearButtonChecked;
    }
    // Face - Gender
    bool isFaceGenderMaleButtonChecked()
    {
        return m_isFaceGenderMaleButtonChecked;
    }
    bool isFaceGenderFemaleButtonChecked()
    {
        return m_isFaceGenderFemaleButtonChecked;
    }
    // Face - AgeType
    bool isFaceAgeTypeYoungButtonChecked()
    {
        return m_isFaceAgeTypeYoungButtonChecked;
    }
    bool isFaceAgeTypeAdultButtonChecked()
    {
        return m_isFaceAgeTypeAdultButtonChecked;
    }
    bool isFaceAgeTypeMiddleButtonChecked()
    {
        return m_isFaceAgeTypeMiddleButtonChecked;
    }
    bool isFaceAgeTypeSeniorButtonChecked()
    {
        return m_isFaceAgeTypeSeniorButtonChecked;
    }
    // Face - Hat
    bool isFaceHatWearButtonChecked()
    {
        return m_isFaceHatWearButtonChecked;
    }
    bool isFaceHatNoWearButtonChecked()
    {
        return m_isFaceHatNoWearButtonChecked;
    }
    // Face - Opticals
    bool isFaceOpticalsWearButtonChecked()
    {
        return m_isFaceOpticalsWearButtonChecked;
    }
    bool isFaceOpticalsNoWearButtonChecked()
    {
        return m_isFaceOpticalsNoWearButtonChecked;
    }
    // Face - Mask
    bool isFaceMaskWearButtonChecked()
    {
        return m_isFaceMaskWearButtonChecked;
    }
    bool isFaceMaskNoWearButtonChecked()
    {
        return m_isFaceMaskNoWearButtonChecked;
    }
    // Vehicle - Type
    bool isVehicleTypeCarButtonChecked()
    {
        return m_isVehicleTypeCarButtonChecked;
    }
    bool isVehicleTypeBusButtonChecked()
    {
        return m_isVehicleTypeBusButtonChecked;
    }
    bool isVehicleTypeTruckButtonChecked()
    {
        return m_isVehicleTypeTruckButtonChecked;
    }
    bool isVehicleTypeMotorcycleButtonChecked()
    {
        return m_isVehicleTypeMotorcycleButtonChecked;
    }
    bool isVehicleTypeBicycleButtonChecked()
    {
        return m_isVehicleTypeBicycleButtonChecked;
    }
    // Vehicle - Color
    bool isVehicleColorBlackButtonChecked()
    {
        return m_isVehicleColorBlackButtonChecked;
    }
    bool isVehicleColorGrayButtonChecked()
    {
        return m_isVehicleColorGrayButtonChecked;
    }
    bool isVehicleColorWhiteButtonChecked()
    {
        return m_isVehicleColorWhiteButtonChecked;
    }
    bool isVehicleColorRedButtonChecked()
    {
        return m_isVehicleColorRedButtonChecked;
    }
    bool isVehicleColorOrangeButtonChecked()
    {
        return m_isVehicleColorOrangeButtonChecked;
    }
    bool isVehicleColorYellowButtonChecked()
    {
        return m_isVehicleColorYellowButtonChecked;
    }
    bool isVehicleColorGreenButtonChecked()
    {
        return m_isVehicleColorGreenButtonChecked;
    }
    bool isVehicleColorBlueButtonChecked()
    {
        return m_isVehicleColorBlueButtonChecked;
    }
    bool isVehicleColorPurpleButtonChecked()
    {
        return m_isVehicleColorPurpleButtonChecked;
    }
    QString getOcrLicensePlateText()
    {
        return m_ocrLicensePlateText;
    }

    // set visible value
    // Person - Gender
    void setPersonGenderMaleButtonVisible(bool isPersonGenderMaleButtonVisible)
    {
        //qDebug() << "setPersonGenderMaleButtonVisible " << isPersonGenderMaleButtonVisible;
        m_isPersonGenderMaleButtonVisible = isPersonGenderMaleButtonVisible;

        emit personGenderMaleButtonVisibleChanged(isPersonGenderMaleButtonVisible);
    }
    void setPersonGenderFemaleButtonVisible(bool isPersonGenderFemaleButtonVisible)
    {
        //qDebug() << "setPersonGenderFemaleButtonVisible " << isPersonGenderFemaleButtonVisible;
        m_isPersonGenderFemaleButtonVisible = isPersonGenderFemaleButtonVisible;

        emit personGenderFemaleButtonVisibleChanged(isPersonGenderFemaleButtonVisible);
    }
    // Person - ClothingTop
    void setPersonClothingTopBlackButtonVisible(bool isPersonClothingTopBlackButtonVisible)
    {
        //qDebug() << "setPersonClothingTopBlackButtonVisible " << isPersonClothingTopBlackButtonVisible;
        m_isPersonClothingTopBlackButtonVisible = isPersonClothingTopBlackButtonVisible;

        emit personClothingTopBlackButtonVisibleChanged(isPersonClothingTopBlackButtonVisible);
    }
    void setPersonClothingTopGrayButtonVisible(bool isPersonClothingTopGrayButtonVisible)
    {
        //qDebug() << "setPersonClothingTopGrayButtonVisible " << isPersonClothingTopGrayButtonVisible;
        m_isPersonClothingTopGrayButtonVisible = isPersonClothingTopGrayButtonVisible;

        emit personClothingTopGrayButtonVisibleChanged(isPersonClothingTopGrayButtonVisible);
    }
    void setPersonClothingTopWhiteButtonVisible(bool isPersonClothingTopWhiteButtonVisible)
    {
        //qDebug() << "setPersonClothingTopWhiteButtonVisible " << isPersonClothingTopWhiteButtonVisible;
        m_isPersonClothingTopWhiteButtonVisible = isPersonClothingTopWhiteButtonVisible;

        emit personClothingTopWhiteButtonVisibleChanged(isPersonClothingTopWhiteButtonVisible);
    }
    void setPersonClothingTopRedButtonVisible(bool isPersonClothingTopRedButtonVisible)
    {
        //qDebug() << "setPersonClothingTopRedButtonVisible " << isPersonClothingTopRedButtonVisible;
        m_isPersonClothingTopRedButtonVisible = isPersonClothingTopRedButtonVisible;

        emit personClothingTopRedButtonVisibleChanged(isPersonClothingTopRedButtonVisible);
    }
    void setPersonClothingTopOrangeButtonVisible(bool isPersonClothingTopOrangeButtonVisible)
    {
        //qDebug() << "setPersonClothingTopOrangeButtonVisible " << isPersonClothingTopOrangeButtonVisible;
        m_isPersonClothingTopOrangeButtonVisible = isPersonClothingTopOrangeButtonVisible;

        emit personClothingTopOrangeButtonVisibleChanged(isPersonClothingTopOrangeButtonVisible);
    }
    void setPersonClothingTopYellowButtonVisible(bool isPersonClothingTopYellowButtonVisible)
    {
        //qDebug() << "setPersonClothingTopYellowButtonVisible " << isPersonClothingTopYellowButtonVisible;
        m_isPersonClothingTopYellowButtonVisible = isPersonClothingTopYellowButtonVisible;

        emit personClothingTopYellowButtonVisibleChanged(isPersonClothingTopYellowButtonVisible);
    }
    void setPersonClothingTopGreenButtonVisible(bool isPersonClothingTopGreenButtonVisible)
    {
        //qDebug() << "setPersonClothingTopGreenButtonVisible " << isPersonClothingTopGreenButtonVisible;
        m_isPersonClothingTopGreenButtonVisible = isPersonClothingTopGreenButtonVisible;

        emit personClothingTopGreenButtonVisibleChanged(isPersonClothingTopGreenButtonVisible);
    }
    void setPersonClothingTopBlueButtonVisible(bool isPersonClothingTopBlueButtonVisible)
    {
        //qDebug() << "setPersonClothingTopBlueButtonVisible " << isPersonClothingTopBlueButtonVisible;
        m_isPersonClothingTopBlueButtonVisible = isPersonClothingTopBlueButtonVisible;

        emit personClothingTopBlueButtonVisibleChanged(isPersonClothingTopBlueButtonVisible);
    }
    void setPersonClothingTopPurpleButtonVisible(bool isPersonClothingTopPurpleButtonVisible)
    {
        //qDebug() << "setPersonClothingTopPurpleButtonVisible " << isPersonClothingTopPurpleButtonVisible;
        m_isPersonClothingTopPurpleButtonVisible = isPersonClothingTopPurpleButtonVisible;

        emit personClothingTopPurpleButtonVisibleChanged(isPersonClothingTopPurpleButtonVisible);
    }
    // Person - ClothingBottom
    void setPersonClothingBottomBlackButtonVisible(bool isPersonClothingBottomBlackButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomBlackButtonVisible " << isPersonClothingBottomBlackButtonVisible;
        m_isPersonClothingBottomBlackButtonVisible = isPersonClothingBottomBlackButtonVisible;

        emit personClothingBottomBlackButtonVisibleChanged(isPersonClothingBottomBlackButtonVisible);
    }
    void setPersonClothingBottomGrayButtonVisible(bool isPersonClothingBottomGrayButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomGrayButtonVisible " << isPersonClothingBottomGrayButtonVisible;
        m_isPersonClothingBottomGrayButtonVisible = isPersonClothingBottomGrayButtonVisible;

        emit personClothingBottomGrayButtonVisibleChanged(isPersonClothingBottomGrayButtonVisible);
    }
    void setPersonClothingBottomWhiteButtonVisible(bool isPersonClothingBottomWhiteButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomWhiteButtonVisible " << isPersonClothingBottomWhiteButtonVisible;
        m_isPersonClothingBottomWhiteButtonVisible = isPersonClothingBottomWhiteButtonVisible;

        emit personClothingBottomWhiteButtonVisibleChanged(isPersonClothingBottomWhiteButtonVisible);
    }
    void setPersonClothingBottomRedButtonVisible(bool isPersonClothingBottomRedButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomRedButtonVisible " << isPersonClothingBottomRedButtonVisible;
        m_isPersonClothingBottomRedButtonVisible = isPersonClothingBottomRedButtonVisible;

        emit personClothingBottomRedButtonVisibleChanged(isPersonClothingBottomRedButtonVisible);
    }
    void setPersonClothingBottomOrangeButtonVisible(bool isPersonClothingBottomOrangeButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomOrangeButtonVisible " << isPersonClothingBottomOrangeButtonVisible;
        m_isPersonClothingBottomOrangeButtonVisible = isPersonClothingBottomOrangeButtonVisible;

        emit personClothingBottomOrangeButtonVisibleChanged(isPersonClothingBottomOrangeButtonVisible);
    }
    void setPersonClothingBottomYellowButtonVisible(bool isPersonClothingBottomYellowButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomYellowButtonVisible " << isPersonClothingBottomYellowButtonVisible;
        m_isPersonClothingBottomYellowButtonVisible = isPersonClothingBottomYellowButtonVisible;

        emit personClothingBottomYellowButtonVisibleChanged(isPersonClothingBottomYellowButtonVisible);
    }
    void setPersonClothingBottomGreenButtonVisible(bool isPersonClothingBottomGreenButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomGreenButtonVisible " << isPersonClothingBottomGreenButtonVisible;
        m_isPersonClothingBottomGreenButtonVisible = isPersonClothingBottomGreenButtonVisible;

        emit personClothingBottomGreenButtonVisibleChanged(isPersonClothingBottomGreenButtonVisible);
    }
    void setPersonClothingBottomBlueButtonVisible(bool isPersonClothingBottomBlueButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomBlueButtonVisible " << isPersonClothingBottomBlueButtonVisible;
        m_isPersonClothingBottomBlueButtonVisible = isPersonClothingBottomBlueButtonVisible;

        emit personClothingBottomBlueButtonVisibleChanged(isPersonClothingBottomBlueButtonVisible);
    }
    void setPersonClothingBottomPurpleButtonVisible(bool isPersonClothingBottomPurpleButtonVisible)
    {
        //qDebug() << "setPersonClothingBottomPurpleButtonVisible " << isPersonClothingBottomPurpleButtonVisible;
        m_isPersonClothingBottomPurpleButtonVisible = isPersonClothingBottomPurpleButtonVisible;

        emit personClothingBottomPurpleButtonVisibleChanged(isPersonClothingBottomPurpleButtonVisible);
    }
    // Person - Bag
    void setPersonBagWearButtonVisible(bool isPersonBagWearButtonVisible)
    {
        //qDebug() << "setPersonGenderMaleButtonVisible " << isPersonBagWearButtonVisible;
        m_isPersonBagWearButtonVisible = isPersonBagWearButtonVisible;

        emit personBagWearButtonVisibleChanged(isPersonBagWearButtonVisible);
    }
    void setPersonBagNoWearButtonVisible(bool isPersonBagNoWearButtonVisible)
    {
        //qDebug() << "setPersonGenderFemaleButtonVisible " << isPersonBagNoWearButtonVisible;
        m_isPersonBagNoWearButtonVisible = isPersonBagNoWearButtonVisible;

        emit personBagNoWearButtonVisibleChanged(isPersonBagNoWearButtonVisible);
    }
    // Face - Gender
    void setFaceGenderMaleButtonVisible(bool isFaceGenderMaleButtonVisible)
    {
        //qDebug() << "setFaceGenderMaleButtonVisible " << isFaceGenderMaleButtonVisible;
        m_isFaceGenderMaleButtonVisible = isFaceGenderMaleButtonVisible;

        emit faceGenderMaleButtonVisibleChanged(isFaceGenderMaleButtonVisible);
    }
    void setFaceGenderFemaleButtonVisible(bool isFaceGenderFemaleButtonVisible)
    {
        //qDebug() << "setFaceGenderFemaleButtonVisible " << isFaceGenderFemaleButtonVisible;
        m_isFaceGenderFemaleButtonVisible = isFaceGenderFemaleButtonVisible;

        emit faceGenderFemaleButtonVisibleChanged(isFaceGenderFemaleButtonVisible);
    }
    // Face - AgeType
    void setFaceAgeTypeYoungButtonVisible(bool isFaceAgeTypeYoungButtonVisible)
    {
        //qDebug() << "setFaceAgeTypeYoungButtonVisible " << isFaceAgeTypeYoungButtonVisible;
        m_isFaceAgeTypeYoungButtonVisible = isFaceAgeTypeYoungButtonVisible;

        emit faceAgeTypeYoungButtonVisibleChanged(isFaceAgeTypeYoungButtonVisible);
    }
    void setFaceAgeTypeAdultButtonVisible(bool isFaceAgeTypeAdultButtonVisible)
    {
        //qDebug() << "setFaceAgeTypeAdultButtonVisible " << isFaceAgeTypeAdultButtonVisible;
        m_isFaceAgeTypeAdultButtonVisible = isFaceAgeTypeAdultButtonVisible;

        emit faceAgeTypeAdultButtonVisibleChanged(isFaceAgeTypeAdultButtonVisible);
    }
    void setFaceAgeTypeMiddleButtonVisible(bool isFaceAgeTypeMiddleButtonVisible)
    {
        //qDebug() << "setFaceAgeTypeMiddleButtonVisible " << isFaceAgeTypeMiddleButtonVisible;
        m_isFaceAgeTypeMiddleButtonVisible = isFaceAgeTypeMiddleButtonVisible;

        emit faceAgeTypeMiddleButtonVisibleChanged(isFaceAgeTypeMiddleButtonVisible);
    }
    void setFaceAgeTypeSeniorButtonVisible(bool isFaceAgeTypeSeniorButtonVisible)
    {
        //qDebug() << "setFaceAgeTypeSeniorButtonVisible " << isFaceAgeTypeSeniorButtonVisible;
        m_isFaceAgeTypeSeniorButtonVisible = isFaceAgeTypeSeniorButtonVisible;

        emit faceAgeTypeSeniorButtonVisibleChanged(isFaceAgeTypeSeniorButtonVisible);
    }
    // Face - Hat
    void setFaceHatWearButtonVisible(bool isFaceHatWearButtonVisible)
    {
        //qDebug() << "setFaceHatWearButtonVisible " << isFaceHatWearButtonVisible;
        m_isFaceHatWearButtonVisible = isFaceHatWearButtonVisible;

        emit faceHatWearButtonVisibleChanged(isFaceHatWearButtonVisible);
    }
    void setFaceHatNoWearButtonVisible(bool isFaceHatNoWearButtonVisible)
    {
        //qDebug() << "setFaceHatNoWearButtonVisible " << isFaceHatNoWearButtonVisible;
        m_isFaceHatNoWearButtonVisible = isFaceHatNoWearButtonVisible;

        emit faceHatNoWearButtonVisibleChanged(isFaceHatNoWearButtonVisible);
    }
    // Face - Opticals
    void setFaceOpticalsWearButtonVisible(bool isFaceOpticalsWearButtonVisible)
    {
        //qDebug() << "setFaceOpticalsWearButtonVisible " << isFaceOpticalsWearButtonVisible;
        m_isFaceOpticalsWearButtonVisible = isFaceOpticalsWearButtonVisible;

        emit faceOpticalsWearButtonVisibleChanged(isFaceOpticalsWearButtonVisible);
    }
    void setFaceOpticalsNoWearButtonVisible(bool isFaceOpticalsNoWearButtonVisible)
    {
        //qDebug() << "setFaceOpticalsNoWearButtonVisible " << isFaceOpticalsNoWearButtonVisible;
        m_isFaceOpticalsNoWearButtonVisible = isFaceOpticalsNoWearButtonVisible;

        emit faceOpticalsNoWearButtonVisibleChanged(isFaceOpticalsNoWearButtonVisible);
    }
    // Face - Mask
    void setFaceMaskWearButtonVisible(bool isFaceMaskWearButtonVisible)
    {
        //qDebug() << "setFaceMaskWearButtonVisible " << isFaceMaskWearButtonVisible;
        m_isFaceMaskWearButtonVisible = isFaceMaskWearButtonVisible;

        emit faceMaskWearButtonVisibleChanged(isFaceMaskWearButtonVisible);
    }
    void setFaceMaskNoWearButtonVisible(bool isFaceMaskNoWearButtonVisible)
    {
        //qDebug() << "setFaceMaskNoWearButtonVisible " << isFaceMaskNoWearButtonVisible;
        m_isFaceMaskNoWearButtonVisible = isFaceMaskNoWearButtonVisible;

        emit faceMaskNoWearButtonVisibleChanged(isFaceMaskNoWearButtonVisible);
    }
    // Vehicle - Type
    void setVehicleTypeCarButtonVisible(bool isVehicleTypeCarButtonVisible)
    {
        //qDebug() << "setVehicleTypeCarButtonVisible " << isVehicleTypeCarButtonVisible;
        m_isVehicleTypeCarButtonVisible = isVehicleTypeCarButtonVisible;

        emit vehicleTypeCarButtonVisibleChanged(isVehicleTypeCarButtonVisible);
    }
    void setVehicleTypeBusButtonVisible(bool isVehicleTypeBusButtonVisible)
    {
        //qDebug() << "setVehicleTypeBusButtonVisible " << isVehicleTypeBusButtonVisible;
        m_isVehicleTypeBusButtonVisible = isVehicleTypeBusButtonVisible;

        emit vehicleTypeBusButtonVisibleChanged(isVehicleTypeBusButtonVisible);
    }
    void setVehicleTypeTruckButtonVisible(bool isVehicleTypeTruckButtonVisible)
    {
        //qDebug() << "setVehicleTypeTruckButtonVisible " << isVehicleTypeTruckButtonVisible;
        m_isVehicleTypeTruckButtonVisible = isVehicleTypeTruckButtonVisible;

        emit vehicleTypeTruckButtonVisibleChanged(isVehicleTypeTruckButtonVisible);
    }
    void setVehicleTypeMotorcycleButtonVisible(bool isVehicleTypeMotorcycleButtonVisible)
    {
        //qDebug() << "setVehicleTypeMotorcycleButtonVisible " << isVehicleTypeMotorcycleButtonVisible;
        m_isVehicleTypeMotorcycleButtonVisible = isVehicleTypeMotorcycleButtonVisible;

        emit vehicleTypeMotorcycleButtonVisibleChanged(isVehicleTypeMotorcycleButtonVisible);
    }
    void setVehicleTypeBicycleButtonVisible(bool isVehicleTypeBicycleButtonVisible)
    {
        //qDebug() << "setVehicleTypeBicycleButtonVisible " << isVehicleTypeBicycleButtonVisible;
        m_isVehicleTypeBicycleButtonVisible = isVehicleTypeBicycleButtonVisible;

        emit vehicleTypeBicycleButtonVisibleChanged(isVehicleTypeBicycleButtonVisible);
    }
    // Vehicle - Color
    void setVehicleColorBlackButtonVisible(bool isVehicleColorBlackButtonVisible)
    {
        //qDebug() << "setVehicleColorBlackButtonVisible " << isVehicleColorBlackButtonVisible;
        m_isVehicleColorBlackButtonVisible = isVehicleColorBlackButtonVisible;

        emit vehicleColorBlackButtonVisibleChanged(isVehicleColorBlackButtonVisible);
    }
    void setVehicleColorGrayButtonVisible(bool isVehicleColorGrayButtonVisible)
    {
        //qDebug() << "setVehicleColorGrayButtonVisible " << isVehicleColorGrayButtonVisible;
        m_isVehicleColorGrayButtonVisible = isVehicleColorGrayButtonVisible;

        emit vehicleColorGrayButtonVisibleChanged(isVehicleColorGrayButtonVisible);
    }
    void setVehicleColorWhiteButtonVisible(bool isVehicleColorWhiteButtonVisible)
    {
        //qDebug() << "setVehicleColorWhiteButtonVisible " << isVehicleColorWhiteButtonVisible;
        isVehicleColorWhiteButtonVisible = isVehicleColorWhiteButtonVisible;

        emit vehicleColorWhiteButtonVisibleChanged(isVehicleColorWhiteButtonVisible);
    }
    void setVehicleColorRedButtonVisible(bool isVehicleColorRedButtonVisible)
    {
        //qDebug() << "setVehicleColorRedButtonVisible " << isVehicleColorRedButtonVisible;
        m_isVehicleColorRedButtonVisible = isVehicleColorRedButtonVisible;

        emit vehicleColorRedButtonVisibleChanged(isVehicleColorRedButtonVisible);
    }
    void setVehicleColorOrangeButtonVisible(bool isVehicleColorOrangeButtonVisible)
    {
        //qDebug() << "setVehicleColorOrangeButtonVisible " << isVehicleColorOrangeButtonVisible;
        m_isVehicleColorOrangeButtonVisible = isVehicleColorOrangeButtonVisible;

        emit vehicleColorOrangeButtonVisibleChanged(isVehicleColorOrangeButtonVisible);
    }
    void setVehicleColorYellowButtonVisible(bool isVehicleColorYellowButtonVisible)
    {
        //qDebug() << "setVehicleColorYellowButtonVisible " << isVehicleColorYellowButtonVisible;
        m_isVehicleColorYellowButtonVisible = isVehicleColorYellowButtonVisible;

        emit vehicleColorYellowButtonVisibleChanged(isVehicleColorYellowButtonVisible);
    }
    void setVehicleColorGreenButtonVisible(bool isVehicleColorGreenButtonVisible)
    {
        //qDebug() << "setVehicleColorGreenButtonVisible " << isVehicleColorGreenButtonVisible;
        m_isVehicleColorGreenButtonVisible = isVehicleColorGreenButtonVisible;

        emit vehicleColorGreenButtonVisibleChanged(isVehicleColorGreenButtonVisible);
    }
    void setVehicleColorBlueButtonVisible(bool isVehicleColorBlueButtonVisible)
    {
        //qDebug() << "setVehicleColorBlueButtonVisible " << isVehicleColorBlueButtonVisible;
        m_isVehicleColorBlueButtonVisible = isVehicleColorBlueButtonVisible;

        emit vehicleColorBlueButtonVisibleChanged(isVehicleColorBlueButtonVisible);
    }
    void setVehicleColorPurpleButtonVisible(bool isVehicleColorPurpleButtonVisible)
    {
        //qDebug() << "setVehicleColorPurpleButtonVisible " << isVehicleColorPurpleButtonVisible;
        m_isVehicleColorPurpleButtonVisible = isVehicleColorPurpleButtonVisible;

        emit vehicleColorPurpleButtonVisibleChanged(isVehicleColorPurpleButtonVisible);
    }


    // set checked value
    // Person - Gender
    void setPersonGenderMaleButtonChecked(bool isPersonGenderMaleButtonChecked)
    {
        //qDebug() << "setPersonGenderMaleButtonChecked " << isPersonGenderMaleButtonChecked;
        m_isPersonGenderMaleButtonChecked = isPersonGenderMaleButtonChecked;

        emit personGenderMaleButtonCheckedChanged(isPersonGenderMaleButtonChecked);
    }
    void setPersonGenderFemaleButtonChecked(bool isPersonGenderFemaleButtonChecked)
    {
        //qDebug() << "setPersonGenderFemaleButtonChecked " << isPersonGenderFemaleButtonChecked;
        m_isPersonGenderFemaleButtonChecked = isPersonGenderFemaleButtonChecked;

        emit personGenderFemaleButtonCheckedChanged(isPersonGenderFemaleButtonChecked);
    }
    // Person - ClothingTop
    void setPersonClothingTopBlackButtonChecked(bool isPersonClothingTopBlackButtonChecked)
    {
        //qDebug() << "setPersonClothingTopBlackButtonChecked " << isPersonClothingTopBlackButtonChecked;
        m_isPersonClothingTopBlackButtonChecked = isPersonClothingTopBlackButtonChecked;

        emit personClothingTopBlackButtonCheckedChanged(isPersonClothingTopBlackButtonChecked);
    }
    void setPersonClothingTopGrayButtonChecked(bool isPersonClothingTopGrayButtonChecked)
    {
        //qDebug() << "setPersonClothingTopGrayButtonChecked " << isPersonClothingTopGrayButtonChecked;
        m_isPersonClothingTopGrayButtonChecked = isPersonClothingTopGrayButtonChecked;

        emit personClothingTopGrayButtonCheckedChanged(isPersonClothingTopGrayButtonChecked);
    }
    void setPersonClothingTopWhiteButtonChecked(bool isPersonClothingTopWhiteButtonChecked)
    {
        //qDebug() << "setPersonClothingTopWhiteButtonChecked " << isPersonClothingTopWhiteButtonChecked;
        m_isPersonClothingTopWhiteButtonChecked = isPersonClothingTopWhiteButtonChecked;

        emit personClothingTopWhiteButtonCheckedChanged(isPersonClothingTopWhiteButtonChecked);
    }
    void setPersonClothingTopRedButtonChecked(bool isPersonClothingTopRedButtonChecked)
    {
        //qDebug() << "setPersonClothingTopRedButtonChecked " << isPersonClothingTopRedButtonChecked;
        m_isPersonClothingTopRedButtonChecked = isPersonClothingTopRedButtonChecked;

        emit personClothingTopRedButtonCheckedChanged(isPersonClothingTopRedButtonChecked);
    }
    void setPersonClothingTopOrangeButtonChecked(bool isPersonClothingTopOrangeButtonChecked)
    {
        //qDebug() << "setPersonClothingTopOrangeButtonChecked " << isPersonClothingTopOrangeButtonChecked;
        m_isPersonClothingTopOrangeButtonChecked = isPersonClothingTopOrangeButtonChecked;

        emit personClothingTopOrangeButtonCheckedChanged(isPersonClothingTopOrangeButtonChecked);
    }
    void setPersonClothingTopYellowButtonChecked(bool isPersonClothingTopYellowButtonChecked)
    {
        //qDebug() << "setPersonClothingTopYellowButtonChecked " << isPersonClothingTopYellowButtonChecked;
        m_isPersonClothingTopYellowButtonChecked = isPersonClothingTopYellowButtonChecked;

        emit personClothingTopYellowButtonCheckedChanged(isPersonClothingTopYellowButtonChecked);
    }
    void setPersonClothingTopGreenButtonChecked(bool isPersonClothingTopGreenButtonChecked)
    {
        //qDebug() << "setPersonClothingTopGreenButtonChecked " << isPersonClothingTopGreenButtonChecked;
        m_isPersonClothingTopGreenButtonChecked = isPersonClothingTopGreenButtonChecked;

        emit personClothingTopGreenButtonCheckedChanged(isPersonClothingTopGreenButtonChecked);
    }
    void setPersonClothingTopBlueButtonChecked(bool isPersonClothingTopBlueButtonChecked)
    {
        //qDebug() << "setPersonClothingTopBlueButtonChecked " << isPersonClothingTopBlueButtonChecked;
        m_isPersonClothingTopBlueButtonChecked = isPersonClothingTopBlueButtonChecked;

        emit personClothingTopBlueButtonCheckedChanged(isPersonClothingTopBlueButtonChecked);
    }
    void setPersonClothingTopPurpleButtonChecked(bool isPersonClothingTopPurpleButtonChecked)
    {
        //qDebug() << "setPersonClothingTopPurpleButtonChecked " << isPersonClothingTopPurpleButtonChecked;
        m_isPersonClothingTopPurpleButtonChecked = isPersonClothingTopPurpleButtonChecked;

        emit personClothingTopPurpleButtonCheckedChanged(isPersonClothingTopPurpleButtonChecked);
    }
    // Person - ClothingBottom
    void setPersonClothingBottomBlackButtonChecked(bool isPersonClothingBottomBlackButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomBlackButtonChecked " << isPersonClothingBottomBlackButtonChecked;
        m_isPersonClothingBottomBlackButtonChecked = isPersonClothingBottomBlackButtonChecked;

        emit personClothingBottomBlackButtonCheckedChanged(isPersonClothingBottomBlackButtonChecked);
    }
    void setPersonClothingBottomGrayButtonChecked(bool isPersonClothingBottomGrayButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomGrayButtonChecked " << isPersonClothingBottomGrayButtonChecked;
        m_isPersonClothingBottomGrayButtonChecked = isPersonClothingBottomGrayButtonChecked;

        emit personClothingBottomGrayButtonCheckedChanged(isPersonClothingBottomGrayButtonChecked);
    }
    void setPersonClothingBottomWhiteButtonChecked(bool isPersonClothingBottomWhiteButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomWhiteButtonChecked " << isPersonClothingBottomWhiteButtonChecked;
        m_isPersonClothingBottomWhiteButtonChecked = isPersonClothingBottomWhiteButtonChecked;

        emit personClothingBottomWhiteButtonCheckedChanged(isPersonClothingBottomWhiteButtonChecked);
    }
    void setPersonClothingBottomRedButtonChecked(bool isPersonClothingBottomRedButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomRedButtonChecked " << isPersonClothingBottomRedButtonChecked;
        m_isPersonClothingBottomRedButtonChecked = isPersonClothingBottomRedButtonChecked;

        emit personClothingBottomRedButtonCheckedChanged(isPersonClothingBottomRedButtonChecked);
    }
    void setPersonClothingBottomOrangeButtonChecked(bool isPersonClothingBottomOrangeButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomOrangeButtonChecked " << isPersonClothingBottomOrangeButtonChecked;
        m_isPersonClothingBottomOrangeButtonChecked = isPersonClothingBottomOrangeButtonChecked;

        emit personClothingBottomOrangeButtonCheckedChanged(isPersonClothingBottomOrangeButtonChecked);
    }
    void setPersonClothingBottomYellowButtonChecked(bool isPersonClothingBottomYellowButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomYellowButtonChecked " << isPersonClothingBottomYellowButtonChecked;
        m_isPersonClothingBottomYellowButtonChecked = isPersonClothingBottomYellowButtonChecked;

        emit personClothingBottomYellowButtonCheckedChanged(isPersonClothingBottomYellowButtonChecked);
    }
    void setPersonClothingBottomGreenButtonChecked(bool isPersonClothingBottomGreenButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomGreenButtonChecked " << isPersonClothingBottomGreenButtonChecked;
        m_isPersonClothingBottomGreenButtonChecked = isPersonClothingBottomGreenButtonChecked;

        emit personClothingBottomGreenButtonCheckedChanged(isPersonClothingBottomGreenButtonChecked);
    }
    void setPersonClothingBottomBlueButtonChecked(bool isPersonClothingBottomBlueButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomBlueButtonChecked " << isPersonClothingBottomBlueButtonChecked;
        m_isPersonClothingBottomBlueButtonChecked = isPersonClothingBottomBlueButtonChecked;

        emit personClothingBottomBlueButtonCheckedChanged(isPersonClothingBottomBlueButtonChecked);
    }
    void setPersonClothingBottomPurpleButtonChecked(bool isPersonClothingBottomPurpleButtonChecked)
    {
        //qDebug() << "setPersonClothingBottomPurpleButtonChecked " << isPersonClothingBottomPurpleButtonChecked;
        m_isPersonClothingBottomPurpleButtonChecked = isPersonClothingBottomPurpleButtonChecked;

        emit personClothingBottomPurpleButtonCheckedChanged(isPersonClothingBottomPurpleButtonChecked);
    }
    // Person - Bag
    void setPersonBagWearButtonChecked(bool isPersonBagWearButtonChecked)
    {
        //qDebug() << "setPersonGenderMaleButtonChecked " << isPersonBagWearButtonChecked;
        m_isPersonBagWearButtonChecked = isPersonBagWearButtonChecked;

        emit personBagWearButtonCheckedChanged(isPersonBagWearButtonChecked);
    }
    void setPersonBagNoWearButtonChecked(bool isPersonBagNoWearButtonChecked)
    {
        //qDebug() << "setPersonGenderFemaleButtonChecked " << isPersonBagNoWearButtonChecked;
        m_isPersonBagNoWearButtonChecked = isPersonBagNoWearButtonChecked;

        emit personBagNoWearButtonCheckedChanged(isPersonBagNoWearButtonChecked);
    }
    // Face - Gender
    void setFaceGenderMaleButtonChecked(bool isFaceGenderMaleButtonChecked)
    {
        //qDebug() << "setFaceGenderMaleButtonChecked " << isFaceGenderMaleButtonChecked;
        m_isFaceGenderMaleButtonChecked = isFaceGenderMaleButtonChecked;

        emit faceGenderMaleButtonCheckedChanged(isFaceGenderMaleButtonChecked);
    }
    void setFaceGenderFemaleButtonChecked(bool isFaceGenderFemaleButtonChecked)
    {
        //qDebug() << "setFaceGenderFemaleButtonChecked " << isFaceGenderFemaleButtonChecked;
        m_isFaceGenderFemaleButtonChecked = isFaceGenderFemaleButtonChecked;

        emit faceGenderFemaleButtonCheckedChanged(isFaceGenderFemaleButtonChecked);
    }
    // Face - AgeType
    void setFaceAgeTypeYoungButtonChecked(bool isFaceAgeTypeYoungButtonChecked)
    {
        //qDebug() << "setFaceAgeTypeYoungButtonChecked " << isFaceAgeTypeYoungButtonChecked;
        m_isFaceAgeTypeYoungButtonChecked = isFaceAgeTypeYoungButtonChecked;

        emit faceAgeTypeYoungButtonCheckedChanged(isFaceAgeTypeYoungButtonChecked);
    }
    void setFaceAgeTypeAdultButtonChecked(bool isFaceAgeTypeAdultButtonChecked)
    {
        //qDebug() << "setFaceAgeTypeAdultButtonChecked " << isFaceAgeTypeAdultButtonChecked;
        m_isFaceAgeTypeAdultButtonChecked = isFaceAgeTypeAdultButtonChecked;

        emit faceAgeTypeAdultButtonCheckedChanged(isFaceAgeTypeAdultButtonChecked);
    }
    void setFaceAgeTypeMiddleButtonChecked(bool isFaceAgeTypeMiddleButtonChecked)
    {
        //qDebug() << "setFaceAgeTypeMiddleButtonChecked " << isFaceAgeTypeMiddleButtonChecked;
        m_isFaceAgeTypeMiddleButtonChecked = isFaceAgeTypeMiddleButtonChecked;

        emit faceAgeTypeMiddleButtonCheckedChanged(isFaceAgeTypeMiddleButtonChecked);
    }
    void setFaceAgeTypeSeniorButtonChecked(bool isFaceAgeTypeSeniorButtonChecked)
    {
        //qDebug() << "setFaceAgeTypeSeniorButtonChecked " << isFaceAgeTypeSeniorButtonChecked;
        m_isFaceAgeTypeSeniorButtonChecked = isFaceAgeTypeSeniorButtonChecked;

        emit faceAgeTypeSeniorButtonCheckedChanged(isFaceAgeTypeSeniorButtonChecked);
    }
    // Face - Hat
    void setFaceHatWearButtonChecked(bool isFaceHatWearButtonChecked)
    {
        //qDebug() << "setFaceHatWearButtonChecked " << isFaceHatWearButtonChecked;
        m_isFaceHatWearButtonChecked = isFaceHatWearButtonChecked;

        emit faceHatWearButtonCheckedChanged(isFaceHatWearButtonChecked);
    }
    void setFaceHatNoWearButtonChecked(bool isFaceHatNoWearButtonChecked)
    {
        //qDebug() << "setFaceHatNoWearButtonChecked " << isFaceHatNoWearButtonChecked;
        m_isFaceHatNoWearButtonChecked = isFaceHatNoWearButtonChecked;

        emit faceHatNoWearButtonCheckedChanged(isFaceHatNoWearButtonChecked);
    }
    // Face - Opticals
    void setFaceOpticalsWearButtonChecked(bool isFaceOpticalsWearButtonChecked)
    {
        //qDebug() << "setFaceOpticalsWearButtonChecked " << isFaceOpticalsWearButtonChecked;
        m_isFaceOpticalsWearButtonChecked = isFaceOpticalsWearButtonChecked;

        emit faceOpticalsWearButtonCheckedChanged(isFaceOpticalsWearButtonChecked);
    }
    void setFaceOpticalsNoWearButtonChecked(bool isFaceOpticalsNoWearButtonChecked)
    {
        //qDebug() << "setFaceOpticalsNoWearButtonChecked " << isFaceOpticalsNoWearButtonChecked;
        m_isFaceOpticalsNoWearButtonChecked = isFaceOpticalsNoWearButtonChecked;

        emit faceOpticalsNoWearButtonCheckedChanged(isFaceOpticalsNoWearButtonChecked);
    }
    // Face - Mask
    void setFaceMaskWearButtonChecked(bool isFaceMaskWearButtonChecked)
    {
        //qDebug() << "setFaceMaskWearButtonChecked " << isFaceMaskWearButtonChecked;
        m_isFaceMaskWearButtonChecked = isFaceMaskWearButtonChecked;

        emit faceMaskWearButtonCheckedChanged(isFaceMaskWearButtonChecked);
    }
    void setFaceMaskNoWearButtonChecked(bool isFaceMaskNoWearButtonChecked)
    {
        //qDebug() << "setFaceMaskNoWearButtonChecked " << isFaceMaskNoWearButtonChecked;
        m_isFaceMaskNoWearButtonChecked = isFaceMaskNoWearButtonChecked;

        emit faceMaskNoWearButtonCheckedChanged(isFaceMaskNoWearButtonChecked);
    }
    // Vehicle - Type
    void setVehicleTypeCarButtonChecked(bool isVehicleTypeCarButtonChecked)
    {
        //qDebug() << "setVehicleTypeCarButtonChecked " << isVehicleTypeCarButtonChecked;
        m_isVehicleTypeCarButtonChecked = isVehicleTypeCarButtonChecked;

        emit vehicleTypeCarButtonCheckedChanged(isVehicleTypeCarButtonChecked);
    }
    void setVehicleTypeBusButtonChecked(bool isVehicleTypeBusButtonChecked)
    {
        //qDebug() << "setVehicleTypeBusButtonChecked " << isVehicleTypeBusButtonChecked;
        m_isVehicleTypeBusButtonChecked = isVehicleTypeBusButtonChecked;

        emit vehicleTypeBusButtonCheckedChanged(isVehicleTypeBusButtonChecked);
    }
    void setVehicleTypeTruckButtonChecked(bool isVehicleTypeTruckButtonChecked)
    {
        //qDebug() << "setVehicleTypeTruckButtonChecked " << isVehicleTypeTruckButtonChecked;
        m_isVehicleTypeTruckButtonChecked = isVehicleTypeTruckButtonChecked;

        emit vehicleTypeTruckButtonCheckedChanged(isVehicleTypeTruckButtonChecked);
    }
    void setVehicleTypeMotorcycleButtonChecked(bool isVehicleTypeMotorcycleButtonChecked)
    {
        //qDebug() << "setVehicleTypeMotorcycleButtonChecked " << isVehicleTypeMotorcycleButtonChecked;
        m_isVehicleTypeMotorcycleButtonChecked = isVehicleTypeMotorcycleButtonChecked;

        emit vehicleTypeMotorcycleButtonCheckedChanged(isVehicleTypeMotorcycleButtonChecked);
    }
    void setVehicleTypeBicycleButtonChecked(bool isVehicleTypeBicycleButtonChecked)
    {
        //qDebug() << "setVehicleTypeBicycleButtonChecked " << isVehicleTypeBicycleButtonChecked;
        m_isVehicleTypeBicycleButtonChecked = isVehicleTypeBicycleButtonChecked;

        emit vehicleTypeBicycleButtonCheckedChanged(isVehicleTypeBicycleButtonChecked);
    }
    // Vehicle - Color
    void setVehicleColorBlackButtonChecked(bool isVehicleColorBlackButtonChecked)
    {
        //qDebug() << "setVehicleColorBlackButtonChecked " << isVehicleColorBlackButtonChecked;
        m_isVehicleColorBlackButtonChecked = isVehicleColorBlackButtonChecked;

        emit vehicleColorBlackButtonCheckedChanged(isVehicleColorBlackButtonChecked);
    }
    void setVehicleColorGrayButtonChecked(bool isVehicleColorGrayButtonChecked)
    {
        //qDebug() << "setVehicleColorGrayButtonChecked " << isVehicleColorGrayButtonChecked;
        m_isVehicleColorGrayButtonChecked = isVehicleColorGrayButtonChecked;

        emit vehicleColorGrayButtonCheckedChanged(isVehicleColorGrayButtonChecked);
    }
    void setVehicleColorWhiteButtonChecked(bool isVehicleColorWhiteButtonChecked)
    {
//        qDebug() << "setVehicleColorWhiteButtonChecked " << isVehicleColorWhiteButtonChecked;
        m_isVehicleColorWhiteButtonChecked = isVehicleColorWhiteButtonChecked;

        emit vehicleColorWhiteButtonCheckedChanged(isVehicleColorWhiteButtonChecked);
    }
    void setVehicleColorRedButtonChecked(bool isVehicleColorRedButtonChecked)
    {
        //qDebug() << "setVehicleColorRedButtonChecked " << isVehicleColorRedButtonChecked;
        m_isVehicleColorRedButtonChecked = isVehicleColorRedButtonChecked;

        emit vehicleColorRedButtonCheckedChanged(isVehicleColorRedButtonChecked);
    }
    void setVehicleColorOrangeButtonChecked(bool isVehicleColorOrangeButtonChecked)
    {
        //qDebug() << "setVehicleColorOrangeButtonChecked " << isVehicleColorOrangeButtonChecked;
        m_isVehicleColorOrangeButtonChecked = isVehicleColorOrangeButtonChecked;

        emit vehicleColorOrangeButtonCheckedChanged(isVehicleColorOrangeButtonChecked);
    }
    void setVehicleColorYellowButtonChecked(bool isVehicleColorYellowButtonChecked)
    {
        //qDebug() << "setVehicleColorYellowButtonChecked " << isVehicleColorYellowButtonChecked;
        m_isVehicleColorYellowButtonChecked = isVehicleColorYellowButtonChecked;

        emit vehicleColorYellowButtonCheckedChanged(isVehicleColorYellowButtonChecked);
    }
    void setVehicleColorGreenButtonChecked(bool isVehicleColorGreenButtonChecked)
    {
        //qDebug() << "setVehicleColorGreenButtonChecked " << isVehicleColorGreenButtonChecked;
        m_isVehicleColorGreenButtonChecked = isVehicleColorGreenButtonChecked;

        emit vehicleColorGreenButtonCheckedChanged(isVehicleColorGreenButtonChecked);
    }
    void setVehicleColorBlueButtonChecked(bool isVehicleColorBlueButtonChecked)
    {
        //qDebug() << "setVehicleColorBlueButtonChecked " << isVehicleColorBlueButtonChecked;
        m_isVehicleColorBlueButtonChecked = isVehicleColorBlueButtonChecked;

        emit vehicleColorBlueButtonCheckedChanged(isVehicleColorBlueButtonChecked);
    }
    void setVehicleColorPurpleButtonChecked(bool isVehicleColorPurpleButtonChecked)
    {
        //qDebug() << "setVehicleColorPurpleButtonChecked " << isVehicleColorPurpleButtonChecked;
        m_isVehicleColorPurpleButtonChecked = isVehicleColorPurpleButtonChecked;

        emit vehicleColorPurpleButtonCheckedChanged(isVehicleColorPurpleButtonChecked);
    }
    void setOcrLicensePlateText(QString ocrLicensePlateText)
    {
        //qDebug() << "setOcrLicensePlateText " << ocrLicensePlateText;
        m_ocrLicensePlateText = ocrLicensePlateText;

        emit ocrLicensePlateTextChanged(ocrLicensePlateText);
    }

signals:
    // notify visible changed
    // Person - Gender
    void personGenderMaleButtonVisibleChanged(bool isPersonGenderMaleButtonVisible);
    void personGenderFemaleButtonVisibleChanged(bool isPersonGenderFemaleButtonVisible);
    // Person - ClothingTop
    void personClothingTopBlackButtonVisibleChanged(bool isPersonClothingTopBlackButtonVisible);
    void personClothingTopGrayButtonVisibleChanged(bool isPersonClothingTopGrayButtonVisible);
    void personClothingTopWhiteButtonVisibleChanged(bool isPersonClothingTopWhiteButtonVisible);
    void personClothingTopRedButtonVisibleChanged(bool isPersonClothingTopRedButtonVisible);
    void personClothingTopOrangeButtonVisibleChanged(bool isPersonClothingTopOrangeButtonVisible);
    void personClothingTopYellowButtonVisibleChanged(bool isPersonClothingTopYellowButtonVisible);
    void personClothingTopGreenButtonVisibleChanged(bool isPersonClothingTopGreenButtonVisible);
    void personClothingTopBlueButtonVisibleChanged(bool isPersonClothingTopBlueButtonVisible);
    void personClothingTopPurpleButtonVisibleChanged(bool isPersonClothingTopPurpleButtonVisible);
    // Person - ClothingBottom
    void personClothingBottomBlackButtonVisibleChanged(bool isPersonClothingBottomBlackButtonVisible);
    void personClothingBottomGrayButtonVisibleChanged(bool isPersonClothingBottomGrayButtonVisible);
    void personClothingBottomWhiteButtonVisibleChanged(bool isPersonClothingBottomWhiteButtonVisible);
    void personClothingBottomRedButtonVisibleChanged(bool isPersonClothingBottomRedButtonVisible);
    void personClothingBottomOrangeButtonVisibleChanged(bool isPersonClothingBottomOrangeButtonVisible);
    void personClothingBottomYellowButtonVisibleChanged(bool isPersonClothingBottomYellowButtonVisible);
    void personClothingBottomGreenButtonVisibleChanged(bool isPersonClothingBottomGreenButtonVisible);
    void personClothingBottomBlueButtonVisibleChanged(bool isPersonClothingBottomBlueButtonVisible);
    void personClothingBottomPurpleButtonVisibleChanged(bool isPersonClothingBottomPurpleButtonVisible);
    // Person - Bag
    void personBagWearButtonVisibleChanged(bool isPersonBagWearButtonVisible);
    void personBagNoWearButtonVisibleChanged(bool isPersonBagNoWearButtonVisible);
    // Face - Gender
    void faceGenderMaleButtonVisibleChanged(bool isFaceGenderMaleButtonVisible);
    void faceGenderFemaleButtonVisibleChanged(bool isFaceGenderFemaleButtonVisible);
    // Face - AgeType
    void faceAgeTypeYoungButtonVisibleChanged(bool isFaceAgeTypeYoungButtonVisible);
    void faceAgeTypeAdultButtonVisibleChanged(bool isFaceAgeTypeAdultButtonVisible);
    void faceAgeTypeMiddleButtonVisibleChanged(bool isFaceAgeTypeMiddleButtonVisible);
    void faceAgeTypeSeniorButtonVisibleChanged(bool isFaceAgeTypeSeniorButtonVisible);
    // Face - Hat
    void faceHatWearButtonVisibleChanged(bool isFaceHatWearButtonVisible);
    void faceHatNoWearButtonVisibleChanged(bool isFaceHatNoWearButtonVisible);
    // Face - Opticals
    void faceOpticalsWearButtonVisibleChanged(bool isFaceOpticalsWearButtonVisible);
    void faceOpticalsNoWearButtonVisibleChanged(bool isFaceOpticalsNoWearButtonVisible);
    // Face - Mask
    void faceMaskWearButtonVisibleChanged(bool isFaceMaskWearButtonVisible);
    void faceMaskNoWearButtonVisibleChanged(bool isFaceMaskNoWearButtonVisible);
    // Vehicle - Type
    void vehicleTypeCarButtonVisibleChanged(bool isVehicleTypeCarButtonVisible);
    void vehicleTypeBusButtonVisibleChanged(bool isVehicleTypeBusButtonVisible);
    void vehicleTypeTruckButtonVisibleChanged(bool isVehicleTypeTruckButtonVisible);
    void vehicleTypeMotorcycleButtonVisibleChanged(bool isVehicleTypeMotorcycleButtonVisible);
    void vehicleTypeBicycleButtonVisibleChanged(bool isVehicleTypeBicycleButtonVisible);
    // Vehicle - Color
    void vehicleColorBlackButtonVisibleChanged(bool isVehicleColorBlackButtonVisible);
    void vehicleColorGrayButtonVisibleChanged(bool isVehicleColorGrayButtonVisible);
    void vehicleColorWhiteButtonVisibleChanged(bool isVehicleColorWhiteButtonVisible);
    void vehicleColorRedButtonVisibleChanged(bool isVehicleColorRedButtonVisible);
    void vehicleColorOrangeButtonVisibleChanged(bool isVehicleColorOrangeButtonVisible);
    void vehicleColorYellowButtonVisibleChanged(bool isVehicleColorYellowButtonVisible);
    void vehicleColorGreenButtonVisibleChanged(bool isVehicleColorGreenButtonVisible);
    void vehicleColorBlueButtonVisibleChanged(bool isVehicleColorBlueButtonVisible);
    void vehicleColorPurpleButtonVisibleChanged(bool isVehicleColorPurpleButtonVisible);

    // notify checked changed
    // Person - Gender
    void personGenderMaleButtonCheckedChanged(bool isPersonGenderMaleButtonChecked);
    void personGenderFemaleButtonCheckedChanged(bool isPersonGenderFemaleButtonChecked);
    // Person - ClothingTop
    void personClothingTopBlackButtonCheckedChanged(bool isPersonClothingTopBlackButtonChecked);
    void personClothingTopGrayButtonCheckedChanged(bool isPersonClothingTopGrayButtonChecked);
    void personClothingTopWhiteButtonCheckedChanged(bool isPersonClothingTopWhiteButtonChecked);
    void personClothingTopRedButtonCheckedChanged(bool isPersonClothingTopRedButtonChecked);
    void personClothingTopOrangeButtonCheckedChanged(bool isPersonClothingTopOrangeButtonChecked);
    void personClothingTopYellowButtonCheckedChanged(bool isPersonClothingTopYellowButtonChecked);
    void personClothingTopGreenButtonCheckedChanged(bool isPersonClothingTopGreenButtonChecked);
    void personClothingTopBlueButtonCheckedChanged(bool isPersonClothingTopBlueButtonChecked);
    void personClothingTopPurpleButtonCheckedChanged(bool isPersonClothingTopPurpleButtonChecked);
    // Person - ClothingBottom
    void personClothingBottomBlackButtonCheckedChanged(bool isPersonClothingBottomBlackButtonChecked);
    void personClothingBottomGrayButtonCheckedChanged(bool isPersonClothingBottomGrayButtonChecked);
    void personClothingBottomWhiteButtonCheckedChanged(bool isPersonClothingBottomWhiteButtonChecked);
    void personClothingBottomRedButtonCheckedChanged(bool isPersonClothingBottomRedButtonChecked);
    void personClothingBottomOrangeButtonCheckedChanged(bool isPersonClothingBottomOrangeButtonChecked);
    void personClothingBottomYellowButtonCheckedChanged(bool isPersonClothingBottomYellowButtonChecked);
    void personClothingBottomGreenButtonCheckedChanged(bool isPersonClothingBottomGreenButtonChecked);
    void personClothingBottomBlueButtonCheckedChanged(bool isPersonClothingBottomBlueButtonChecked);
    void personClothingBottomPurpleButtonCheckedChanged(bool isPersonClothingBottomPurpleButtonChecked);
    // Person - Bag
    void personBagWearButtonCheckedChanged(bool isPersonBagWearButtonChecked);
    void personBagNoWearButtonCheckedChanged(bool isPersonBagNoWearButtonChecked);
    // Face - Gender
    void faceGenderMaleButtonCheckedChanged(bool isFaceGenderMaleButtonChecked);
    void faceGenderFemaleButtonCheckedChanged(bool isFaceGenderFemaleButtonChecked);
    // Face - AgeType
    void faceAgeTypeYoungButtonCheckedChanged(bool isFaceAgeTypeYoungButtonChecked);
    void faceAgeTypeAdultButtonCheckedChanged(bool isFaceAgeTypeAdultButtonChecked);
    void faceAgeTypeMiddleButtonCheckedChanged(bool isFaceAgeTypeMiddleButtonChecked);
    void faceAgeTypeSeniorButtonCheckedChanged(bool isFaceAgeTypeSeniorButtonChecked);
    // Face - Hat
    void faceHatWearButtonCheckedChanged(bool isFaceHatWearButtonChecked);
    void faceHatNoWearButtonCheckedChanged(bool isFaceHatNoWearButtonChecked);
    // Face - Opticals
    void faceOpticalsWearButtonCheckedChanged(bool isFaceOpticalsWearButtonChecked);
    void faceOpticalsNoWearButtonCheckedChanged(bool isFaceOpticalsNoWearButtonChecked);
    // Face - Mask
    void faceMaskWearButtonCheckedChanged(bool isFaceMaskWearButtonChecked);
    void faceMaskNoWearButtonCheckedChanged(bool isFaceMaskNoWearButtonChecked);
    // Vehicle - Type
    void vehicleTypeCarButtonCheckedChanged(bool isVehicleTypeCarButtonChecked);
    void vehicleTypeBusButtonCheckedChanged(bool isVehicleTypeBusButtonChecked);
    void vehicleTypeTruckButtonCheckedChanged(bool isVehicleTypeTruckButtonChecked);
    void vehicleTypeMotorcycleButtonCheckedChanged(bool isVehicleTypeMotorcycleButtonChecked);
    void vehicleTypeBicycleButtonCheckedChanged(bool isVehicleTypeBicycleButtonChecked);
    // Vehicle - Color
    void vehicleColorBlackButtonCheckedChanged(bool isVehicleColorBlackButtonChecked);
    void vehicleColorGrayButtonCheckedChanged(bool isVehicleColorGrayButtonChecked);
    void vehicleColorWhiteButtonCheckedChanged(bool isVehicleColorWhiteButtonChecked);
    void vehicleColorRedButtonCheckedChanged(bool isVehicleColorRedButtonChecked);
    void vehicleColorOrangeButtonCheckedChanged(bool isVehicleColorOrangeButtonChecked);
    void vehicleColorYellowButtonCheckedChanged(bool isVehicleColorYellowButtonChecked);
    void vehicleColorGreenButtonCheckedChanged(bool isVehicleColorGreenButtonChecked);
    void vehicleColorBlueButtonCheckedChanged(bool isVehicleColorBlueButtonChecked);
    void vehicleColorPurpleButtonCheckedChanged(bool isVehicleColorPurpleButtonChecked);
    // OCR - LP
    void ocrLicensePlateTextChanged(QString ocrLicensePlateText);

protected:
    // button visible variable
    // Person - Gender
    bool m_isPersonGenderMaleButtonVisible = false;
    bool m_isPersonGenderFemaleButtonVisible = false;
    // Person - ClothingTop
    bool m_isPersonClothingTopBlackButtonVisible = false;
    bool m_isPersonClothingTopGrayButtonVisible = false;
    bool m_isPersonClothingTopWhiteButtonVisible = false;
    bool m_isPersonClothingTopRedButtonVisible = false;
    bool m_isPersonClothingTopOrangeButtonVisible = false;
    bool m_isPersonClothingTopYellowButtonVisible = false;
    bool m_isPersonClothingTopGreenButtonVisible = false;
    bool m_isPersonClothingTopBlueButtonVisible = false;
    bool m_isPersonClothingTopPurpleButtonVisible = false;
    // Person - ClothingBottom
    bool m_isPersonClothingBottomBlackButtonVisible = false;
    bool m_isPersonClothingBottomGrayButtonVisible = false;
    bool m_isPersonClothingBottomWhiteButtonVisible = false;
    bool m_isPersonClothingBottomRedButtonVisible = false;
    bool m_isPersonClothingBottomOrangeButtonVisible = false;
    bool m_isPersonClothingBottomYellowButtonVisible = false;
    bool m_isPersonClothingBottomGreenButtonVisible = false;
    bool m_isPersonClothingBottomBlueButtonVisible = false;
    bool m_isPersonClothingBottomPurpleButtonVisible = false;
    // Person - Bag
    bool m_isPersonBagWearButtonVisible = false;
    bool m_isPersonBagNoWearButtonVisible = false;
    // Face - Gender
    bool m_isFaceGenderMaleButtonVisible = false;
    bool m_isFaceGenderFemaleButtonVisible = false;
    // Face - AgeType
    bool m_isFaceAgeTypeYoungButtonVisible = false;
    bool m_isFaceAgeTypeAdultButtonVisible = false;
    bool m_isFaceAgeTypeMiddleButtonVisible = false;
    bool m_isFaceAgeTypeSeniorButtonVisible = false;
    // Face - Hat
    bool m_isFaceHatWearButtonVisible = false;
    bool m_isFaceHatNoWearButtonVisible = false;
    // Face - Opticals
    bool m_isFaceOpticalsWearButtonVisible = false;
    bool m_isFaceOpticalsNoWearButtonVisible = false;
    // Face - Mask
    bool m_isFaceMaskWearButtonVisible = false;
    bool m_isFaceMaskNoWearButtonVisible = false;
    // Vehicle - Type
    bool m_isVehicleTypeCarButtonVisible = false;
    bool m_isVehicleTypeBusButtonVisible = false;
    bool m_isVehicleTypeTruckButtonVisible = false;
    bool m_isVehicleTypeMotorcycleButtonVisible = false;
    bool m_isVehicleTypeBicycleButtonVisible = false;
    // Vehicle - Color
    bool m_isVehicleColorBlackButtonVisible = false;
    bool m_isVehicleColorGrayButtonVisible = false;
    bool m_isVehicleColorWhiteButtonVisible = false;
    bool m_isVehicleColorRedButtonVisible = false;
    bool m_isVehicleColorOrangeButtonVisible = false;
    bool m_isVehicleColorYellowButtonVisible = false;
    bool m_isVehicleColorGreenButtonVisible = false;
    bool m_isVehicleColorBlueButtonVisible = false;
    bool m_isVehicleColorPurpleButtonVisible = false;

    // button checked variable
    // Person - Gender
    bool m_isPersonGenderMaleButtonChecked = false;
    bool m_isPersonGenderFemaleButtonChecked = false;
    // Person - ClothingTop
    bool m_isPersonClothingTopBlackButtonChecked = false;
    bool m_isPersonClothingTopGrayButtonChecked = false;
    bool m_isPersonClothingTopWhiteButtonChecked = false;
    bool m_isPersonClothingTopRedButtonChecked = false;
    bool m_isPersonClothingTopOrangeButtonChecked = false;
    bool m_isPersonClothingTopYellowButtonChecked = false;
    bool m_isPersonClothingTopGreenButtonChecked = false;
    bool m_isPersonClothingTopBlueButtonChecked = false;
    bool m_isPersonClothingTopPurpleButtonChecked = false;
    // Person - ClothingBottom
    bool m_isPersonClothingBottomBlackButtonChecked = false;
    bool m_isPersonClothingBottomGrayButtonChecked = false;
    bool m_isPersonClothingBottomWhiteButtonChecked = false;
    bool m_isPersonClothingBottomRedButtonChecked = false;
    bool m_isPersonClothingBottomOrangeButtonChecked = false;
    bool m_isPersonClothingBottomYellowButtonChecked = false;
    bool m_isPersonClothingBottomGreenButtonChecked = false;
    bool m_isPersonClothingBottomBlueButtonChecked = false;
    bool m_isPersonClothingBottomPurpleButtonChecked = false;
    // Person - Bag
    bool m_isPersonBagWearButtonChecked = false;
    bool m_isPersonBagNoWearButtonChecked = false;
    // Face - Gender
    bool m_isFaceGenderMaleButtonChecked = false;
    bool m_isFaceGenderFemaleButtonChecked = false;
    // Face - AgeType
    bool m_isFaceAgeTypeYoungButtonChecked = false;
    bool m_isFaceAgeTypeAdultButtonChecked = false;
    bool m_isFaceAgeTypeMiddleButtonChecked = false;
    bool m_isFaceAgeTypeSeniorButtonChecked = false;
    // Face - Hat
    bool m_isFaceHatWearButtonChecked = false;
    bool m_isFaceHatNoWearButtonChecked = false;
    // Face - Opticals
    bool m_isFaceOpticalsWearButtonChecked = false;
    bool m_isFaceOpticalsNoWearButtonChecked = false;
    // Face - Mask
    bool m_isFaceMaskWearButtonChecked = false;
    bool m_isFaceMaskNoWearButtonChecked = false;
    // Vehicle - Type
    bool m_isVehicleTypeCarButtonChecked = false;
    bool m_isVehicleTypeBusButtonChecked = false;
    bool m_isVehicleTypeTruckButtonChecked = false;
    bool m_isVehicleTypeMotorcycleButtonChecked = false;
    bool m_isVehicleTypeBicycleButtonChecked = false;
    // Vehicle - Color
    bool m_isVehicleColorBlackButtonChecked = false;
    bool m_isVehicleColorGrayButtonChecked = false;
    bool m_isVehicleColorWhiteButtonChecked = false;
    bool m_isVehicleColorRedButtonChecked = false;
    bool m_isVehicleColorOrangeButtonChecked = false;
    bool m_isVehicleColorYellowButtonChecked = false;
    bool m_isVehicleColorGreenButtonChecked = false;
    bool m_isVehicleColorBlueButtonChecked = false;
    bool m_isVehicleColorPurpleButtonChecked = false;
    // OCR - LP
    QString m_ocrLicensePlateText = "";
};
