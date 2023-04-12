var playback_timeline_layout =  '<div class="item_div_cont" data-content="p080201">'+
''+
'    <h3><a class="heading-offset-anchor" id="p080201"></a>타임라인 알아보기</h3>'+
'    <p>타일을 선택하면 타임라인에 구간 표시 막대가 표시됩니다. 구간 표시 막대는 녹화 영상이 있는 구간에만 표시됩니다. 타임라인의 기본 시간 표시 범위는 1일(24시간) 이며, 마우스 휠로 시간 표시 배율을 확대하거나 축소할 수 있습니다. 원하는 시간을 클릭하면 해당 시점의 녹화 영상이 재생됩니다.</p>'+
''+
'    <span class="img_preveal_p"><img src="./image/sub/114.png" /></span>'+
''+
'    <li><span class="main_color on_num">➊</span> <i class="img_preveal_p_i2"><img src="./image/sub/i165.svg" /></i></li>'+
'    <dd>현재 재생중인 영상의 썸네일을 표시하거나 숨깁니다. 타임라인 표시 범위를 균등하게 분할하여, 각 분할 구간의 첫 번째 시간 썸네일을 타임라인에 표시합니다.</dd>'+
''+
'    <li><span class="main_color on_num">➋</span> <i class="img_preveal_p_i2"><img src="./image/sub/i167.svg" /></i></li>'+
'    <dd>멀티 타임라인 또는 싱글 타임라인으로 전환합니다. 멀티 타임라인을 선택하면 현재 레이아웃에 포함된 전체 채널의 타임라인이 표시됩니다. 멀티 타임라인의 경우 1일 동안의 타임라인만 확인할 수 있습니다.</dd>'+
''+
'    <li><span class="main_color on_num">➌</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_2.svg" /></i></li>'+
'    <dd>저장 타입 필터 선택창이 나타나면 검색할 저장 타입을 선택합니다. 자세한 정보는 <label class="viewbtns" data-id="playback_search_event" onclick="location.href=\'#p080302\';">저장 타입으로 검색하기</label>를 참고하십시오.</dd>'+
''+
'    <li><span class="main_color on_num">➍</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_1.svg" /></i></li>'+
'    <dd>달력 창이 나타나면 영상을 검색할 날짜를 선택합니다. 자세한 정보는 <label class="viewbtns" data-id="playback_search_date" onclick="location.href=\'#p080301\';">날짜로 검색하기</label>를 참고하십시오.</dd>'+
''+
'    <li><span class="main_color on_num">➎</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_4.png" /></i></li>'+
'    <dd>현재 재생중인 영상의 날짜와 시간을 표시합니다. 날짜를 클릭하면 달력 창에서 검색할 날짜를 선택할 수 있으며, 시간을 클릭하면 검색 시간을 직접 입력할 수 있습니다.</dd>'+
''+
'    <li><span class="main_color on_num">➏</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_5.png" /></i></li>'+
'    <dd>영상의 재생, 재생 속도를 제어합니다. 자세한 정보는 <label class="viewbtns" data-id="playback_timeline_control" onclick="location.href=\'#p08020201\';">재생 기능 사용하기</label>를 참고하십시오.</dd>'+
''+
'    <li><span class="main_color on_num">➐</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_3.png" /></i></li>'+
'    <dd>현재 선택된 채널 이름을 표시합니다.</dd>'+
''+
'    <li><span class="main_color on_num">➑</span> <i class="img_preveal_p_i2"><img src="./image/sub/i166.svg" /></i></li>'+
'    <dd>여러 개의 재생 영상을 같은 시간으로 동기화하여 재생하거나 동기화 재생을 해제할 수 있습니다.</dd>'+
''+
'    <li><span class="main_color on_num">➒</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_6.png" /></i></li>'+
'    <dd>라이브 모드 또는 재생 모드로 전환합니다. 동기화 재생 중에는 레이아웃에 포함된 전체 채널이 라이브 또는 재생 모드로 전환됩니다.</dd>'+
''+
'    <li><span class="main_color on_num">➓</span> <i class="img_preveal_p_i2"><img src="./image/sub/114_7.png" /></i></li>'+
'    <dd>녹화 영상이 있으면 구간 표시 막대가 표시됩니다. 마우스로 타임라인의 특정 지점을 선택하면 해당 지점부터 영상이 재생되며 키프레임이 나타납니다. 키프레임을 드래그하면 재생 위치를 변경할 수 있습니다.</dd>'+
''+
'    <div class="c_bg">'+
'        <h4>참고</h4>'+
''+
'        <ul>'+
'            <li>녹화 데이터의 종류에 따라 색상이 다르게 표시됩니다.<br>일반 녹화 영상은 연두색으로, 이벤트 녹화 영상은 빨간색으로 구분됩니다.</li>'+
'            <li>타임라인 하단의 슬라이더를 선택해 시간을 이동할 수 있습니다. 또는 타임라인 위에서 마우스 휠을 사용하면 시간 표시 배율을 확대하거나 축소할 수 있습니다.</li>'+
'            <li>북마크가 설정되어 있는 경우 타임라인의 해당 지점에 북마크 아이콘이 표시됩니다. 북마크 아이콘을 선택하면 설정된 구간이 주황색으로 표시되며, 북마크 탭에서 해당 북마크를 확인할 수 있습니다. 북마크 사용에 대한 자세한 정보는 <label class="viewbtns" data-id="playback_bookmark" onclick="location.href=\'#p0805\';">북마크 사용하기</label>를 참고하십시오.</li>'+
'        </ul>'+
'    </div>'+
''+
'    <h4>타임라인 바로 가기 메뉴 사용하기</h4>'+
'    <p>영상 재생 중 타임라인 위에서 마우스 오른쪽 버튼을 클릭하면 바로 가기 메뉴를 사용할 수 있습니다.</p>'+
''+
'    <span class="img_preveal_p"><img src="./image/sub/115.png" /></span>'+
''+
'    <ul>'+
'        <li>중복 구간</li>'+
'        <dd>장치의 시간이나 시간대를 변경하면 동일한 시간에 녹화 영상이 중복되어 저장됩니다. 중복 영상이 있는 경우, 타임라인에 모자이크로 표시되며 중복 리스트에서 선택하여 재생할 수 있습니다.</dd>'+
''+
'        <li>시작점으로 표시 / 끝점으로 표시</li>'+
'        <dd>'+
'            타임라인에서 원하는 구간의 시작점과 종료점을 선택합니다.<br>'+
'            Ctrl키를 누른 후 원하는 지점에서 마우스를 클릭하면 구간의 시작점과 종료점을 설정할 수 있습니다. 구간 설정에 대한 자세한 정보는 <label class="viewbtns" data-id="playback_timeline_section" onclick="location.href=\'#p080203\';">구간 선택하기</label>를 참고하십시오.'+
'        </dd>'+
''+
'        <li>지우기</li>'+
'        <dd>타임라인에서 선택한 구간 박스를 해제할 수 있습니다.</dd>'+
''+
'        <li>확대</li>'+
'        <dd>타임라인에서 선택한 구간을 확대하여 볼 수 있습니다.</dd>'+
''+
'        <li>북마크 추가</li>'+
'        <dd>타임라인에서 원하는 구간을 선택하고 북마크를 추가할 수 있습니다. 북마크 설정에 대한 자세한 정보는 <label class="viewbtns" data-id="playback_bookmark_add" onclick="location.href=\'#p080501\';">북마크 추가하기</label>를 참고하십시오.</dd>'+
''+
'        <li>영상 내보내기</li>'+
'        <dd>타임라인에서 원하는 구간을 선택하고 PC에 동영상 파일로 저장할 수 있습니다. 영상 내보내기에 대한 자세한 정보는 <label class="viewbtns" data-id="playback_export" onclick="location.href=\'#p0804\';">녹화 영상 내보내기</label>를 참고하십시오.</dd>'+
''+
'        <li>이벤트 검색</li>'+
'        <dd>해당 채널의 이벤트를 검색할 수 있습니다. 이벤트 검색에 대한 자세한 정보는 <label class="viewbtns" data-id="search_event" onclick="location.href=\'#p0704\';">이벤트 녹화 영상 검색하기</label>를 참고하십시오. </dd>'+
''+
'        <li>이벤트 로그</li>'+
'        <dd>해당 채널의 이벤트 로그를 검색할 수 있습니다. 이벤트 로그 검색에 대한 자세한 정보는 <label class="viewbtns" data-id="log_event" onclick="location.href=\'#p0901\';">이벤트 로그 확인하기</label>를 참고하십시오.</dd>'+
'    </ul>'+
''+
'    <div class="c_bg">'+
'        <h4>참고</h4>'+
'        <p>구간을 선택하면 바로 가기 메뉴에 구간 관련 메뉴가 추가되어 나타납니다. 구간 선택 후 사용할 수 있는 바로 가기 메뉴에 대한 자세한 정보는 <label class="viewbtns" data-id="playback_timeline_section" onclick="location.href=\'#p080203\';">구간 선택하기</label>를 참고하십시오.</p>'+
'    </div>'+
''+
'</div>';