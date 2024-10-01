Physics 플러그인은, 외부 물리 엔진을 Gazebo Physics에 통합하는 역할을 한다. 

사용자는 시뮬레이션 상황에 따라 여러가지 지원되는 물리 엔진 중 선택할 수 있다. 

그렇다면 Gazebo Physics API를 사용하여 컴파일 된 Physics 플러그인을 로드해보자.

---

## Write a Simple Loader

예제 코드는 `gz-physics/examples` 폴더에서 찾을 수 있다.

코드를 살펴보자

```cpp
#include <iostream>

#include <gz/plugin/Loader.hh>
#include <gz/plugin/PluginPtr.hh>

#include <gz/physics/FindFeatures.hh>
#include <gz/physics/GetEntities.hh>
#include <gz/physics/RequestEngine.hh>

// The features that an engine must have to be loaded by this loader.
using Features = gz::physics::FeatureList<
    gz::physics::GetEngineInfo
>;
```
해당 코드의 상단에서는 코드에서 사용할 헤더 파일들을 포함하고 있다.

기본적으로 표준 C++ 라이브러리에서 물리 플러그인을 로드하고, 포인터 주요 기능을 제공하는 `Loader.hh`와 `PluginPtr.hh`가 포함된다.

그리고 `<gz/physics/**>`는 Physics 플러그인에서 Feature와 Entity를 검색하는 역할을 한다.

```cpp
int main(int argc, char **argv)
{
  // User should provide path to plugin library
  if (argc <= 1)
  {
    std::cerr << "Please provide the path to an engine plugin." << std::endl;
    return 1;
  }
 
  std::string pluginPath = argv[1];
 
  gz::plugin::Loader pl;
  auto plugins = pl.LoadLib(pluginPath);
 
  // Look for 3d plugins
  auto pluginNames = gz::physics::FindFeatures3d<Features>::From(pl);
  if (pluginNames.empty())
  {
    std::cerr << "No plugins with required features found in "
              << pluginPath
              << std::endl;
  }
 
  for (const std::string &name : pluginNames)
  {
    std::cout << "Testing plugin: " << name << std::endl;
    gz::plugin::PluginPtr plugin = pl.Instantiate(name);
 
    auto engine = gz::physics::RequestEngine3d<Features>::From(plugin);
 
    std::cout << "  engine name: " << engine->GetName() << std::endl;
  }
}
```
또한, main 함수에서 Loader는 사용자가 로드할 플러그인의 경로를 제공해야 한다.

플러그인의 이름은 `gz::plugin::Loader::LoadLib` 멤버 함수를 통해 찾을 수 있다.

정확한 경로가 있다면, Loader는 `gz::plugin::Loader::Instantiate` 멤버 함수를 통해 경로에 있는 모든 플러그인을 인스턴스화 한다.

그런 다음, 인스턴스화 된 각 플러그인에 대해 `gz::physics::RequestEngine3d<Features>::From`을 사용하여 `FeaturePolicy`(3D의 경우)를 구현하는 엔진을 요청합니다.

### Setup CMakeLists.txt for Build

이제 `CMakeLists.txt` 파일을 생성하고 `gz-plugin`과 `gz-physics` 종속성을 찾기 위한 내용을 추가한다.

이후 파일을 가리키는 실행(executable) 파일을 추가하고 CMake가 컴파일 할 수 있도록 link 라이브러리도 추가하자.

(CMake 에 대한 이해가 필요하다면 https://cmake.org/cmake/help/latest/guide/tutorial/index.html 로 가보자!)

```python
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
 
project(gz-physics-hello-world-loader)
 
find_package(gz-plugin2 REQUIRED COMPONENTS all)
set(GZ_PLUGIN_VER ${gz-plugin2_VERSION_MAJOR})
 
find_package(gz-physics7 REQUIRED)
set(GZ_PHYSICS_VER ${gz-physics7_VERSION_MAJOR})
 
add_executable(hello_world_loader hello_world_loader.cc)
target_link_libraries(hello_world_loader
  gz-plugin${GZ_PLUGIN_VER}::loader
  gz-physics${GZ_PHYSICS_VER}::gz-physics${GZ_PHYSICS_VER})
```
### Compile the loader

이전 작성한 C++ 파일 및 CMake 파일이 있는 경로로 들어가서 다음의 과정을 따라하자.
``
cd build
cmake ..
make
``
### Load existing plugins

만약, DART용 Gazebo Physics 플러그인이 컴파일되어 설치된 경우, 다음 명령어를 통해 설치된 위치를 찾자.
```
find / | grep libgz-physics-dartsim-plugin.so
```

찾은 경로를 통해, 해당 파일을 load 하자.
```
./hello_world_loader <path_to>/libgz-physics-dartsim-plugin.so
```

다음과 같은 결과를 얻을 수 있다.
```
Testing plugin: gz::physics::dartsim::Plugin
  engine name: dartsim-6.13.2
```




