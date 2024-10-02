3D Policy를 구현하고 Feature를 포함하는 간단한 플러그인을 만들고 빌드해보자.

### Write a simple physics plugin

먼저 개인적으로 작업할 공간을 만들자 (사용자에 따라)
```
cd ~
mkdir -p ~/my_world_plugin/build
cd my_world_plugin
```

C++ 파일을 하나 만들고 구현을 위한 필요한 라이러리들을 Include 하자
```cpp
#include <gz/physics/FeatureList.hh>
#include <gz/physics/FeaturePolicy.hh>
#include <gz/physics/GetEntities.hh>
#include <gz/physics/Register.hh>
```
각 헤더파일이 하는 역할은 다음과 같다.

- <strong>FeatureList.hh</strong> : 플러그인에서 구현할 Feature 목록을 정의함 (예. GetEngineInfo)
- <strong>FeaturePolicy.hh</strong> : Feature가 구현될 좌표계(2차원, 3차원)와 숫자 시스템(Float, int...)에 대한 메타데이터
- <strong>GetEntities.hh</strong> : Gazebo Physics에서 정의해둔 Feature 리스트 들을 가져옴.
- <strong>Register.hh</strong> : Gazebo Physics에 플러그인을 등록하기 위해 사용.

그리고 mock이라는 네임스페이스를 만들고 플러그인이 구현하고 자 하는 모든 Feature들을 나열하자
```cpp
namespace mock{
  // List of all features that this plugin will implement
  struct MyWorldFeatureList : gz::physics::FeatureList<gz::physics::GetEngineInfo>{ };
}
```

해당 플러그인은 물리 엔진에 대한 메타 데이터를 Return 할 수 있다.

위에서 정의한 FeatureList를 사용하여 `MyWorldPlugin` 이라는 클래스를 구현하자.

이 때 클래스는 Implements3d 를 상속받아 플러그인의 `MyWorldFeatureList`가 3D 좌표계에 있다는 것을 선언한다.
```cpp
// The plugin class, which implements a 3D policy
  class MyWorldPlugin
      : public gz::physics::Implements3d<HelloWorldFeatureList>
  {
    using Identity = gz::physics::Identity;
 
    public: Identity InitiateEngine(std::size_t /*_engineID*/) override
    {
      this->engineName = "HelloWorld";
 
      return this->GenerateIdentity(0);
    }
 
    public: std::size_t GetEngineIndex(const Identity &/*_id*/) const override
    {
      return 0;
    }
 
    public: const std::string &GetEngineName(const Identity &/*_id*/) const override
    {
      return this->engineName;
    }
 
    std::string engineName;
  };
```

물리 엔진을 실제로 사용하지 않기 때문에, `InitiateEngine` 멤버 함수 내에, 더미 물리 엔진을 정의하고 그 이름을 "HelloWorld"로 설정했다.

`Identity`를 사용하여, 엔진 객체를 반환한 후에 `GetEngineInfo` 기능에 대해 메타데이터를 가져오는 `GetEngineIndex`와 `GetEingineName`을 정의한다.

마지막으로, 작성한 플러그인을 Gazebo Physics에 물리 엔진으로 등록하기만 하면 된다.
```cpp
  // Register plugin
  GZ_PHYSICS_ADD_PLUGIN(
      MyWorldPlugin,
      gz::physics::FeaturePolicy3d,
      MyWorldFeatureList)
```

빌드를 위해 CMake 파일을 만들고 내용을 다음과 같이 작성한다. 사용자의 파일이름에 맞게 변경해야 하는 부분 주의하자.
```
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(gz-physics-physics-world-plugin)

find_package(gz-plugin2 REQUIRED COMPONENTS all)
set(GZ_PLUGIN_VER ${gz-plugin2_VERSION_MAJOR})

find_package(gz-physics7 REQUIRED)
set(GZ_PHYSICS_VER ${gz-physics7_VERSION_MAJOR})

add_library(PhysicsWorldPlugin SHARED physics_code.cpp)
target_link_libraries(PhysicsWorldPlugin
    PRIVATE
    gz-physics${GZ_PHYSICS_VER}::gz-physics${GZ_PHYSICS_VER}
)
```

### Build and Run

빌드
```
cd ~/my_world_plugin/build
cmake ..
make
```
실행 (이 때, 이전에 연습했던 loader 튜토리얼을 통해 실행하자)
```
cd ~
./hello_world_loader/build/hello_world_loader simple_plugin/build/libHelloWorldPlugin.so
```


