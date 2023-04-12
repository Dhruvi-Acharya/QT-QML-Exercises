import QtQuick 2.15
import WisenetStyle 1.0
import "qrc:/WisenetStyle/"

Rectangle{
    id: chartView

    property alias depth1Data : lineChartDepth1.chartData
    property alias depth2Data : lineChartDepth2.chartData
    property alias depth3Data : lineChartDepth3.chartData
    property alias depth4Data : lineChartDepth4.chartData

    property int depth2Width : 1050
    property int depth3Width : 1950
    property int depth4Width : 3900
    property int maxWidth: 4000

    component LegendTypeItem: Rectangle{
        property alias name: nameText.text
        property alias lineColor: lineColorRect.color

        anchors.verticalCenter: parent.verticalCenter

        width: nameText.width + lineColorRect.width + 6
        height: 14

        color: "transparent"

        Rectangle{
            id: lineColorRect

            width: 12
            height: 12
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 1
        }

        Text{
            id: nameText

            anchors.left: lineColorRect.right
            anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter

            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 10
        }
    }

    onWidthChanged: {
        if(chartContainer.width > lineChartDepth1.width)
        {
            lineChartDepth1.width = chartContainer.width
            lineChartDepth2.width = chartContainer.width
            lineChartDepth3.width = chartContainer.width
            lineChartDepth4.width = chartContainer.width
        }
    }

    onHeightChanged: {
        console.log("ChartView.onHeightChanged", height)

        lineChartDepth1.height = chartContainer.height
        lineChartDepth2.height = chartContainer.height
        lineChartDepth3.height = chartContainer.height
        lineChartDepth4.height = chartContainer.height
    }

    function chartVisibleChange(){
        if(lineChartDepth1.width > chartView.depth2Width && lineChartDepth1.width <= chartView.depth3Width)
        {
            lineChartDepth1.visible = false
            lineChartDepth2.visible = true
            lineChartDepth3.visible = false
            lineChartDepth4.visible = false
        }
        else if(lineChartDepth1.width > chartView.depth3Width && lineChartDepth1.width <= chartView.depth4Width)
        {
            lineChartDepth1.visible = false
            lineChartDepth2.visible = false
            lineChartDepth3.visible = true
            lineChartDepth4.visible = false
        }
        else if(lineChartDepth1.width > chartView.depth4Width)
        {
            lineChartDepth1.visible = false
            lineChartDepth2.visible = false
            lineChartDepth3.visible = false
            lineChartDepth4.visible = true
        }
        else
        {
            lineChartDepth1.visible = true
            lineChartDepth2.visible = false
            lineChartDepth3.visible = false
            lineChartDepth4.visible = false
        }
    }

    function getChartOption(){
        return {
            maintainAspectRatio: false,
            responsive: true,
            title: {
                display: false,
            },
            tooltips: {
                mode: 'index',
                intersect: false,
                backgroundColor: '#313131',
                titleFontSize: 10,
                titleFontColor: '#C4C4C4',
                bodyFontSize: 12,
                bodyFontColor: '#E5E5E5',
                xPadding: 13,
                yPadding: 12,
                borderWidth: 1,
                borderColor: '#5C5C5C',
                bodySpacing: 4,
                //multiKeyBackground: '#000000',
                //displayColors: false,

            },
            hover: {
                mode: 'nearest',
                intersect: true
            },
            legend: {
                display: false
            },
            scales: {
                xAxes: [{
                        display: true,
                        scaleLabel: {
                            display: false,
                            //labelString: 'Day'
                        },
                        gridLines: {
                            color: '#282828',
                            zeroLineColor: '#282828'
                        },
                        ticks: {
                            //beginAtZero:true,
                            fontColor: '#C4C4C4'
                        }
                    }],
                yAxes: [{
                        display: true,
                        scaleLabel: {
                            display: false,
                        },
                        gridLines: {
                            zeroLineColor: '#C4C4C4'
                        },
                        ticks: {
                            display: false
                        }
                    }]
            }
        }
    }

    // Zoom in/out
    MouseArea{
        anchors.fill: chartContainer
        onWheel: {
            console.log("ChartView. onWheel ", wheel.x, chartContainer.contentX, chartContainer.contentWidth)
            if(wheel.angleDelta.y > 0)
            {
                if(lineChartDepth1.width + 50 <= chartView.maxWidth){
                    console.log("up 1", lineChartDepth1.width)
                    lineChartDepth1.width = lineChartDepth1.width + 50
                    lineChartDepth2.width = lineChartDepth1.width
                    lineChartDepth3.width = lineChartDepth1.width
                    lineChartDepth4.width = lineChartDepth1.width
                    console.log("up 1", lineChartDepth1.width)

                    // chartContainer.width : wheel.x = chartContainer.width+50 : wheel.x + dX
                    // dX = chartContainer.width+50*wheel.x / chartContainer.width - wheel.x
                    chartContainer.contentX = chartContainer.contentX + ((chartContainer.width + 50)*wheel.x/chartContainer.width - wheel.x)
                }
            }
            else
            {
                console.log("down 1", lineChartDepth1.width)
                if(chartContainer.width < lineChartDepth1.width){
                    lineChartDepth1.width = lineChartDepth1.width - 50
                    lineChartDepth2.width = lineChartDepth1.width
                    lineChartDepth3.width = lineChartDepth1.width
                    lineChartDepth4.width = lineChartDepth1.width

                    chartContainer.contentX = chartContainer.contentX + ((chartContainer.width - 50)*wheel.x/chartContainer.width - wheel.x)

                    if(chartContainer.contentX < 0)
                        chartContainer.contentX = 0
                }
                console.log("down 2", lineChartDepth1.width)
            }
        }
    }

    /*
    Text{
        id: chartTitleText

        height:20
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 19
        anchors.leftMargin: 23

        text: WisenetLinguist.status

        color: WisenetGui.contrast_02_light_grey
        font.pixelSize: 20

        verticalAlignment: Text.AlignVCenter
    }

    Rectangle{
        id: chartSeparator

        height: 1
        anchors.top: chartTitleText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 19

        color: WisenetGui.contrast_08_dark_grey
    }*/

    // Chart content
    Flickable{
        id: chartContainer
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: chartlegend.top

        anchors.topMargin: 28
        anchors.leftMargin: 28
        anchors.rightMargin: 28
        anchors.bottomMargin: 18

        contentWidth: lineChartDepth1.width
        clip:true

        WisenetChart {
            id: lineChartDepth1

            onWidthChanged: chartView.chartVisibleChange()

            width: parent.width
            height: parent.height
            x:0
            y:0
            animationDuration: 0
            chartType: 'line'
            chartOptions: chartView.getChartOption()
        }

        WisenetChart {
            id: lineChartDepth2

            width: parent.width
            height: parent.height
            x:0
            y:0
            animationDuration: 0
            visible: false
            chartType: 'line'
            chartOptions: chartView.getChartOption()
        }

        WisenetChart {
            id: lineChartDepth3

            width: parent.width
            height: parent.height
            x:0
            y:0
            animationDuration: 0
            visible: false
            chartType: 'line'
            chartOptions: chartView.getChartOption()
        }

        WisenetChart {
            id: lineChartDepth4

            width: parent.width
            height: parent.height
            x:0
            y:0
            animationDuration: 0
            visible: false
            chartType: 'line'
            chartOptions: chartView.getChartOption()
        }
    }

    Rectangle{
        id: chartlegend

        height: 40

        anchors.rightMargin: 24
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        color: "transparent"

        Text{
            id: errorTypeText

            anchors.top: parent.top
            anchors.right: legendRow.left
            anchors.rightMargin: 16

            text: WisenetLinguist.errorType
            font.pixelSize: 12
            color: WisenetGui.contrast_04_light_grey

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
        }

        Row{
            id: legendRow
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 3

            spacing: 7

            LegendTypeItem{
                id: powerError

                name: WisenetLinguist.power
                lineColor: WisenetGui.color_dashboard_power_error
            }

            LegendTypeItem{
                id: recordingError

                name: WisenetLinguist.recording
                lineColor: WisenetGui.color_dashboard_recording_error
            }

            LegendTypeItem{
                id: overloadError

                name: WisenetLinguist.overload
                lineColor: WisenetGui.color_dashboard_overload_error
            }

            LegendTypeItem{
                id: connectionError

                name: WisenetLinguist.connection
                lineColor: WisenetGui.color_dashboard_connection_error
            }

            LegendTypeItem{
                id: fanError

                name: WisenetLinguist.fan
                lineColor: WisenetGui.color_dashboard_fan_error
            }

            LegendTypeItem{
                id: disconnectedError

                name: WisenetLinguist.cameraDisconnected
                lineColor: WisenetGui.color_dashboard_camera_disconnected_error
            }
        }
    }
}

