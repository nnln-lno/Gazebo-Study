## Implement a custom feature in DART Plugin

이전에 "Feature"를 구현하는 튜토리얼에서, 가상의 더미 물리엔진("HelloWorld")을 구현하고 이를 Gazebo-Physics API를 사용하여 로드하는 방법을 배웠다.

이번에는 `gz-physics` 저장소에 있는 DART 물리 엔진과 같은 플러그인의 구조를 조금 더 살펴보고, 플러그인 내 사용자 정의 Feature를 정의하는 방법을 알아보자

* Feature 은 [홈페이지 API](https://gazebosim.org/api/physics/7/classgz_1_1physics_1_1Feature.html)를 보면 Engine, Joint, Link, Model 등을 얘기하는 것 같다.
* Policy 또한 [홈페이지 API](https://gazebosim.org/api/physics/7/namespacegz_1_1physics.html#a6d38e59cc9ee7d1edeb9af2205aac83d)를 보면 사용하는 데이터 타입을 정의해둔 것 같다.

#### Plugin Folder Structure

아래 구조는 `gz-physics` 레포를 설명하고 있다.

```
gz-physics
├── dartsim                   Files for dartsim plugin component.
├── tpe                       Files for tpe plugin component.
├── heightmap                 Files for heightmap component.
├── include/gz/physics  Header files.
├── mesh                      Files for mesh component.
├── resources                 Model and mesh resource files used by tests.
├── sdf                       Files for sdf component.
├── src                       Source files and unit tests.
├── test
├── tutorials                 Tutorials, written in markdown.
├── Changelog.md              Changelog.
└── CMakeLists.txt            CMake build script.
```
보는것과 같이, 2가지 물리엔진이 사용가능하다.

- DART : gz-physics-dartsim-plugin
- TPE : gz-physics-tpe-plugin

이 두 물리엔진 폴더는 gz-physics 상위 폴더 바로 아래에 있다. (위 구조 참고)

예시로 DART 물리엔진 폴더를 들어가보면 아래와 같이 구조가 정리되어 있다.
```
dartsim
├── worlds                            Example SDF files for testing dartsim plugin functionalities.
├── src                               Main implementation files of the plugin features interfacing the physics engines API
├── include/gz/physics/dartsim  Header files for the plugin features.
└── CMakeLists.txt                    CMake plugin build script.
```

기본적으로 물리 엔진의 기능에 해당하는 `Feature` 혹은 `FeatureList`(Feature들의 묶음)를 새로 구현하는 것은

`include/gz/physics/<plugin_name>` 폴더에 헤더파일 형태로 정의할 수 있다. (기본적으로 include 폴더가 없다)

사용자 정의 `Feature`는 `FeatureList`에 추가되고 src 폴더에서 그 기능을 구현할 수 있다.

#### Plugin and Feature Requirements

일반적으로, Gazebo에서 특정 물리 엔진 플러그인을 구현하기 위해 필요한 최소한의 Feature 리스트는 아래와 같다.

- FindFreeGroupFeature
- SetFreeGroupWorldPose
- FreeGroupFrameSemantics
- LinkFrameSemantics
- ForwardStep
- RemoveEntities
- ConstructSdfLink
- ConstructSdfModel
- ConstructSdfWorld

해당 목록은 물리 엔진 플로그인의 시뮬레이션 기능에 대한 최소 요구 사항을 정의하고 있으며, 호환성을 유지한다.

사용자 정의 Feature의 경우, 주요 2가지의 구성 클래스들이 있다.

- Entity는 구현된 Feature에 해당하는 "Proxy Object" 이며, Entity 클래스에서 상속된 가장 일반적인 "Proxy Object"는 다음과 같다.
  - Engine : Engine API에 대한 클래스, 해당 클래스는 물리 엔진에 대한 메타데이터를 제공한다. (예. GetEngineInfo feature). <strong>모든 Engine feature는 반드시 해당 클래스를 상속해야함</strong>
  - Joint : 물리 개념인 Joint 동작을 정의함 (예. GetBasicJointState Feature)
  - Link : 물리 개념인 Link 구조를 정의함.
  - Model : 물리 개념인 Model 구조를 정의함 (예. Link와 Model 객체를 포함한 GetLinkFromModel Feature).
  - Shape : 물리 개념인 Shape 구조를 정의함 (예. GetShapeKinematicProperties Feature).
  - World : 물리 개념인 Shape 구조를 정의함 (예. Dartsim 플러그인 에서의 RetrieveWorld Feature).

이러한 객체 클래스는 서로 베타적이지 않고, Feature를 설명하기 위해 함께 정의될 수 있다. 기능의 구현에 따라 FreeGroup 객체와 같은 비정형 객체도 정의될 수 있음.

[물리 개념에 대한 자세한 내용은 이전의 시뮬레이션 [튜토리얼](https://gazebosim.org/api/physics/7/physicsconcepts.html)을 참고하자]

Implementation의 경우, 사용자 정의 기능에 대한 실제 물리 엔진 API를 인터페이스 한다. 해당 클래스에는 필수 Feature를 제공하기 위해 물리 엔진 초기화를 트리거하는 `InitiateEngine` 메서드가 있음.

또한 사용자 정의 Feature 간 의존성을 정의할 수 있따.

- 기본적으로, Blank Feature은 다른 Feature를 요구하지 않는다. 만약 사용자 정의 Feature가 다른 Feature 집합을 요구하는 경우 `FeatureWithRequirements` 클래스를 상속받아야 하며 필요한 Feature 목록을 제공해야함.
- 또한 Blank Feature는 다른 Feature와 충돌하지 않는다. 만약 충돌하는 경우 `FeatureWithConflicts` 클래스를 상속받아야 하며, 충돌하는 Feature 목록을 제공해야 한다. 충돌하는 Feature는 요청 시 동시에 실행될 수 없음.
