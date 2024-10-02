#### Physics Simulation 의 특징

- Collision Detection(충돌 감지) : ODE, Bullet, 다양한 충돌의 형태와 Mesh
- Kinematics(운동학) : 조인트, 임의의 물체 형태, 변환, 속도, 가속도 등 다양한 운동학적 상태, 역운동학(inverse kinematics)
- Dynamics(동역학) : 조인트 제약, 질량, 중력, 힘, 마찰, 폐쇄루프 구조 등등

Gazebo는 시각적 요소와 동적인 Physics 양상을 설명하기 위해 SDFormat 구조를 사용하고 있음.

SDFormat에 대한 자세한 내용은 [여기](http://sdformat.org/spec?ver=1.7&elem=sdf)에서 찾아보자.

## Physics Concepts in Gazebo Simulation

해당 예제에서는, Gazebo를 사용하여 일부 Physics를 저작하고 시각화하는 방법을 배울 수 있다.

전체 코드 파일은 `gz-sim` 패키지의 `diff_drive.sdf`와 `velocity_control.sdf` 파일을 사용한다.

시뮬레이션은 다음 2가지 중 원하는것으로 실행시키자
```
# then run diff_drive demo with Dartsim
gz sim diff_drive.sdf  # default Dartsim
 
# or run demo with TPE
gz sim velocity_control.sdf --physics-engine gz-physics-tpe-plugin # specify TPE as physics engine
```
커맨드를 통해 파란색 차량에 속도와 각속도 명령을 입력해주자.

참고로 자동차를 움직이는 방식은 사용되는 물리 엔진에 따라 다를 수 있다.

Dartsim의 경우 조인트에 힘을 적용하여 차를 이동시키고, TPE는 모델에 속도를 직접 설정한다.
```
gz topic -t "/model/vehicle_blue/cmd_vel" -m gz.msgs.Twist -p "linear: {x: 1.0}, angular: {z: 0.5}"
```
파란색 차량이 빙글빙글 도는것을 볼 수 있다. 

### Monitor the model and link pose
--- 
Entry Tree 에서 모델 링크를 클릭하여 Pose를 확인해보고 여러가지 값들을 살펴보자.

### Collision Effect
---
Gazebo는 또한 현실적인 충돌 효과를 시뮬레이션 할 수 있다.

파란색 차가 빙글빙글 이동하는 동안, 초록색 차량을 파란색 차량 이동방향에 위치시켜 보자.

파란색 차가 초록색 자동차를 밀어내는 모습을 볼 수 있다.

### Lift Drag

양력과 항력을 시뮬레이션 하기 위해 `lift_drag.sdf` 파일을 실행시켜보자
```
gz sim lift_drag.sdf
```
아래 커맨드를 통해 조인트를 회전시키고 그 결과를 살펴보자
```
gz topic -t "/model/lift_drag_demo_model/joint/rod_1_joint/cmd_force" -m gz.msgs.Double  -p "data: 0.7"
```
반대로 회전을 멈추고 나서, 양력이 줄어들어 큐브가 떨어지는것 또한 볼 수 있다.
```
gz topic -t "/model/lift_drag_demo_model/joint/rod_1_joint/cmd_force" -m gz.msgs.Double  -p "data: 0.0"
```
이 때, 여러가지 시뮬레이션 기능이 작용하고, 양력과 항력은 바람의 압력, 큐브 질량, 중력을 고려하여 계산되며 그 결과로 발생한 힘이 여러 조인트에 가해진다.

이는 Dartsim을 이용하여 구해진다.

### Buoyancy (부력)
---
Gazebo는 부력 또한 시뮬레이션 가능하다. 아래 월드(`buoyancy.sdf`)는 총 3개의 모델로 구성되어 있다.

- submarine : 제자리에 떠있는 간단한 잠수함 모델
- submarine_sinking : 부력이 없는 간단한 잠수함 모델로 가라앉음.
- submarine_buoyant : 부력이 있는 간단한 잠수함 모델로 떠있음.

아래 SDF 파일을 다운받고 실행시켜보자

```
wget https://raw.githubusercontent.com/gazebosim/gz-sim/gz-sim8/examples/worlds/buoyancy.sdf
gz sim buoyancy.sdf
```
이 부력은, 유체 밀도를 시뮬레이션 하고 물체의 부피에 비례하여 유체 내의 물체에 힘을 가하는 방식으로 구현된다.

모델의 부력을 변경하려면 관성(Inertia)을 수정하면 되며, 그 방법은 [여기](http://sdformat.org/spec?ver=1.7&elem=link)를 참고하자.

### Pendulum
---
또한 관성과 중력이 물체의 움직임에 어떤 영향을 주는지 보기 위해 진자(Pendulum)의 흔들림을 살펴보자.
```
gz sim video_record_dbl_pendulum.sdf
```
진자 막대의 관성을 수정하면, 진동 주기나 각속도가 달라진다. 현재 시뮬레이션에서는 중력은 -Z 축으로 `-9.8 m/s^2`으로 정의되어 있다.

### Multicopter

액추에이터 및 관성을 사용하여 물체의 속도를 제어하는 시뮬레이션을 멀티콥터 예제를 통해 살펴보자.
```
gz sim multicopter_velocity_control.sdf
```
멀티콥터를 제어하고 호버링 하기 위해서, 새로운 터미널 창을 킨 후에 커맨드를 입력해주자.
```
gz topic -t "/X3/gazebo/command/twist" -m gz.msgs.Twist -p "linear: {x:0 y: 0 z: 0.1} angular {z: 0}"
```
멀티콥터가 +Z축 방향으로 일정한 속도를 유지하며 비행할 것이다.

만약 호버링을 하고싶다면 현재 위치에서 커맨드를 주지 않으면 된다.
```
gz topic -t "/X3/gazebo/command/twist" -m gz.msgs.Twist -p " "
```

옥타콥터에 대해서도 동일하게 하고싶을 경우에, X3 모델 대신에 X4 모델에 대해 명령을 주면 된다.

해당 시뮬레이션은 물리 엔진이 모델의 운동학과 동역학을 활용하여 복잡한 모델과 제어기를 시뮬레이션 하는 방식을 보여준다.

멀티콥터 제어에 대한 자세한 내용은 [해당 코드](https://github.com/gazebosim/gz-sim/blob/gz-sim8/src/systems/multicopter_control/MulticopterVelocityControl.cc)를 참고하자

