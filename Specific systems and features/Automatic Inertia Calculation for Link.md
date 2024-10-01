Gazebo에서는 `Monent of Inertia`와 `Center of Mass pose`를 자동으로 계산해준다

현재는 아래의 모델들에 대해서만 가능하다

- Box
- Capsule
- Cylinder
- Ellipsoid
- Sphere
- Mesh

이를 통해 사용자가 정확한 시뮬레이션을 구성할 수 있고, 수동 계산이나 Third-party 메쉬 처리 소프트웨어에 대한 의존성을 제거하므로 초보자들이 접근하기 쉽다.

## Basic

자동으로 계산하기 위해 SDF 파일에서 `<link>`내의 `<inertial>` 태그를 다음과 같이 작성해주면 된다.
```html
<inertial auto="true" />
```
이 경우, 링크의 Collision Geometry 계산이 고려되며 `<density>` 태그를 사용하여 충돌의 질량 밀도 값을 `kg/m^3` 단위로 지정할 수 있다. (기본값은 물의 밀도)
```html
<collision name="collision">
  <density>2710.0</density>
  <geometry>
    <box>
      <size>1 1 1</size>
    </box>
  </geometry>
</collision>
```
링크에 여러 Collision Geometry가 있는 경우, 사용자는 각 링크에 대한 서로 다른 밀도값을 입력할 수 있으며 이를 통한 관성값은 합산되어 최종적으로 계산된다.

하지만 Collision이 없는 경우에는, `ELEMENT_MISSING` 오류가 발생한다.

만약 `auto = true`로 설정하고 사용자가 `<mass>`,`<pose>`,`<inertia>` 태그를 사용했더라도, 자동으로 계산된 값이 덮어쓰게 된다.

아래의 예제를 살펴보자.

```html
<model name="cylinder">
  <link name="cylinder_link">
    <inertial auto="true" />
    <collision name="collision">
      <density>1240.0</density>
      <geometry>
        <cylinder>
          <radius>1</radius>
          <length>2</length>
        </cylinder>
      </geometry>
    </collision>
    <visual name="visual">
      <geometry>
        <cylinder>
          <radius>1</radius>
          <length>2</length>
        </cylinder>
      </geometry>
      <material>
        <diffuse>1.0 1.0 0.0 1.0</diffuse>
        <ambient>1.0 1.0 0.0 1.0</ambient>
        <specular>1.0 1.0 0.0 1.0</specular>
      </material>
    </visual>
  </link>
</model>
```
원기둥을 우클릭하고 View에서 Inertia, Center of Mass 를 시각적으로 확인해보자. 만약 잘 안보인다면 Transparent 적용하기!

## Links with Multiple Collisions & the Effect of Density

아래 예제를 통해 여러 Collision이 있는 링크에서 관성을 계산하고, 서로 다른 밀도가 어떤 효과를 주는지 보자.
```
gz sim auto_inertia_pendulum.sdf
```
본 예제에는 구조적으로 동일한 2개의 모델이 있으며, 각 모델의 진자 링크에는 3개의 원통형 Collision Geometry가 포함되어 있다.

- 상단의 원통은 관절(Pivot)을 형성
- 중간에는 더 긴 원통형 모델
- 끝에는 추 역할을 하는 원통형 모델

비록 두 모델이 동일해도, Collision에 대해 다른 density 값을 사용하기 때문에 질량 중심이 다르다.

왼쪽 진자의 경우, 상단 Joint의 Collision은 가장 높은 밀도를 가지고 있으므로 질량 중심이 축에 가깝지만,

오른쪽 진자의 경우 추가 가장 높은 밀도를 가지고 있으므로 질량 중심이 추에 가까워 진다.

따라서 회전축 주위의 질량 분포 차이는 두 진자의 관성모멘트 차이를 만들고 서로다른 각속도로 회전하게 된다.

## Mesh Inertia Calculation with Rolling Shapes Demo

이번에는 서로다른 Inertia에 따라 구르는 물체들이 어떻게 구르는지 확인해보자.
```
gz sim auto_inertia_rolling_shapes.sdf
```
노란색의 빈 원통은, Gazebo Fuel에서 로드되며 'Collada' Mesh를 사용하여 만들어졌다.

모든 모델은 `<geometry>` 태그를 사용하여 생성되었으며 자동 관성 계산이 활성화되어있다.

## Key Points on Mesh Inertia Calculator

- 관성 계산에는 vertex data가 사용되므로, 이상적인 값을 얻기 위해서는 최대한 많은 vertex data가 필요하다. 하지만 기본적인 모델들(Box, Capsule, Cylinder ..)을 권장하고 있다. 이 경우 전반적인 시뮬레이션 성능, RTF(Real-Time Factor)가 좋아진다.








