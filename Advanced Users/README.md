Gazebo는 사용자가 원하는 물리엔진(Physics Engine)을 사용하여 시뮬레이션을 진행할 수 있다.

기본적으로 Gazebo는 `DART` 물리엔진을 사용한다.

사용자 혹은 개발자들은 새로운 Gazebo 물리 엔진 플러그인을 생성하여 통합할 수 있으며 자세한 내용은 [여기](https://gazebosim.org/api/physics/7/tutorials.html)를 참고하자.

시뮬레이션에서 사용자가 어떤 물리엔진을 쓸지는 다음 3가지의 방법으로 지정할 수 있다.

## Tell Gazebo What engine to load

### From SDF
```html
<plugin
  filename="gz-sim-physics-system"
  name="gz::sim::systems::Physics">
  <engine>
    <filename>CustomEngine</filename>
  </engine>
</plugin>
```

### From Command Line
```
gz sim --physics-engine CustomEngine
```
혹은, 다른 엔진이 있을 경우
```
gz sim --physics-engine gz-physics-bullet-featherstone-plugin
```

### From C++ API
```Cpp
gz::sim::ServerConfig serverConfig;
serverConfig.SetPhysicsEngine("CustomEngine");
 
gz::sim::Server server(serverConfig);
```

Gazebo Physics 와 관련한 자세한 내용은 아래 두 링크를 참고하자

SDF 관련 : http://sdformat.org/spec?ver=1.8&elem=physics#dart_collision_detector
TroubleShooting 관련 : https://gazebosim.org/api/sim/8/physics.html
