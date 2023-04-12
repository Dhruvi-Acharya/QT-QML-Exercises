import QtQuick 2.15
//import QtQuick.Controls 2.15 as Control2
import QtQml.Models 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import Wisenet.Define 1.0
import WisenetLanguage 1.0

Item {
    id:searchBoxAutofill
    width: parent.width
    height: 30

    property int searchboxIcon_x: 13
    property int autofillList_x: 34 - 2// 34: WisenetSearhBox's searchImage's x(13) + its width(11) + its margin(10)
    property int autofillList_y: 0
    property int autofillList_height:400
    property var dictionary: [{word:"",word_data:""}]
    onDictionaryChanged: {
        searchBoxAutofill.dictionary.forEach(function(element){
            dictModel.append(element);
        })
    }
    onVisibleChanged: {
        if(visible===false){
            searchBoxAutofill.reset();
        }
    }
    signal returnData( var word_data )
    signal searchTextChanged(var text)
    property color searchboxBgColor : WisenetGui.contrast_08_dark_grey
    property alias searchboxBorderColor : searchBoxId.borderColor
    property alias searchboxBorderWidth : searchBoxId.borderWidth

    function reset()
    {
        searchBoxId.reset();
    }

    function searchDictionary( text ){
        model_suggestion.clear();
        //var dict_word = text;
        if( text !== "" && text !== " ")
        {
            const regex = new RegExp( text, "i" );
            for( var i = 0; i < dictModel.count; ++i)
            {
                var dict_word = dictModel.get(i).word;
                //console.log( "#"+i+"Comparing... ["+text+"] == ["+dict_word+"] ?")
                if( regex.test(dict_word) )
                {
                    //console.log( "#"+i+"Comparing... ["+text+"] == ["+dict_word+"] YES")
                    model_suggestion.append( {"word":dict_word, "word_data":dictModel.get(i).word_data} );
                    autofillBG.visible=true;
                }
            }
        }
        else
        {
            autofillBG.visible=false;
        }
    }

    ListModel{
        id:model_suggestion
        ListElement {
            word: ""
            word_data: 0
        }
    }
    ListModel{
        id:dictModel
        ListElement {
            word: "";
            word_data: 0
        }
    }

    WisenetSearchBox{
        id:searchBoxId
        x:0
        y:0
        width:parent.width
        height:parent.height
        z:100
        onSearchTextChanged:{
            list.currentIndex = -1;
            searchDictionary( text );
            searchBoxAutofill.searchTextChanged( text);
            //???
            //autofillBG.update();
        }
        icon_x: searchboxIcon_x
        bgColor: searchboxBgColor
        borderWidth: 0
        borderColor: WisenetGui.transparent
        onDownKeyPressed: {
            if (list.currentIndex < list.count) {
                list.incrementCurrentIndex();
                setText(list.currentItem.text)
            }
        }
        onUpKeyPressed: {
            if (list.currentIndex > 0) {
                list.decrementCurrentIndex();
                setText(list.currentItem.text)
            }
        }
        onAccepted: {
            if (list.currentIndex > 0) {
                var currentModel = list.currentItem.wordData;
                returnData(currentModel);
                searchBoxAutofill.reset();
                model_suggestion.clear();
                list.currentIndex = -1;
            }
        }
        onEscapePressed: {
            if (list.currentIndex !== -1) {
                undoText();
                model_suggestion.clear();
                list.currentIndex = -1;
            }
        }

        Component.onCompleted: {
            searchBoxId.resetPlaceholderText()
        }
    }

    Rectangle{
        id:autofillBG
        width:parent.width - autofillList_x
        property int listitem_height : 20
        height:{(listitem_height*model_suggestion.count)>autofillList_height?autofillList_height: listitem_height*model_suggestion.count}
        x : autofillList_x
        y : searchBoxId.height + autofillList_y
        z : 99
        color : WisenetGui.contrast_09_dark
        visible:false
        border.width: 1
        border.color: "#797979"
        clip: true

        ListView{
            id:list
            anchors.fill: parent
            model: model_suggestion
            ScrollBar.vertical: WisenetScrollBar {
                id: verticalScrollbar
            }
            highlightMoveDuration: 0
            highlight: Rectangle {
                width:parent.width
                height:autofillBG.listitem_height
                x:0
                color: WisenetGui.color_primary_dark
            }

            delegate: Rectangle{
                id:listitem
                width:parent.width
                height:autofillBG.listitem_height
                x:0
                color: WisenetGui.transparent
                property alias text: searchText.text
                property var wordData : word_data;

                Text{
                    id: searchText
                    height: parent.height
                    width:parent.width
                    x:0
                    y:0
                    text:word
                    leftPadding: 5
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    //wrapMode:Text.WrapAnywhere
                    color:WisenetGui.contrast_04_light_grey
                    clip: true
                }
                MouseArea{
                    id:mouseArea
                    anchors.fill: parent
                    onClicked: {
                        returnData(word_data);
                        searchBoxAutofill.reset();
                    }
                    hoverEnabled: true
                    onEntered: list.currentIndex = index
                }
            }

        }
    }
}
