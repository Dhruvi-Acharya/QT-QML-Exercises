1. 소스 다운로드 경로: (NVR팀에 프로젝트 포함시켜달라고 요청 필요)
   git clone –b develop ssh:// ctf.ep.hanwha.com:29418/utility  utility_develop 

2. 옵션 설명
   -DEMUL=yes : 에뮬레이터 모드로 동작
   -DEMUL=no  : 실제 hbird 사용(해당 library있어야함)
   -DBUILD_SHARED_LIBS=on  : 우분투는 so 파일, 윈도우는 dll 파일로 빌드함.

3. CMakeList 파일 경로
    easyConnection/integration/client/proxy/lib.cmake

4. 빌드 
    * lib.cmake 폴더에서 build 폴더 만들어서 이동 (cmake와 make를 하면서 파일이 많이 만들어져 헷갈림.)
    * 에뮬레이터 모드 동작에 따라 -DEMUL 값은 변경해야함

	4.1 우분투
		cmake .. -DCMAKE_BUILD_TYPE=release -DPLATFORM=x86_64 -DP2P=hbird -DEMUL=yes -DBUILD_SHARED_LIBS=on
		make

	4.1 윈도우
                        릴리즈버전 만들기
		cmake .. -DCMAKE_BUILD_TYPE=release -DPLATFORM=win64 -DP2P=hbird -DEMUL=no -DBUILD_SHARED_LIBS=on
		libsiproxy.sln 파일이 생성됨. Visual Studio 2019 로 실행시켜서 릴리즈로 빌드하면 됨.
		디버그버전 만들기
		cmake .. -DCMAKE_BUILD_TYPE=debug -DPLATFORM=win64 -DP2P=hbird -DEMUL=no -DBUILD_SHARED_LIBS=on
		libsiproxy.sln 파일이 생성됨. Visual Studio 2019 로 실행시켜서 디버그로 빌드하면 됨.

	4.3 macOS
		cmake .. -DCMAKE_BUILD_TYPE=release -DPLATFORM=macOS -DP2P=hbird -DEMUL=no -DBUILD_SHARED_LIBS=on
	  	make

5. 주의 사항
	5.1 우분투
		라이브러리 이름이 맞지 않아 빌드 실패함. 아래처럼 CMakeList.txt 내용 변경 필요. (수정요청음 해놓음)
  		151Line, #hbrtc_static -> hbrtc_static
		152Line, hbrtc -> #hbrtc
     		153Line, webrtc -> hbwebrtc
