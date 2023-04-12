#pragma once
#include <QSharedPointer>
#include <QObject>
#include <QQmlEngine>

namespace WisenetViewerDefine{

const int WISENET_VIEWER_MAJOR_VERSION = 1;
const int WISENET_VIEWER_MINER_VERSION = 0;

enum class RequestType{
    ShowPasswordSettingPage,
    ShowLoginPage,
    ShowMonitoringPage,

    Logout,
};

struct ViewerRequest{
    RequestType requestType;
};

typedef QSharedPointer<ViewerRequest> ViewerRequestSharedPtr;

class WisenetItemType : public QObject
{
    Q_OBJECT
public:
    explicit WisenetItemType(QObject *parent = nullptr) : QObject(parent){}

    enum ItemType
    {
        MainRoot = 0,

        ResourceRoot,

        Group,
        Device,
        Camera,
        Channel,

        LayoutRoot,
        Layout,

        SequenceRoot,
        Sequence,
        SequenceItem,

        WebPageRoot,
        WebPage,

        UserRoot,
        Role,
        User,

        LocalFileRoot,
        LocalDirectory,
        LocalFile,

        Event,
        EventGroup,
        AlarmOutput,
        AlarmIn,
        EventEmailAllUser,    // 2022.06.21. Added
        EventEmailGroup,      // 2022.06.21. Added
        EventEmail,           // 2022.06.21. Added
    };
    Q_ENUM(ItemType)

    static void Register() {
        qmlRegisterType<WisenetItemType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "ItemType");
    }
};
typedef WisenetItemType::ItemType ItemType;

class WisenetItemStatus : public QObject
{
    Q_OBJECT
public:
    explicit WisenetItemStatus(QObject *parent = nullptr) : QObject(parent){}

    enum ItemStatus
    {
        Loading = 0,
        Normal,
        Abnormal,
        Unused,
        Authenticated,
        Manipulated,
        NotSigned,
        Verifying
    };
    Q_ENUM(ItemStatus)

    static void Register() {
        qmlRegisterType<WisenetItemStatus>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "ItemStatus");
    }
};
typedef WisenetItemStatus::ItemStatus ItemStatus;

class WisenetLanguage: public QObject{
    Q_OBJECT

public:
    enum Language{
        Korean = 0,
        English_USA,
        English_UK,
        Chinese,
        Croatian,
        Czech,
        Danish,
        Dutch,
        Finnish,
        French,
        German,
        Greek,
        Hungarian,
        Italian,
        Japanese,
        Norwegian,
        Polish,
        Portuguese,
        Romanian,
        Russian,
        Serbian,
        Spanish,
        Taiwanese,
        Turkish
    };
    Q_ENUM(Language)

    static void Register() {
        qmlRegisterType<WisenetLanguage>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "Language");
    }
    static enum Language Convert(QString lang){
        if(lang == "Korean") return Language::Korean;
        if(lang == "English-USA") return Language::English_USA;
        if(lang == "English-UK") return Language::English_UK;
        if(lang == "Chinese") return Language::Chinese;
        if(lang == "Croatian") return Language::Croatian;
        if(lang == "Czech") return Language::Czech;
        if(lang == "Danish") return Language::Danish;
        if(lang == "Dutch") return Language::Dutch;
        if(lang == "Finnish") return Language::Finnish;
        if(lang == "French") return Language::French;
        if(lang == "German") return Language::German;
        if(lang == "Greek") return Language::Greek;
        if(lang == "Hungarian") return Language::Hungarian;
        if(lang == "Italian") return Language::Italian;
        if(lang == "Japanese") return Language::Japanese;
        if(lang == "Norwegian") return Language::Norwegian;
        if(lang == "Polish") return Language::Polish;
        if(lang == "Portuguese") return Language::Portuguese;
        if(lang == "Romanian") return Language::Romanian;
        if(lang == "Russian") return Language::Russian;
        if(lang == "Serbian") return Language::Serbian;
        if(lang == "Spanish") return Language::Spanish;
        if(lang == "Taiwanese") return Language::Taiwanese;
        if(lang == "Turkish") return Language::Turkish;
        return Language::English_USA;
    }
};
typedef WisenetLanguage::Language Language;

class WisenetLoginErrorCode : public QObject
{
    Q_OBJECT
public:
    explicit WisenetLoginErrorCode(QObject *parent = nullptr) : QObject(parent){}

    enum LoginErrorCode
    {
        Success = 0,
        InvalidIdOrPassword,
        AccountBlocked,
        LdapConnectionFailed,
    };
    Q_ENUM(LoginErrorCode)

    static void Register() {
        qmlRegisterType<WisenetLoginErrorCode>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "LoginErrorCode");
    }
};
typedef WisenetLoginErrorCode::LoginErrorCode LoginErrorCode;

class WisenetPasswordStrength : public QObject
{
    Q_OBJECT
public:
    explicit WisenetPasswordStrength(QObject *parent = nullptr) : QObject(parent){}

    enum PasswordStrength
    {
        Success = 0,
        Blank,
        LengthUnderEight,
        NoNumbers,
        LengthUnderTenAndNoSymbols,
        RepeatedValue,
        ConsecutiveValue,
    };
    Q_ENUM(PasswordStrength)

    static void Register() {
        qmlRegisterType<WisenetPasswordStrength>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "PasswordStrength");
    }
};
typedef WisenetPasswordStrength::PasswordStrength PasswordStrength;

class WisenetRecordFilterType : public QObject
{
    Q_OBJECT
public:
    explicit WisenetRecordFilterType(QObject *parent = nullptr) : QObject(parent){}

    enum RecordFilterType : unsigned int
    {
        None = 0x00000000,
        Normal = 0x00000001,
        Event = 0x00000002,
        AlarmInput = 0x00000004,
        VideoAnalysis = 0x00000008,
        MotionDetection = 0x00000010,
        NetworkDisconnect = 0x00000020,
        FaceDetection = 0x00000040,
        TamperingDetection = 0x00000080,
        AudioDetection = 0x00000100,
        Tracking = 0x00000200,
        Manual = 0x00000400,
        UserInput = 0x00000800,
        DefocusDetection = 0x00001000,
        FogDetection = 0x00002000,
        AudioAnalysis = 0x00004000,
        QueueEvent = 0x00008000,
        videoloss = 0x00010000,
        EmergencyTrigger = 0x00020000,
        InternalHDDWarmup = 0x00040000,
        GSensorEvent = 0x00080000,
        ShockDetection = 0x00100000,
        TemperatureChangeDetection = 0x00200000,
        BoxTemperatureDetection = 0x00400000,
        ObjectDetectionVehicle = 0x00800000,
        ObjectDetectionFace = 0x01000000,
        ObjectDetectionPerson = 0x02000000,
        FaceRecognition = 0x04000000,
        ObjectDetectionLicensePlate = 0x08000000,
        All = 0xFFFFFFFF,
    };
    Q_ENUM(RecordFilterType)

    static void Register() {
        qmlRegisterType<WisenetRecordFilterType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "RecordFilterType");
    }
};
typedef WisenetRecordFilterType::RecordFilterType RecordFilterType;

inline RecordFilterType operator |(RecordFilterType a, RecordFilterType b)
{
    return static_cast<RecordFilterType>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline RecordFilterType operator &(RecordFilterType a, RecordFilterType b)
{
    return static_cast<RecordFilterType>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline RecordFilterType& operator |=(RecordFilterType& a, RecordFilterType b)
{
    return a = a | b;
}

inline RecordFilterType operator -(RecordFilterType a, RecordFilterType b)
{
    if((a & b) == b)
        return static_cast<RecordFilterType>(static_cast<unsigned int>(a) - static_cast<unsigned int>(b));
    else
        return a;
}

inline RecordFilterType& operator -=(RecordFilterType& a, RecordFilterType b)
{
    return a = a - b;
}

class WisenetSmartSearchType: public QObject
{
    Q_OBJECT
public:
    explicit WisenetSmartSearchType(QObject *parent = nullptr) : QObject(parent){}

    enum class SmartSearchAreaEventType
    {
        Motion = 0,
        Enter,
        Exit,
    };
    Q_ENUM(SmartSearchAreaEventType)

    enum class SmartSearchAIType
    {
        Person = 0,
        Vehicle,
        Unknown,
    };
    Q_ENUM(SmartSearchAIType)

    enum class SmartSearchLineEventType
    {
        BothDirection = 0,
        LeftDirection ,
        RightDirection,
    };
    Q_ENUM(SmartSearchLineEventType)

    enum class SmartSearchAreaType{     //only area search
        Inside = 0,
        Outside,
    };
    Q_ENUM(SmartSearchAreaType)

    enum class SmartSearchResultType{
        Motion = 0, // Area
        Enter,
        Exit,
        Pass,       // Line
    };
    Q_ENUM(SmartSearchResultType)


    static void Register() {
        qmlRegisterType<WisenetSmartSearchType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "SmartSearchAreaEventType");
        qmlRegisterType<WisenetSmartSearchType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "SmartSearchAIType");
        qmlRegisterType<WisenetSmartSearchType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "SmartSearchLineEventType");
        qmlRegisterType<WisenetSmartSearchType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "SmartSearchAreaType");
        qmlRegisterType<WisenetSmartSearchType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "SmartSearchResultType");
    }
};

class WisenetMaskType: public QObject
{
    Q_OBJECT
public:
    explicit WisenetMaskType(QObject *parent = nullptr) : QObject(parent){}

    enum class MaskType
    {
        Fixed = 0,
        Manual,
        Auto,
    };
    Q_ENUM(MaskType)

    enum class MaskPaintType
    {
        Mosaic = 0,
        Solid,
    };
    Q_ENUM(MaskPaintType)

    static void Register() {
        qmlRegisterType<WisenetMaskType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "MaskType");
        qmlRegisterType<WisenetMaskType>("Wisenet.Define", WISENET_VIEWER_MAJOR_VERSION, WISENET_VIEWER_MINER_VERSION, "MaskPaintType");
    }
};

}


