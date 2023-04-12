var device_media_add =  '<div class="item_div_cont" data-content="p040502">'+
'    '+
'    <h3><a class="heading-offset-anchor" id="p040502"></a>Allocating Media Files</h3>'+
''+
'    <p>You can drag video or image files to directly allocate them to a tile. You can also arrange and save layouts.</p>'+
''+
'    <div class="c_bg">'+
'        <h4>Note</h4>'+
'        <p>Here are the media file types that you can check on Wisenet Viewer: Files that are not supported may not work properly.</p>'+
'        <ul>'+
'            <li>Video: mkv, avi, mp4, wisenet media file(wnm), SEC</li>'+
'            <dd class="none-list-ol">'+
'                <ol>'+
'                    <li>Video codec: H264, H265, MJPEG</li>'+
'                    <li>Audio codec: AAC, G.711, G.726</li>'+
'                </ol>'+
'            </dd>'+
''+
'        </ul>'+
'        <ul>'+
'            <li>Image: png, jpeg, jpg, bmp</li>'+
'        </ul>'+
'    </div>'+
''+
'    <h4>Adding Video/Image File via the Resource Tree</h4>'+
'    <p>Drag a video file displayed on the Media File item of the Resource tree to add to a tile.</p>'+
''+
'    <h4>Adding Video/Image File from PC</h4>'+
'    <p>Drag video or image files saved on your PC to add them to a tile.</p>'+
''+
'    <div class="c_bg">'+
'        <h4>Note</h4>'+
'        <ul>'+
'            <li>If you select the <b>Open Media File</b> from the shortcut menu, which appears when right-clicking on an empty tile in the grid, you can add media files to the tile.'+
'                Or, you can press Ctrl + O on the empty tile to add media files.</li>'+
'            <li>If any video file has been assigned to a tile, you can configure the fisheye settings with the shortcut menu. You can enable the fisheye mode and select the type and mount to apply.</li>'+
'            <li>If the tile with an assigned media file ends, it will be deleted from the layout.</li>'+
'        </ul>'+
'    </div>'+
''+
''+
'    <h4>Verifying the digital signature of the WNM video file</h4>'+
'    <p>When saving a WNM video by adding a digital signature, the digital signature of the video can be verified.</p>'+
''+
'    <ol>'+
'        <li>Select a WNM file at the bottom of a media file on the resource tree, and right-click it.</li>'+
'        <li>Select <b>Verifying digital signature</b> in the Shortcut menu.</li>'+
'        <ul class="none-list">'+
'            <li>When verification is complete, <b>Verification has been completed</b> will be displayed in the upper right corner of the screen.</li>'+
'            <li>The icon of the file will be changed after the verification of the digital signature is completed. You can check whether the file has been tampered with by the selected file&apos;s icon.</li>'+
'        <dd class="none-list-ol">'+
'           <ol>'+
'               <li><i class="img_preveal_p_i"><img src="./image/sub/i_158.svg" /></i><b>　　File tampered</b>: An authentication certificate is included since the digital signature function was used when exporting the video. However, the file has been abnormally altered and the authentication information cannot be verified.</li>'+
'               <li><i class="img_preveal_p_i"><img src="./image/sub/i_159.svg" /></i><b>　　Verification complete</b>: An authentication certificate is included since the digital signature function was used when exporting the video, and the authentication information has been verified.</li>'+
'               <li><i class="img_preveal_p_i"><img src="./image/sub/i_160.svg" /></i><b>　　No authentication</b>: The file does not include an authentication certificate since the digital signature function was not used when exporting the video, so it is impossible to determine whether the file has been tampered with.</li>'+
'           </ol>'+
'        </dd>'+
'        </ul>'+
'    </ol>'+
''+
'    <div class="c_bg">'+
'        <h4>Note</h4>'+
'        <p>To save a WNM file with a digital signature, select <b>Enable</b> on the digital signature menu when exporting the video. For more information on exporting videos, refer to <label class="viewbtns" data-id="playback_export" onclick="location.href=\'#p0804\';">Exporting Recorded Videos</label>.</p>'+
'    </div>'+
''+

'</div>';