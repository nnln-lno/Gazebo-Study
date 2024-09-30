Gazebo sim에서 `/world/<world name>/light_config` 서비스를 통해 Scene의 조명을 수정할 수 있다.

### Modifying Lights

Scene 내부에서 조명을 바꾸기 위해, `/world/<world name>/light_config` 서비스를 사용해야 한다. 이 때 `gz::msgs::Light` 메시지를 채워 호출해야함.

기본적으로 Gazebo에서 제공하는 `/gz_ws/src/gz-sim/examples/standalone/light_control/` 폴더 내부에 코드가 작성되어 있다.

코드 일부를 분석해보자.

먼저 해당 파트는, `point light`를 만드는 `CreateFunction()` 함수를 정의하였음.

<strong>[NOTE : ]</strong> Entity를 만다는 튜토리얼은 [여기](https://gazebosim.org/api/sim/8/entity_creation.html)를 참고하자.

```C++
void createLight()
{
  gz::msgs::Boolean rep;
  //! [create light]
  gz::msgs::EntityFactory entityFactoryRequest;

  entityFactoryRequest.mutable_light()->set_name("point");
  entityFactoryRequest.mutable_light()->set_range(4);
  entityFactoryRequest.mutable_light()->set_intensity(1);
  entityFactoryRequest.mutable_light()->set_attenuation_linear(0.5);
  entityFactoryRequest.mutable_light()->set_attenuation_constant(0.2);
  entityFactoryRequest.mutable_light()->set_attenuation_quadratic(0.01);
  entityFactoryRequest.mutable_light()->set_cast_shadows(false);
  entityFactoryRequest.mutable_light()->set_type(gz::msgs::Light::POINT);
  gz::msgs::Set(
    entityFactoryRequest.mutable_light()->mutable_direction(),
    gz::math::Vector3d(directionX, directionY, directionZ));
  gz::msgs::Set(entityFactoryRequest.mutable_light()->mutable_pose(),
    gz::math::Pose3d(0.0, 0, 3.0, 0.0, 0.0, 0.0));
  //! [create light]

  bool executedEntityFactory = node.Request("/world/empty/create",
        entityFactoryRequest, timeout, rep, result);
  if (executedEntityFactory)
  {
    if (result)
      std::cout << "Light was created : [" << rep.data() << "]" << std::endl;
    else
    {
      std::cout << "Service call failed" << std::endl;
      return;
    }
  }
  else
    std::cerr << "Service call timed out" << std::endl;
}

```

해당 부분에서 조명에 대한 speular과 diffuse 색상을 수정할 수 있음

```C++
//! [modify light]
    lightRequest.set_name("point");
    lightRequest.set_range(4);
    lightRequest.set_intensity(1);
    lightRequest.set_attenuation_linear(0.5);
    lightRequest.set_attenuation_constant(0.2);
    lightRequest.set_attenuation_quadratic(0.01);
    lightRequest.set_cast_shadows(false);
    lightRequest.set_type(gz::msgs::Light::POINT);
    // direction field only affects spot / directional lights
    gz::msgs::Set(lightRequest.mutable_direction(),
      gz::math::Vector3d(directionX, directionY, directionZ));
    gz::msgs::Set(lightRequest.mutable_pose(),
      gz::math::Pose3d(0.0, -1.5, 3.0, 0.0, 0.0, 0.0));
    gz::msgs::Set(lightRequest.mutable_diffuse(),
      gz::math::Color(r, g, b, 1));
    gz::msgs::Set(lightRequest.mutable_specular(),
      gz::math::Color(r, g, b, 1));
```

R,G,B 요소들은 해당 `while` 내부에서 지속적으로 랜덤하게 변경된다.

```C++
    while (m < epsilon)
    {
      r = distr(twister);
      g = distr(twister);
      b = distr(twister);
      m = std::sqrt(r*r + b*b + g*g);
    }
    r /= m;
    g /= m;
    b /= m;
```

### Run the Example

해당 예제를 실행하기 위해, 이전에 안내한 경로로 들어가서 아래 커맨드를 실행한다.

```
mkdir build
cd build
cmake ..
make

```

이후 터미널을 2개 실행시켜 각 터미널에서 아래 커맨드를 입력한다.
- 1번 터미널
```
gz sim -r -v 4 empty.sdf
```
- 2번 터미널
```
./light_control
```

