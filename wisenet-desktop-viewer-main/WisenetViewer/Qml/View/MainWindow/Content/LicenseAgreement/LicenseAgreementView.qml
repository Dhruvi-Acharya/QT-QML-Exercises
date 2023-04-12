import QtQuick 2.15
import QtQuick.Controls 2.15
import WisenetStyle 1.0
import WisenetLanguage 1.0

Item{
    id: licenseAgreementView
    //anchors.fill: parent
    //color: WisenetGui.contrast_11_bg

    width: 800
    height: 600
    Rectangle{
        anchors.fill: parent

        color: WisenetGui.contrast_09_dark
        border.color: WisenetGui.contrast_06_grey
        border.width: 1

        Text{
            id: title
            y: 20
            height: 33
            anchors.horizontalCenter: parent.horizontalCenter
            text: WisenetLinguist.licenseAgreement
            color: WisenetGui.contrast_02_light_grey
            font.pixelSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle{
            color: "transparent"

            anchors.top: title.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: collectPolicyRect.top

            anchors.topMargin: 54
            anchors.leftMargin: 50
            anchors.rightMargin: 50
            anchors.bottomMargin: 14

            Text {
                id: warnText
                width: parent.width
                text: WisenetLinguist.licenseWarn
                color: WisenetGui.contrast_02_light_grey
                padding: 0
                wrapMode: Text.Wrap
                font.pixelSize: 12
            }

            Flickable {
                anchors.fill: parent
                anchors.topMargin: warnText.height + 20
                anchors.bottomMargin: acceptCheckbox.height + 14
                contentWidth: width
                contentHeight: txt.height
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: WisenetScrollBar{}
                Rectangle {
                    anchors.fill: parent
                    color: WisenetGui.contrast_08_dark_grey
                }

                Text {
                    id: txt
                    width: parent.width
                    text: licenseTxt
                    color: WisenetGui.contrast_04_light_grey
                    wrapMode: Text.Wrap
                    topPadding: 12
                    bottomPadding: 12
                    leftPadding: 20
                    rightPadding: 20
                    font.pixelSize: 10
                }
            }

            WisenetCheckBox2 {
                id: acceptCheckbox
                anchors.bottom: parent.bottom
                width: parent.width
                text: WisenetLinguist.licenseAccept

                onCheckedChanged: {
                    if(checked == false){
                        acceptAllCheckBox.checked = false
                    }
                }
            }
        }

        Rectangle{
            id: collectPolicyRect

            height: 70
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: acceptAllCheckBox.top
            anchors.leftMargin: 50
            anchors.rightMargin: 50
            anchors.bottomMargin: 14

            color: "transparent"

            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: acceptCollectingCheckbox.top
                anchors.bottomMargin: 14
                color: WisenetGui.contrast_08_dark_grey

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: WisenetLinguist.appImprovementSentence
                    color: WisenetGui.contrast_04_light_grey
                    wrapMode: Text.Wrap
                    font.pixelSize: 10
                }
            }

            WisenetCheckBox2 {
                id: acceptCollectingCheckbox
                anchors.bottom: parent.bottom
                width: parent.width
                text: WisenetLinguist.participateImprovement

                onCheckedChanged: {
                    if(checked == false){
                        acceptAllCheckBox.checked = false
                    }
                }
            }
        }

        WisenetCheckBox2 {
            id: acceptAllCheckBox
            anchors.bottom: okButton.top
            anchors.bottomMargin: 14
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 50
            anchors.rightMargin: 50

            text: WisenetLinguist.acceptAll

            onCheckedChanged: {
                if(checked == true){
                    acceptCheckbox.checked = true
                    acceptCollectingCheckbox.checked = true
                }
            }
        }

        WisenetGrayButton{
            id: okButton
            enabled: acceptCheckbox.checked
            width: 282
            height: 36
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 25
            anchors.horizontalCenter: parent.horizontalCenter

            text: WisenetLinguist.ok
            onClicked: {
                mainViewModel.setCollectingAgreed(acceptCollectingCheckbox.checked)
                mainViewModel.setLicenseAgreed()
            }
        }
    }

    property string licenseTxt:
        "SOFTWARE LICENSE AGREEMENT


This is an Agreement between You (either an individual or entity) and HANWHA VISION CO., LTD., a corporation duly organized and existing under the laws of the Republic of Korea, with its principal office at 701, Sampyung-Dong, Bundang-Gu, Seongnam-City, Gyeonggi-Do 463-400, Korea(hereinafter referred to as “Licensor”). By installing, copying or otherwise using the SOFTWARE, you are agreeing to be bound by the terms of this agreement. If you do not agree with these terms and conditions of this Agreement, do not download or use this Software and delete or destroy all copies of the product in your possession.


1. Definition

“You” mean the user of the license rights by this Agreement (also referred to as “Licensee”)

“Software” means Wisenet Viewer software and its components, in Source Code or Object Code form.

“Documentation” means all user guides, reference manuals, other documentation accompanying Software (as defined below), and all other documentation otherwise made available by Licensor

“Licensed Product” means the Software (as defined below) and Documentation and any copies or modifications.


2. Grant of License.

Subject to the terms and conditions of this Agreement, Licensor grants to you the right to use one (1) copy of the Software on one (1) computer and You hereby accepts, a non-exclusive license, non-transferable and non-assignable license to use the Software and the Documentation with the term of this license commencing upon the furnishing of the Software and expiring upon expiration or termination of this Agreement.

The Software is in \"use\" on a computer when it is loaded into temporary memory (i.e. RAM) or installed into permanent memory (e.g. CD-ROM, hard disk, or other storage devices) of that computer.


3. Ownership.

All title, ownership or intellectual property rights, including, but not limited to, copyrights, in and to the Software and copies thereof are owned by Licensor and/or its licensor(s), if any, and are protected by intellectual property laws and international treaty provisions.


4. Restrictions.

You shall not alter, improve, enhance, or otherwise modify, reverse, decompile or disassemble the Software, in whole or in part without Licensor’s prior written consent. In the event of any alteration, improvement, enhancement, or other modification of the Software, You shall give notice to Licensor, with such notice explaining the same in detail and accompanied by a copy of the Software as altered, improved, enhanced, or modified.

You shall not title to and ownership in such alteration, improvement, enhancement, or other modification shall belong exclusively to the Licensor, and You shall possess no interest therein; except that the Licensor shall grant the Licensor a non-exclusive, non-transferable, non-assignable license to use such alteration, improvement, enhancement, or other modification during the term hereof transfer, assign, rent, lease or sublicense the Software without the prior written consent of Licensor thereof.

You shall be liable for any and all damages or claims to Licensor or any third party caused by any alteration, improvement, enhancement, or other modification of Software, Documentation or Licensed Product.

You shall not assign, sublicense, or otherwise transfer in whole or in part, the license granted under this Agreement and You shall not sell, license, lease, publish, distribute or otherwise transfer in whole or in part the Software or the Documentation to any third party.


5. Third Party Rights.

The Software may contain third party’s software or intellectual property rights, which require additional terms and conditions. Such required additional terms and conditions are located in the manual. By accepting this Agreement, you are also accepting the additional terms and conditions, if any, set forth therein. “Licensor” does not guarantee or warranty any non-infringement of any third party’s rights including intellectual property rights.


6. Term and Termination.

This Agreement is effective until terminated. Your rights under this Agreement will terminate automatically without notice from Licensor if you fail to comply with any term(s) of this Agreement. Upon the termination of this Agreement, you shall cease all use of the Software and destroy all copies of the Software in whole.

Upon expiration or termination of this Agreement, all rights and licenses granted under this Agreement shall terminate, and You thereafter shall not make use of, or claim any rights in or to the use of, the Software or Licensed Product.


7. Governing Law.

This agreement shall be governed by and construed in accordance with the laws of the Republic of Korea.


8. NO WARRANTY.

LICENSOR EXPRESSLY DISCLAIM ANY WARRANTY FOR THE SOFTWARE. LICENSOR DOES NOT WARRANT THAT THE FUCTIONS CONTAINED IN THE LICENSED PRODUCT WILL MEET YOUR REQUIREMENT OR THAT OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE. THE SOFTWARE IS PROVIDED “AS IS’ WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSE OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.

The entire risk arising out of use or performance of the Software remains with you as the user. You are notified that the Software, when used with certain equipment or other Software, may enable you to perform surveillance actions and data processing which may be restricted by or contrary to applicable laws, including, but not limited to, data protection and criminal law. The sole responsibility to verify that your use is in compliance with applicable laws lies with you as the user.


9. LIMITATION OF LIABILITY

IN NO EVENT SHALL LICENSOR OR ITS SUPPLIER BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, BUT NOT LIMITED TO, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) NOR FOR ANY PRODUCT LIABILITY, UNLESS IT IS ENFORCED BY THE LAWS, ARISING OUT OF THE USE OF OR INABILITY TO USE THE SOFTWARE OR THE PROVISION OF OR FAILURE TO PROVIDE PROPER SUPPORT, EVEN IF LICENSOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. THE ENTIRE LIABILITY OF LICENSOR AND ITS SUPPLIERS SHALL BE LIMITED TO THE AMOUNT ACTUALLY PAID BY YOU FOR THE SOFTWARE OR LICENSED PRODUCT.


10. Miscellaneous.

You may make a copy of the Software only for backup and archival purposes.

You agree that this Agreement is the complete statement of the agreement between Licensor and You, and supersedes any proposal or prior agreement, oral or written, and any other communications relating to the subject matter of this Agreement.

If any provision of this Agreement become or is declared by a court of competent jurisdiction to be unenforceable, the remaining provisions of this Agreement shall continue in full force and effect.

All article and headings contained in the Agreement are for reference and in no way define, limit or extend this Agreement or the intent of any of its provisions.


11. Notices from HEVC Advance

Covered by one or more claims of the patents listed at patentlist.hevcadvance.com."

}



