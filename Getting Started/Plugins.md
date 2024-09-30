Gazebo를 사용하여 시뮬레이션을 구성하고 실행할 때, 그 안에는 로봇 모델, 플러그인 등과 같은 다양한 종류의 리소스들이 포함되어 있다.

또한 이러한 리소스에는 우리 컴퓨터에 있는 파일들이나, 혹은 온라인 서버상에 존재하는 파일이 포함되기도 한다.

Gazebo에서 요구되는 리소스를 사용하기 위한 방법을 알아보자.

## Plugins

플러그인은, 특정 API를 준수하고 런타임에 로드되는 "Shared Library" 이다. 일반적으로 플러그인은 여러가지의 특정 기능을 수행하도록 설계 된다.

예를 들어, Gazebo에서 제공하는 `diff_drive` 플러그인은 모바일 로봇을 위한 차동 구동 컨트롤러(Differential Drive Controller)를 구현함.

### System Plugins

"시스템 플러그인"은 Gazebo에서 사용되며, 시뮬레이션의 맞춤화 및 제어를 위한 Entry Point를 제공. (디테일한 튜토리얼은 [여기](https://gazebosim.org/api/sim/8/createsystemplugins.html)를 참고) 

이러한 시스템 플러그인은 다음을 통해 로드(Load)될 수 있다.

- SDF 파일의 태그에서, `filename`은 공유 라이브러리 이름이고, `name`은 로드될 클래스이다. 이는 다양한 엔티티에 연결될 수 있다.
    - `<world>`, `<model>`, `<sensor>` 태그 내에 `<plugin>` 태그로 연결될 수 있음.

```
<plugin filename="MinimalScene" name="3D View"></plugin>
```

- 서버를 인스턴스화 할 때, `server.config` 내에 있는 plugin을 통해 로드될 공유 라이브러리와 클래스를 전달.

Gazebo가 시스템 플러그인을 찾는 순서는 다음과 같다.

1. `GZ_SIM_SYSTEM_PLUGIN_PATH` 환경 변수에 있는 모든 경로에서
   * Source build 로 패키지를 받은 경우, `/gz_ws/build/gz-sim<#>/lib`으로 지정되어 있는 것 같음
2. `$HOME/.gz/sim/plugins`
3. Gazebo와 함께 설치된 시스템 경로

### Gazebo GUI Plugins

각각의 Gazebo GUI 플러그인은 위젯을 정의함. 이러한 GUI 플러그인은 다음을 통해 로드된다.

- `filename`이 shared library인 SDF 파일내의 태그로 부터

```
<world><gui><plugin></plugin></gui></world>
```

- `filename`이 shared library인 [GUI Config files](https://gazebosim.org/api/gui/8/config.html) 파일내의 태그로 부터

```
<plugin></plugin>
```

- Gazebo 시뮬레이터 스크린의 우측 상단에 있는 메뉴로부터

Gazebo가 GUI 플러그인을 찾는 순서는 다음과 같다.

1. `GZ_GUI_PLUGIN_PATH` 환경 변수에 있는 모든 경로에서
    * Source build 로 패키지를 받은 경우, `/gz_ws/build/gz-gui<#>/lib`으로 지정되어 있는 것 같ㅇ므.   
2. Gazebo와 함께 설치된 GUI 플러그인 경로
3. `gz::gui::App() -> AddPluginPath`로 호출된 추가적인 경로
4. `~/.gz/gui/plugins`
5. Gazebo GUI와 함께 설치된 플러그인 경로

### Physics Engines

Gazebo Physics는 플러그인 아키텍처를 사용하며, 물리 엔진은 Gazebo 플러그인을 사용하여 런타임에 로드되는 플러그인으로 만들어짐. (디테일한 튜토리얼은 [여기](https://gazebosim.org/api/sim/8/physics.html)를 참고)

### Rendering Engines

Gazebo Rendering는 플러그인 아키텍처를 사용하며, 렌더링 엔진은 Gazebo 플러그인을 사용하여 런타임에 로드되는 플러그인으로 만들어짐.

현재, Gazebo `Rendering`은 `<install_prefix>/lib` 디렉토리 (나의 경우 `/gz_ws/install/lib`)에 설치된 렌더링 엔진 플러그인의 Shared Library 만을 찾으며

이 엔진들이 사용하는 리소스 또한 Gazebo Rendering의 `<install_prefix>/share` 디렉토리에 위치하여 있음.

### Sensors

Gazebo Sensors의 각 고유한 센서는 플러그인이며, 특정 센서 유형이 요청되면 관련 플러그인이 Gazebo Plugin에 의해 로드됨과 동시에 센서 객체가 인스턴스화 된다.

현재, Gazebo `Sensors`은 `<install_prefix>/lib` 디렉토리 (나의 경우 `/gz_ws/install/lib`)에 설치된 센서 플러그인의 Shared Library 만을 찾음.

## Models, Lights, Actors

가장 상위 레벨의 엔티티인 모델(Model), 조명(Lights) 그리고 액터(Actors) 등은 다음을 통해 로드된다.

1. SDF 파일 내의 태그
   
```
<world><model>
<world><light>
<world><actor>
<include><uri>(URL or PATH)
```

2. `/world/<world_name>/create` 서비스를 호출하기
3. 시스템 내에서, SdfEntityCreator를 사용하거나 직접 컴포넌트와 엔티티를 생성하여 사용
   
이때 Gazebo는 `(URL or PATH)`를 다음의 순서로 찾아냄

1. `GZ_SIM_RESOURCE_PATH` 환경 변수에 설정된 모든 경로
2. 현재 경로 혹은 절대 경로
3. [Gazebo Fuel](https://app.gazebosim.org/fuel/models)
   - 캐시를 통해 ($HOME/.gz/fuel)
   - 웹 서버를 통해
* `SDF_PATH` 환경 변수도 일부 상황에서는 동작하지만 권장하고 있지는 않음.

## Meshes

메쉬 파일들은 다음을 통해 로드된다.

1. SDF 파일 내의 태그
   
```
<geometry><mesh><uri>
<actor><skin><filename>
<actor><animation><filename>
```

이때 Gazebo는 `(URL or PATH)`를 다음의 순서로 찾아냄

1. 현재 경로 혹은 절대 경로
2. `GZ_SIM_RESOURCE_PATH` 환경 변수에 설정된 모든 경로
* `GZ_FILE_PATH` 환경 변수도 일부 상황에서는 동작하지만 권장하고 있지는 않음.

만약 `<geometry><mesh><uri>`가 (예:`name://my_mesh`) 으로 시작할 경우에, Gazebo는 해당 이름을 가진 Mesh가 `Mesh Manager`에 존재하는지 확인하고

존재할 경우 해당 Mesh를 로드. 이는 메모리에서 `common::Mesh` 객체가 생성되고 `common::MeshManager::Instance()->AddMesh` 호출을 통해 `Mesh Manager`에 등록될 때 발생함.

## GUI Configuration

Gazebo Sim 의 GUI 구성은 다음의 순서를 따르고 있다.

1. Command Line의 옵션인 `--gui-config <file_path>`를 사용
2. SDF 파일 내의 `<world><gui>`를 사용
3. `$HOME/.gz/sim/<#>/gui.config`를 로드하여 사용



