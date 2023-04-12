
function addDevice(name, ip, deviceID, channelID, status)
{
    devices.append({
                       "deviceName": name,
                       "deviceIP": ip,
                       "deviceID": deviceID,
                       "channelID": channelID,
                       "status": status
                   });
}

function updateDevice(deviceID, channelID, status)
{
    for(var i = 0 ; i < devices.count ; i++) {
        var device = devices.get(i);
        if (device.deviceID === deviceID && device.channelID === channelID) {
            console.log("update status", channelID, status);
            device.status = status;
            break;
        }
    }
}

function addVideoFile(videoFilePath)
{
    console.log('addVideoFile,', videoFilePath);
    for (var i = 0 ; i < videos.count ; i++) {
        if (videos.get(i).filePath === videoFilePath) {
            console.log('already exist video item, skip=', videoFilePath);
            return;
        }
    }

    videos.append({
                       "filePath": videoFilePath
                   });
}


function addEvent(description)
{
    console.log('addEvent,', description);
    events.append({
                       "description": description
                   });
}


function addWebpage(name, url, webpageId)
{
    webpages.append({
                        "webpageName": name,
                        "webpageUrl": url,
                        "webpageID": webpageId
                    });
}
