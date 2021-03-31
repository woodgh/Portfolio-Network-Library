# Project-NetPlay

![image](https://user-images.githubusercontent.com/8158795/110476839-cabba400-8125-11eb-9b0b-4d1c44b15d72.png)

멀티 플랫폼에서 사용할 목적으로 제작한 C++ 기반 네트워크 라이브러리입니다. 

### 지원하는 플랫폼

![Windows](https://img.shields.io/static/v1?label=windows&message=10&color=brightgreen)&nbsp;&nbsp;&nbsp;
![Ubuntu](https://img.shields.io/static/v1?label=ubuntu&message=18.04.5_LTS&color=brightgreen)

# 성능 테스트 - EchoTest

과거에 게임코디([바로가기](http://1st.gamecodi.com/board/zboard.php?id=GAMECODI_Talkdev&page=3&page_num=40&select_arrange=hit&desc=&sn=off&ss=on&sc=on&keyword=&category=&no=872)) 에서 소개된 CGCII 엔진의 EchoTest 성능을 기준으로 제작 되었습니다.

![image](https://user-images.githubusercontent.com/8158795/110477123-21c17900-8126-11eb-9848-f1a796366c03.png)

1. 100개 클라이언트 접속
2. 초당 8바이트 패킷을 20,000,000번 송수신

- 윈도우 빌드 시연 동영상 ([바로가기](https://www.youtube.com/watch?v=GfNdWTIxvSs))
- 리눅스 빌드 시연 동영상 ([바로가기](https://www.youtube.com/watch?v=lVzvF_djYkU))


# 성능 테스트 - DummyTest

10 x 10 크기(한 섹터당 5040 크기)의 월드에서 수천개의 클라이언트를 접속 테스트를 목적으로 제작 되었습니다.

서버는 윈도우/리눅스 기반으로 실행하며, 클라이언트는 윈도우 기반으로 빌드됩니다.

언리얼4 기반 테스트 클라이언트 접속을 지원합니다. ([바로가기](https://github.com/woodgh/Portfolio-GameClient))

> 클라이언트 빌드를 위해 wxWidgets 설치가 필요합니다. (vcpkg를 사용할 경우)
> 
> (https://www.wxwidgets.org/blog/2019/01/wxwidgets-and-vcpkg/)
> 1. git clone https://github.com/microsoft/vcpkg.git
> 2. bootstrap-vcpkg.bat
> 3. vcpkg install wxwidgets
> 4. vcpkg integrate install

### 클라이언트 실행 옵션

```shell
DummyClient.exe --clients=클라이언트갯수
```

![image](https://user-images.githubusercontent.com/8158795/112939410-63ae6f80-9166-11eb-9380-4ef0666c3ffa.png)

- 윈도우 빌드 시연 동영상 ([바로가기](https://www.youtube.com/watch?v=KnV-PIh-gkU))
- 리눅스 빌드 시연 동영상 ([바로가기](https://www.youtube.com/watch?v=5rRfWRcgYBA))

### Unreal4 클라이언트로 접속 가능하며 이동/채팅을 할 수 있습니다.
- 언리얼4 시연 동영상 ([바로가기](https://www.youtube.com/watch?v=eIysjKEEAmg))
