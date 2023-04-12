pragma Singleton
import QtQuick 2.15

QtObject{

    readonly property int borderWidth: 2

    readonly property color backgroundColor : "#282828"
    readonly property color gridBackgroundColor : "#101010"
    readonly property real  gridBackgroundOpacity : 0.7
    readonly property color borderColor: "#303030"
    readonly property color headerColor: "#303030"
    readonly property color orangeColor: "#FFA800"

    readonly property color layoutTabColor: "#232323"
    readonly property color selectedLayoutBorderColor: "#D2D2D2"
    readonly property color layoutTextColor: "#919191"
    readonly property color selectedLayoutTextColor: "#D2D2D2"

    readonly property color tabBackgroundColor : "#484848"
    readonly property color tabDefaultTextColor : "#8e8e8e"

    readonly property color hoveredTextColor : "white"

    readonly property color hoverColor: "#585858"
    readonly property color pressColor: "#484848"
    readonly property color normalColor: "#505050"

    readonly property color gray : "#808080"
    readonly property color black : "#000000"
    readonly property color white : "#ffffff"
    readonly property color orange : "#ffa500"
    readonly property color dimgray : "#696969"

    readonly property color textColor: "#FFFFFF"
    readonly property color textBoxTextColor: "white"
    readonly property color textBoxBackgroundColor : "#181818"
    readonly property color textBoxSelectColor : "orange"
    readonly property color buttonHovered : "gray"
    readonly property color transparent : "transparent"
    readonly property color testBorderColor : "white"

    readonly property color menuSeparatorColor: "#505050"
    readonly property color menuItemColor: "#606060"
    readonly property color menuItemHoveredColor: "#FFA800"


    // Tree text color
    readonly property color treeTextColorLoading: "lightgray"
    readonly property color treeTextColorNormal: "white"
    readonly property color treeTextColorAbnormal: "gray"
    readonly property color treebranchColor: "#686868"

    //Tablview background borderColor
    readonly property color tableSelectedHovered: "#fab778"

    readonly property int popupWindowMinimumWidht: (1280 - 30)
    readonly property int popupWindowMinimumHeight: (720 - 30)
    readonly property int popupSecondWindowDefaultWidth : (1280 - 60)
    readonly property int popupSecondWindowDefaultHeight : (720 - 60)
    readonly property int addEventRuleWindowDefaultWidth: 877
    readonly property int addEventRuleWindowDefaultHeight: 720

    readonly property int addEventScheduleWindowDefaultWidth: 450
    readonly property int addEventScheduleWindowDefaultHeight: 300


    readonly property int eventSearchWindowDefaultWidth: (1280 - 60)
    readonly property int eventSearchWindowDefaultHeight: (720 - 60)
    readonly property int eventSearchWindowMinimumWidht: (1280 - 60)
    readonly property int eventSearchWindowMinimumHeight: (720 - 60)
    readonly property int eventSearchFilterDefaultWidth: 243

    readonly property int footerButtonWidth: 120


    readonly property int popupDeviceTreeDefaultWidth: 400
    readonly property int popupDeviceTreeDefaultHeight: 550

    readonly property color setupPageLine: "#313131"


    // colors defined in GUI guide
    readonly property color contrast_00_white: "#FFFFFF"
    readonly property color contrast_01_light_grey: "#FFFFFF"
    readonly property color contrast_02_light_grey: "#E5E5E5"
    readonly property color contrast_03_light_grey: "#D2D2D2"
    readonly property color contrast_04_light_grey: "#C4C4C4"
    readonly property color contrast_05_grey: "#AAAAAA"
    readonly property color contrast_06_grey: "#909090"
    readonly property color contrast_07_grey: "#5C5C5C"
    readonly property color contrast_08_dark_grey: "#313131"
    readonly property color contrast_09_dark: "#232323"
    readonly property color contrast_10_dark: "#1D1D1D"
    readonly property color contrast_11_bg: "#121212"
    readonly property color contrast_12_black: "#000000"
    readonly property color color_setup_Background : "#121212"

    readonly property color color_primary: "#F37321"
    readonly property color color_primary_opacity50: "#80F37321"
    readonly property color color_primary_dark: "#CC601A"
    readonly property color color_primary_deep_dark: "#9C4811"
    readonly property color color_secondary_bright: "#B5FF51"
    readonly property color color_secondary: "#8DC63F"
    readonly property color color_secondary_dark: "#496621"
    readonly property color color_secondary_dark_opacity30: "#4C496621"
    readonly property color color_accent: "#FF0000"
    readonly property color color_accent_dark: "#8A0000"
    readonly property color color_caution: "#FFA722"
    readonly property color color_yellow: "#FFE91F"
    readonly property color color_live_control_bg: "#333333"
    readonly property color color_orange2: "#F59100"
    readonly property color color_setup_line: "#252525"
    readonly property color color_primary_press: "#924514"
    readonly property color color_tertiary: "#FFE91F"
    readonly property color color_sky: "#00D1FF"
    readonly property color color_purple: "#FFE91F"
    readonly property color color_timeline: "#3FC6AE"

    // Dashboard
    readonly property color color_dashboard_power_error: "#FFA722"
    readonly property color color_dashboard_recording_error: "#7C88EB"
    readonly property color color_dashboard_overload_error: "#D13022"
    readonly property color color_dashboard_connection_error: "#4356FF"
    readonly property color color_dashboard_fan_error: "#CC601A"
    readonly property color color_dashboard_camera_disconnected_error: "#2738D0"

    // Schedule
    readonly property color color_schedule_onDrag: "#DE8A53"
    readonly property color color_schedule_offDrag: "#454545"

    // Smart Search
    readonly property color color_smartSearch_line: "#01CFFC"
    readonly property color color_smartSearch_roi: "#0700F7"
    readonly property color color_smartSearch_roi_30: "#4C0700F7"
    readonly property color color_smartSearch_exclude: "#70C102"
    readonly property color color_smartSearch_exclude_30: "#4C70C102"

    // Masking
    readonly property color color_masking_fixed: "#FF24F6"
    readonly property color color_masking_manual: "#B5FF51"
    readonly property color color_masking_auto: "#00D1FF"
}
