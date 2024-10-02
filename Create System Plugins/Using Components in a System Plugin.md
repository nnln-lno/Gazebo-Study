Gazebo는 엔티티 컴포넌트 시스템(ECS : Entity Component System) 소프트웨어 아키텍처를 사용한다.

사용되는 엔티티(Entity), 컴포넌트(Component), 시스템(System), 엔티티 컴포넌트 매니저(ECM : Entity Component Manager)에 대한 용어는 [여기](https://gazebosim.org/api/sim/8/terminology.html)에서 확인할 수 있다.

간단히 말해, 시뮬레이션 세계는 여러개의 엔티티로 구성되고 각각은 여러 컴포넌트와 연관이 있다.

시스템 플러그인은 컴포넌트를 조작하여 시뮬레이션 세계를 수정할 수 있다. 기본 구조는 해당 폴더 README에 설명하였다.

이번에는 시스템이 컴포넌트를 사용하여 어떻게 세계를 수정할 수 있는지 알아본다. `gz::sim::components` 목록은 [여기](https://gazebosim.org/api/sim/8/namespacegz_1_1sim_1_1components.html)에서 확인하자.

프로그래밍에서 컴포넌트의 사용 예시는, 내부 시스템이나 통한된 소스코드 테스트에서 찾을 수 있다. 대부분의 내부 시스템은 해당하는 예제 SDF가 있고, Gazebo가 기본으로 제공하는 World는 [여기](https://github.com/gazebosim/gz-sim/tree/gz-sim8/examples/worlds)에 있다.

참고
---

해당 내용을 참고하면 이번 예제를 이해하는데 도움이 될 수 있다. 5개 중, 아래 3개는 Gazebo가 제공하는 예제에 대한 코드를 의미한다.

- `gz::sim::components` [API](https://gazebosim.org/api/sim/8/namespacegz_1_1sim_1_1components.html)
- `gz::sim::systems` [API](https://gazebosim.org/api/sim/8/namespacegz_1_1sim_1_1components.html)
- Built-in System 의 [소스코드](https://github.com/gazebosim/gz-sim/tree/gz-sim8/src/systems)
- Example World 의 [소스코드](https://github.com/gazebosim/gz-sim/tree/gz-sim8/examples/worlds)
- Integration Test 의 [소스코드](https://github.com/gazebosim/gz-sim/tree/gz-sim8/test/integration)

#### ECM : Entity Component Manager

엔티티와 상호작용하는 게이트웨이는 `gz::sim::EntityComponentManager`이다. ECM을 통해 모든 엔티티에 접근할 수 있고, 각각의 엔티티에 연관된 컴포넌트에 접근할 수 있다.

ECM 객체는 시스템의 모든 인터페이스(`ISystemConfigure()`,`ISystem[Pre,Post, ]Update()`)에 전달된다.

해당 인터페이스는 `gz/sim/System.hh`에 정의되어 있고 이전 튜토리얼(README.md)에서 설명했다.

본 예제에서는 시스템에서 해당 인터페이스가 `Configure()` 및 `**Update()` 함수로 구현된다고 가정한다.

`Configure()`가 호출될 때, 플러그인의 부모 요소 내 모든 요소가 로드 된 상태이다.

예를 들어 `<model>`에 연결된 경우, 그 `<model>` 내의 모든 요소가 로드된다. 이는 비슷하게 `<world>`에도 적용된다.

하지만 플러그인의 부모 요소 외부 엔티티가 `Configure()`호출에도 로드되지 않았을 때, 그렇지만 접근해야 할 경우, `**Update()` 에서 해당 엔티티에 접근해야 할 수도 있다.

참고로 Configure() 함수의 설명은 이렇게 되어 있다 "Configure는 시스템이 인스턴스화되고 모든 엔티티와 구성 요소가 해당 SDF 월드에서 로드된 후, 시뮬레이션이 실행되기 전에 호출됩니다.". 

크게 2가지의 예제로 살펴보자

## JointForceCmd

본 예제에서는 `gz::sim::components::JointForceCmd` 컴포넌트를 사용하는 방법을 알아본다. 이 컴포넌트를 통해 조인트에 힘 명령을 설정할 수 있다.

전체 코드를 살펴보고 싶다면, [Github](https://github.com/gazebosim/gz-sim/tree/gz-sim8/src/systems/apply_joint_force)에서 참고할 수 있도록 하자.

SDF 파일 또한 위 링크에서 제공하고 있다.

나같은 경우 추후 개발을 위해 Source build 형태의 패키지에서 어떻게 시스템 플러그인을 빌드하는지 정리해보았다.

먼저 시스템 플러그인은 다음 폴더 내부에 정의되어 있다.
```
gazebo_ws/src/gz-sim/src/systems
```
홈페이지에서 알려주는 예제는 `apply_joint_force`로, 기존에 작성된 코드로 부터 설명되지만 나만의 시스템 플러그인 빌드를 위해 `minho_joint_force`라는 시스템 플러그인을 만들었다.

물론 코드는 사실 클래스 이름을 변경해주고 하는 것이 다지만, 그럼에도 공부가 될것 같다.

시스템 플러그인 폴더 내 구조는 아래와 같다.
```
gazebo_ws/src/gz-sim/src/systems/minho_joint_force
 |__MinhoApplyJointForce.cpp
 |__MinhoApplyJointForce.hpp
 |__CMakeLists.txt
```
코드는 Github 해당 폴더 내에 저장해두었으며 몇가지 주석 설명도 붙여두었다. (예제와 다른부분이 몇개 있어서 수정한 부분도 있다)

build는 폴더를 만들고 하던 과정과는 다르게, 패키지를 빌드하듯이 하면 된다. (`colcon build --merge-install`)

SDF 파일을 보면 알듯이, 내가 만든 `gz::sim::v8::systems::MyApplyJointForce` 플러그인을 사용한다.

* 그런데 기존에 사용하던 플러그인 형태인 `gz::sim::systems::MyApplyJointForce`로 만들어 졌는지는 의문이다.
