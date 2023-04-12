function getStatus(status) {
    switch(status) {
    case AutoDiscoveryViewModel.Ready:
        return Fa.fa_exclamation
    case AutoDiscoveryViewModel.Registered:
        return Fa.fa_check
    case AutoDiscoveryViewModel.ConnectionFailed:
        return Fa.fa_times
    case AutoDiscoveryViewModel.AuthFailed:
        return Fa.fa_lock
    default:
        return ""
    }
}
function getStatusColor(status) {
    switch(status) {
    case AutoDiscoveryViewModel.Ready:
        return "orange"
    case AutoDiscoveryViewModel.Registered:
        return "green"
    case AutoDiscoveryViewModel.ConnectionFailed:
        return "red"
    case AutoDiscoveryViewModel.AuthFailed:
        return "red"
    default:
        return "white"
    }
}

function calculateIp(ip, count)
{
    console.log("calculateIp:" + ip + " count:"+ count)
    var tokens = ip.split('.', 4)

    if(count < 1)
        count = 1

    console.log("calculateIp:" + count)

    let numberTokens = []

    for(var item in tokens){
        numberTokens[item] = Number(tokens[item])
    }

    numberTokens[3] += (count -1)
    //console.log(numberTokens[3])

    let maxByte = 256
    let maxIp = 255

    if (numberTokens[3] > maxIp)
    {
        numberTokens[2] = numberTokens[2] + parseInt(numberTokens[3] / maxByte);
        numberTokens[3] = numberTokens[3] % maxByte;

        if (numberTokens[2] > maxIp)
        {
            numberTokens[1] = numberTokens[1] + parseInt(numberTokens[2] / maxByte);
            numberTokens[2] = numberTokens[2] % maxByte;
            if (numberTokens[1] > maxIp)
            {
                numberTokens[0] = numberTokens[0] + parseInt(numberTokens[1] / maxByte);
                numberTokens[1] = numberTokens[1] % maxByte;
            }
        }
    }
    var ret=  numberTokens[0] + "." + numberTokens[1] + "." + numberTokens[2] + "." + numberTokens[3]
    return ret
}

function calculateGateway(ip)
{
    console.log("calculateGateway(ip)" + ip)
    var tokens = ip.split('.')

    var numberTokens = []

    for(var item in tokens){
        numberTokens[item] = tokens[item]
    }

    var ret = ""
    if(numberTokens[0])
        ret = numberTokens[0] + "."
    if(numberTokens[1])
        ret += (numberTokens[1] + ".")
    if(numberTokens[2])
        ret += (numberTokens[2] + ".1")

    return ret
}
