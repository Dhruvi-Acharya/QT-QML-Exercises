
function getResourceIcon(displayName){

    if("MotionDetection" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/motiondetection-normal.svg";
    }
    else if("VideoAnalytics"  === displayName || "AudioAnalytics"  === displayName){
        return "qrc:/WisenetStyle/Icon/Event/videoaudioanalytics-normal.svg";
    }
    else if("Tampering" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/tamperingdetection-normal.svg";
    }
    else if("Tracking" === displayName || "DigitalAutoTracking" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/tracking.svg";
    }
    else if("DefocusDetection" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/defocusdetection-normal.svg";
    }
    else if("FogDetection" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/fogdetection-normal.svg";
    }
    else if("AudioDetection" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/audiodetection-normal.svg";
    }
    else if(displayName === "AlarmInput" || displayName === "NetworkAlarmInput" ){  //kkd
        return "qrc:/WisenetStyle/Icon/Event/alarmIn.svg";
    }
    else if("Videoloss" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/videoLoss.svg";
    }
    else if("FaceDetection" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/facedetection-normal.svg";
    }
    else if("DevicePowerStatus" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/powerStatus-normal.svg";
    }
    else if("DeviceFanStatus" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/fanStatus-normal.svg";
    }
    else if( "DeviceStorageStatus" === displayName ){
        return "qrc:/WisenetStyle/Icon/Event/storageStatus-normal.svg";
    }
    else if("DeviceRecordingStatus" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/recordingStatus-normal.svg";
    }
    else if("DeviceSystemStatus" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/systemError.svg";
    }
    else if("DeviceSystemConnected" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/connect.svg";
    }
    else if("DeviceSystemDisconnected" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/disconnect.svg";
    }
    else if("Firmware" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/firmware.svg";
    }
    else if("Overload" === displayName){
        return "qrc:/WisenetStyle/Icon/Event/overload.svg";
    }
    else if(displayName === "Alarm output"){
        return "qrc:/WisenetStyle/Icon/Event/alarmout.svg";
    }
    else if(displayName === "AlertAlarm"){
        return "qrc:/WisenetStyle/Icon/Event/alertalarm.svg";
    }
    else if(displayName === "Open layout"){
        return "qrc:/WisenetStyle/Icon/Tree/tree_layout_normal.svg";
    }

    //Dynamic event
    else if(displayName === "TemperatureDetection"){
        return "qrc:/WisenetStyle/Icon/Event/temperatureDetection.svg";
    }
    else if(displayName === "BodyTemperatureDetection"){
        return "qrc:/WisenetStyle/Icon/Event/bodyTemperatureDetection.svg";
    }
    else if(displayName === "MaskDetection"){
        return "qrc:/WisenetStyle/Icon/Event/maskDetection.svg";
    }
    else if(displayName === "ShockDetection "){
        return "qrc:/WisenetStyle/Icon/Event/shockDetection.svg";
    }
    else if(displayName === "ObjectDetection"){
        return "qrc:/WisenetStyle/Icon/Event/objectDetection.svg";
    }
    else if(displayName === "ObjectDetection.Vehicle"){
        return "qrc:/WisenetStyle/Icon/Event/obj_car.svg";
    }
    else if(displayName === "ObjectDetection.Person"){
        return "qrc:/WisenetStyle/Icon/Event/obj_person.svg";
    }
    else if(displayName === "ObjectDetection.LicensePlate"){
        return "qrc:/WisenetStyle/Icon/Event/obj_licenseplate.svg";
    }
    else if(displayName === "ObjectDetection.Face"){
        return "qrc:/WisenetStyle/Icon/Event/obj_face.svg";
    }
    else if(displayName === "FaceRecognition"){
        return "qrc:/WisenetStyle/Icon/Event/faceRecognition-normal.svg";
    }
    else if(displayName === "PTZMotion"){
        return "qrc:/WisenetStyle/Icon/Event/ptzMotion-normal.svg";
    }
    else if(displayName === "Queue"){
        return "qrc:/WisenetStyle/Icon/Event/queue-normal.svg";
    }
    else if (displayName === "E-mail"){
        return "qrc:/WisenetStyle/Icon/Event/email.svg";
    }
    else if( displayName.includes("OpenSDK")){
        console.debug("ResourceName :" + displayName)
        return "qrc:/WisenetStyle/Icon/Event/ptzMotion-normal.svg";
    }
    else{
        console.debug("ResourceName :" + displayName)
        return "qrc:/WisenetStyle/Icon/Tree/tree_camera_normal.svg";
    }


}
