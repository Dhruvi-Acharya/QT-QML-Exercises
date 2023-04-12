var preface_troubleshooting =  '<div class="item_div_cont" data-content="p0103">'+
''+
'    <h3><a class="heading-offset-anchor" id="p0103"></a>Troubleshooting</h3>'+
'    <br><br>'+
'    <ul>'+
'        <li>The device is not connected.</li>'+
'        <dd>Enter the IP and Port information that was entered during device registration in Wisenet Viewer into the web browser and check if the web browser opens normally.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>When connecting with http://IP, enter 80 (default port) in the Port item.</li>'+
'                <li>When connecting with http://IP, enter 443 (default port) in the Port item.</li>'+
'            </ol>'+
'        </dd>'+
'        <dd>To strengthen security, the Wisenet Viewer first connects with https if the device information is set in https.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>Some of the former recorder firmware may have passed the wrong https port number, disabling the connection to the device.</li>'+
'                <li>Some of the former recorder firmware may not be connected with https, disabling the connection to the device.</li>'+
'            </ol>'+
'        </dd>'+
''+
'        <li>It does not appear on the list when searching for a device.</li>'+
'        <dd>Check whether the device list is displayed normally in the device manager.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>If the device list appears normally in the device manager but does not on Wisenet Viewer, check whether the execution permission is set <b>No</b> when first running the Wisnet Viewer program.</li>'+
'            </ol>'+
'        <dd>Check whether Wisenet Viewer is registered in the Windows firewall and is being blocked. </dd>'+
''+
'        <li>Wisenet Viewer&apos;s detailed log history is required.</li>'+
'        <dd>If a problem persists while using Wisenet Viewer and you need to contact the customer service center, collect and deliver the system log. When the system log delivers the collected folder in a compressed file format to the customer service center, we can diagnose the problem precisely and help you solve the problem.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>For more information on system log collection, refer to <label class="viewbtns" data-id="setup_system_systemlog" onclick="location.href=\'#p100205\';">Configuring system log collection</label>.</li>'+
'            </ol>'+
'        </dd>'+
''+
'        <li>What information is displayed when you press the <i class="img_preveal_p_i"><img src="./image/sub/i12.svg" /></i>　　button on the function button panel?</li>'+
'        <dd>It will show the currently received frame rate. The frame rate displays the number of frames buffering for the video equivalence.</dd>'+
''+
'        <li>I want to play the recorded video by double clicking the event as soon as the event occurs on the event panel, but the video is not displayed.</li>'+
'        <dd>It may vary depending on the platform of the recorder, but usually a recorder saves videos on an HDD when 2 Mbyte or 4 Mbyte capacity of data is accumulated. For videos with smaller amounts of data, the videos are saved on an HDD for up to 15 seconds. The video can be played on the client when the data is saved on an HDD.</dd>'+
''+
'        <li>Are previous DVR models supported?</li>'+
'        <dd>Wisenet Viewer is compatible with DVR and NVR products that support SUNAPI version 2.3.1 or above. The previous DVR does not support the SUNAPI protocol, hence they are not compatible with Wisenet Viewer. However, if the previous DVR can support SUNAP 2.3.1 or above through firmware upgrades, then it is compatible with Wisenet Viewer.</dd>'+
''+
'        <li>What information is being processed in Backup and Restore on the Settings Menu?</li>'+
'        <dd>Bookmarks, channels, devices, event schedules, the device group, layout, user, user group, web-page information, event rules, and e-mails are backed up and restored.</dd>'+
''+
'        <li>What is the product ID on the screen of the manual registration device?</li>'+
'        <dd>If you select DDNS/P2P when registering a device, you need to enter the device ID. It is the product ID registered on the DDNS site to establish a remote connection and is used when registering a device with DDNS.</dd>'+
''+
'        <li>I want to fix a slider control while playing the recorded video.</li>'+
'        <dd>You can use the playback speed slider on a timeline when using fast forward. If you want to fix the playback rate, select the speed option of your choice from the combo box next to the playback speed slider.</dd>'+
'    </ul>'+
''+
'</div>';