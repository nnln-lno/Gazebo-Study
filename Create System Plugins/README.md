# Create System Plugin

Gazebo에서는 모든 시스템이 Runtime에 플러그인으로 로드 된다.

각 시스템은 시뮬레이션의 Entity와 연결되며, 다음과 같은 엔티티 유형에 연결될 수 있다.

- 월드 (World)
- 모델 (Model)
- 센서 (Sensor)
- 액터 (Actor)

## Decide on interfaces to implement

시스템 플러그인을 구현하는 가장 첫단계는, 구현할 인터페이스의 하위 집합을 결정하는 것이다. 기본 시스템 객체 이외에도 네 가지 추가 인터페이스가 있다.

공통적으로 월드 엔티티와 컴포넌트에 대한 읽기-쓰기 접근을 제공함

1. <strong>ISystemConfigure</strong>  
  - 플러그인이 로드되는 순간에 한번 실행된다.
  - SDF 파일에 custom configuration 정보를 가져오고, Event Manager에 이벤트를 등록하고, 엔티티와 컴포넌트를 수정하는 데 사용함.
2. <strong>ISystemPreUpdate</strong> 
  - 시스템이 `gz::sim::UpdateInfo::simTime` 에서 발생하고자 하는 내용을 선언하는 곳
  - Physics 시뮬레이션이 실행되기 전에 state를 수정하는 데 사용. (예. 제어 신호를 적용 혹은 네트워크 동기화룰 수행하는데)
3. <strong>ISystemUpdate</strong>
  - Physics 시뮬레이션 단계에 사용됨 (`gz::sim::UpdateInfo::simTime` 에서 발생하는 내용을 시뮬레이션함)
4. <strong>ISystemPostUpdate</strong>
  - `gz::sim::UpdateInfo::simTime`에서 발생한 모든 내용을 캡처함.
  - 시뮬레이션 단계가 끝날 때, 센서 또는 컨트롤러 업데이트에 사용할 결과를 읽어오는데 사용.
5.  <strong>ISystemReset</strong>
  - 플러그인이 리셋될 때 한번 실행됨

여기서 중요한 점은 `gz::sim::UpdateInfo::simTime`은 현재 시간이 아닌, `PreUpdate`및 `Update` 호출이 완료된 후 도달한 시간을 나타낸다.

따라서 만약 시뮬레이션이 일시 정지된 상태에서 `Update`함수가 호출될 경우, 시간은 진행되지 않고 이는 `PreUpdate`와 `Update` 후에 도달한 시간이 시작 시간과 동일함을 의미함.

때문에, 시뮬레이션이 일시 정지된 상태에서 시작할 경우 `gz::sim::UpdateInfo::simTime`이 처음에 0으로 초기화되는 반면, 

시뮬레이션이 일시 정지를 해제하고 시작하면 `gz::sim::UpdateInfo::simTime`dl cjdmadp `gz::sim::UpdateInfo::dt`로 초기화 된다.

현재 주변 환경에 대한 데이터를 읽기 위해서만 사용되는 시스템(Sensor, Graphic, Rendering 등)은 `ISystemPostUpdate`를 구현해야 한다.

일반적으로 Physics를 관리하지 않는 시스템은 `ISystemUpdate` 인터페이스를 구현 할 필요는 없다.

World state를 기반으로 피드백을 제공하는 컨트롤러 및 시스템은 Update 프레임이 끝날 때, 상태를 읽기 위해 `ISystemPostUpdate`를 구현해야 하며, 다음 프레임의 시작 시 피드백을 제공하기 위해 `ISystemPreUpdate`도 구현애햐 한다.

## Implement Header

헤더 파일에는 `System` 헤더를 사용해야 하며

`System` 객체는 System 객체와 이 객체가 제공하는 모든 인터페이스에서 상속받아야 한다. 그런 다음 상속받은 인터페이스의 해당 메서드를 구현해야 한다.

나의 경우 다음과 같이 작업 폴더를 만들었다

```
mkdir -p ~/gazebo_ws/src/gz-sim/examples/plugin/<plugin_name>
cd ~/gazebo_ws/src/gz-sim/examples/plugin/<plugin_name>
```
이후 아래의 헤더파일을 작성하였다. `<plugin_name>.hpp`
```cpp
#ifndef SYSTEM_PLUGIN_MINHOSYSTEM_HH_
#define SYSTEM_PLUGIN_MINHOSYSTEM_HH_

#include <gz/sim/System.hh>

namespace minho_system{

    class MinhoSystem : 
        // This class is a system.
        public gz::sim::System,

        // This clashs also implements the ISystemPostUpdate interface
        public gz::sim::ISystemPostUpdate{

            public : MinhoSystem();

            public : ~MinhoSystem() override;

            public : void PostUpdate(const gz::sim::UpdateInfo &_info,
                        const gz::sim::EntityComponentManager &_ecm) override;

        };

    class MinhoSystem2 :
        // This class is a system
        public gz::sim::System,
        // This class also implements the ISystemPreUpdate, ISystemUpdate,
        // and ISystemPostUpdate interfaces.
        public gz::sim::ISystemPreUpdate,
        public gz::sim::ISystemUpdate,
        public gz::sim::ISystemPostUpdate,
        public gz::sim::ISystemReset{

            public : MinhoSystem2();

            public : ~MinhoSystem2() override;

            public : void PreUpdate(const gz::sim::UpdateInfo &_info,
                        gz::sim::EntityComponentManager &_ecm) override;

            public : void Update(const gz::sim::UpdateInfo &_info,
                        gz::sim::EntityComponentManager &_ecm) override;

            public : void PostUpdate(const gz::sim::UpdateInfo &_info,
                        gz::sim::EntityComponentManager &_ecm) override;

            public : void Reset(const gz::sim::UpdateInfo &_info,
                        gz::sim::EntityComponentManager &_ecm) override;

        };        
}

#endif
```
위 헤더에는 지금 2개의 시스템 플러그인(`MinhoSystem`,`MinhoSystem2` 을 구현해두었다.)

이렇게 2개 이상의 플러그인 시스템을 구현할 때는 각각에 대한 소스파일을 작성해야한다.

다음으로 헤당 헤더파일에 대한 소스코드를 작성하자. `<plugin_name_1>.cpp`
```cpp
#include "MinhoSystem.hpp"

#include <gz/plugin/Register.hh>

/// GZ_ADD_PLUGIN_ALIAS(PluginClass, "PluginClass")
/// GZ_ADD_PLUGIN_ALIAS(PluginClass, "SomeOtherName", "Yet another name")
/// GZ_ADD_PLUGIN_ALIAS(AnotherPluginClass, "Foo", "Bar", "Baz")

// Include a line in your source file for each interface implemented
GZ_ADD_PLUGIN(
    minho_system::MinhoSystem,
    gz::sim::System,
    minho_system::MinhoSystem::ISystemPostUpdate
)

using namespace minho_system;

MinhoSystem::MinhoSystem()
{

}

MinhoSystem::~MinhoSystem()
{

}

void MinhoSystem::PostUpdate(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm)
{
    gzmsg << "MinhoSystem::PostUpdate" << std::endl;
}
```

또다른 플러그인 시스팀에 대한 소스코드를 작성하자. `<plugin_name_2>.cpp`
```cpp
#include "MinhoSystem.hpp"

#include <gz/plugin/Register.hh>

/// GZ_ADD_PLUGIN_ALIAS(PluginClass, "PluginClass")
/// GZ_ADD_PLUGIN_ALIAS(PluginClass, "SomeOtherName", "Yet another name")
/// GZ_ADD_PLUGIN_ALIAS(AnotherPluginClass, "Foo", "Bar", "Baz")

// Include a line in your source file for each interface implemented
GZ_ADD_PLUGIN(
    minho_system::MinhoSystem2,
    gz::sim::System,
    minho_system::MinhoSystem2::ISystemPostUpdate,
    minho_system::MinhoSystem2::ISystemUpdate,
    minho_system::MinhoSystem2::ISystemPostUpdate,
    minho_system::MinhoSystem2::ISystemReset
)

using namespace minho_system;

MinhoSystem2::MinhoSystem2()
{

}

MinhoSystem2::~MinhoSystem2()
{

}

void MinhoSystem2::PreUpdate(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm)
{
    gzmsg << "MinhoSystem2::PreUpdate" << std::endl;
}

void MinhoSystem2::Update(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm)
{
    gzmsg << "MinhoSystem2::Update" << std::endl;
}

void MinhoSystem2::PostUpdate(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm)
{
    gzmsg << "MinhoSystem2::PostUpdate" << std::endl;
}

void MinhoSystem2::Reset(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm)
{
    gzmsg << "MinhoSystem2::Reset" << std::endl;
}
```
다음으로 빌드를 위한 CMake파일을 작성하자
```
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

find_package(gz-cmake3 REQUIRED)

project(MinhoSystem)

find_package(gz-plugin2 REQUIRED COMPONENTS register)
set(GZ_PLUGIN_VER ${gz-plugin2_VERSION_MAJOR})

find_package(gz-sim8 REQUIRED)
add_library(MinhoSystem SHARED MinhoSystem.cpp MinhoSystem2.cpp)
set_property(TARGET MinhoSystem PROPERTY CXX_STANDARD 17)
target_link_libraries(MinhoSystem
    PRIVATE gz-plugin${GZ_PLUGIN_VER}::gz-plugin${GZ_PLUGIN_VER}
    PRIVATE gz-sim8::gz-sim8)
```
이렇게 전체적인 파일 구조는 다음과 같다.
```
gz-sim/examples/plugin/<plugin_name> 
 |
 |__MinhoSystem.hpp
 |__MinhoSystem.cpp
 |__MinhoSystem2.cpp
 |__CMakeLists.txt
```
아래의 과정을 통해 빌드하자
```
mkdir build
cd build
cmake ..
make
```
이 후, 빌드한 시스템 플러그인 라이브러리를 사용하기 위한 SDF 파일을 만들자.
```html
<?xml version="1.0" ?>
<sdf version="1.10">
    <world name="minho_plugin">
        <physics name="1ms" type="ignored">
            <max_step_size>0.001</max_step_size>
            <real_time_factor>1.0</real_time_factor>
        </physics>
        <plugin
            filename="gz-sim-physics-system"
            name="gz::sim::systems::Physics">
        </plugin>
        <plugin
            filename="gz-sim-user-commands-system"
            name="gz::sim::systems::UserCommands">
        </plugin>
        <plugin
            filename="gz-sim-scene-broadcaster-system"
            name="gz::sim::systems::SceneBroadcaster">
        </plugin>
        <plugin
            filename="MinhoSystem"
            name="minho_system::MinhoSystem">
        </plugin>
        <plugin
            filename="MinhoSystem"
            name="minho_system::MinhoSystem2">
        </plugin>

        <light type="directional" name="sun">
            <cast_shadows>true</cast_shadows>
            <pose>0 0 10 0 0 0</pose>
            <diffuse>0.8 0.8 0.8 1</diffuse>
            <specular>0.2 0.2 0.2 1</specular>
            <attenuation>
                <range>1000</range>
                <constant>0.9</constant>
                <linear>0.01</linear>
                <quadratic>0.001</quadratic>
            </attenuation>
            <direction>-0.5 0.1 -0.9</direction>
        </light>

        <model name="ground_plane">
            <static>true</static>
            <link name="link">
                <collision name="collision">
                <geometry>
                    <plane>
                    <normal>0 0 1</normal>
                    </plane>
                </geometry>
                </collision>
                <visual name="visual">
                <geometry>
                    <plane>
                    <normal>0 0 1</normal>
                    <size>100 100</size>
                    </plane>
                </geometry>
                <material>
                    <ambient>0.9 0.9 0.9 1</ambient>
                    <diffuse>0.8 0.8 0.8 1</diffuse>
                    <specular>0.8 0.8 0.8 1</specular>
                </material>
                </visual>
            </link>
        </model>
    </world>
</sdf>
```
이 후, gz sim 명령어로 SDF 파일을 실행하면 되는데 여기서 오류가 발생한다. (Could not find Shared Library 와 같은..)

이전에 튜토리얼을 보면, 라이브러리 경로를 추가하는 방법이 있었다. 따라서 경로를 추가해주자 (해당 Shared Library인 .so 파일이 있는 빌드 폴더를 경로로..)
```
export GZ_SIM_SYSTEM_PLUGIN_PATH=/home/minho/gazebo_ws/src/gz-sim/examples/plugin/minho_plugin/build
```
