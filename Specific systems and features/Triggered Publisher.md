TriggerPublisher 시스템은, 사용자가 지정한 어떤 기준에 맞는 입력 메시지에 반응하여 지정한 메시지를 토픽으로 Publish 함.

또한 입력 메시지에 반응하여 사용자가 지정한 서비스를 호출할 수도 있음.

이 시스템은 입력 메시지를 여러 Matcher(매처)와 비교하여 동작하는데, 매처는 Protobuf 메시지의 문자열 표현을 포함하여, 입력 메시지와의 일치 또는 포함 여부를 비교함.

매처는 입력 메시지 전체를 일치시킬 수도 있고, 메시지 내부의 특정 필드만을 일치시킬 수도 있음.

## How Trigged Publisher works?? 

`diff_drive.sdf` 예제를 사용해서 알아보자. (전체 코드가 필요하다면 [여기](https://github.com/gazebosim/gz-sim/blob/gz-sim8/examples/worlds/triggered_publisher.sdf)에서 다운받자)

예제는 차량의 움직임에 반응하여 Detachable Joint를 분리하고, 상자가 처음 위치에서 떨어지도록 함.

또한, 상자가 떨어지는 것이 또 다른 상자가 떨어지는 것을 트리거 하고, 서비스를 호출을 트리거 하여 자세를 초기화 하는 것을 보여줌.

실행 전에 몇가지 수정해야하는 부분이 있는데, `diff_drive.sdf` 파일에 있는 DiffDrive 시스템의 입력 토픽을 `cmd_vel`로 수정하자.

또한 아래에 `triggered publisher` 시스템을 추가하고 입출력 토픽 메시지와 이름을 정의해주자.

```html
<model name='vehicle_blue'>
  ...
 
  <plugin
    filename="gz-sim-diff-drive-system"
    name="gz::sim::systems::DiffDrive">
    <left_joint>front_left_wheel_joint</left_joint>
    <left_joint>rear_left_wheel_joint</left_joint>
    <right_joint>front_right_wheel_joint</right_joint>
    <right_joint>rear_right_wheel_joint</right_joint>
    <wheel_separation>1.25</wheel_separation>
    <wheel_radius>0.3</wheel_radius>
    <topic>cmd_vel</topic>
  </plugin>

  <plugin filename="gz-sim-triggered-publisher-system"
          name="gz::sim::systems::TriggeredPublisher">
    <input type="gz.msgs.Empty" topic="/start"/>
    <output type="gz.msgs.Twist" topic="/cmd_vel">
          linear: {x: 3}
    </output>
  </plugin>

</model>
```

`<input>` 태그를 통해 `TriggeredPublisher`가 `gz.msgs.Empty` 메시지 타입, 그리고 `/start` 이름의 토픽을 Subscribe 하도록 설정하였다.

반대로 `<output>` 태그는, `/start` 토픽을 받았을 때, `gz.msgs.Twist` 메시지 타입, 그리고 `/cmd_vel` 이름의 토픽을 Publish 하도록 되어있다.

이러한 `gz.msg` 타입의 데이터는 Google Protobuf message의 형태로 표현되며 자세한 내용은 [여기](https://protobuf.dev/overview/)를 참고하자.

`TriggerPublisher`는 Gazebo 토픽만을 다루기 때문에, `<plugin>` 태그가 붙을 수 있는 어느곳이던 위치할 수 있지만, 본 예제에서는 `<world>` 아래에 작성하였다.

다음으로 차량이 지면에 있는 접촉 센서를 통과할 때, 상자를 떨어뜨릴 수 있도록 트리거를 생성해보자. 상자 모델을 생성하고 이를 `box1`이라고 하면..

```html
<model name="box1">
  <pose>3 0 8 0 0 0</pose>
  <link name="box_body">
    <visual name="v1">
      <geometry>
        <box><size>1 1 1</size></box>
      </geometry>
      <material>
        <diffuse>0.8 0.2 0.2 1</diffuse>
        <specular>1.0 0 0 1</specular>
      </material>
    </visual>
    <collision name="c1">
      <geometry>
        <box><size>1 1 1</size></box>
      </geometry>
    </collision>
  </link>
</model>
```

다음으로, `TouchPlugin`과 `DetachableJoint` 시스템을 포함하는 `trigger`라는 이름의 모델을 만들자. 

```html
<model name="trigger">
    <pose>3 0 0 0 0 0</pose>
    <static>true</static>
    <link name="body">
      <visual name="v1">
        <geometry>
          <box><size>0.1 10 0.01</size></box>
        </geometry>
      </visual>
      <collision name="c1">
        <geometry>
          <box><size>0.1 10 0.01</size></box>
        </geometry>
      </collision>
      <sensor name='sensor_contact' type='contact'>
        <contact>
          <collision>c1</collision>
        </contact>
      </sensor>
    </link>
    <plugin
      filename="gz-sim-touchplugin-system"
      name="gz::sim::systems::TouchPlugin">
      <target>vehicle_blue</target>
      <namespace>trigger</namespace>
      <time>0.001</time>
      <enabled>true</enabled>
    </plugin>
    <plugin filename="gz-sim-detachable-joint-system"
            name="gz::sim::systems::DetachableJoint">
      <parent_link>body</parent_link>
      <child_model>box1</child_model>
      <child_link>box_body</child_link>
      <detach_topic>/box1/detach</detach_topic>
    </plugin>
  </model>
```
`DetachableJoint` 시스템은 body와 box_body 링크 사이의 fixed joint를 만들었다.

`trigger` 모델은 정적 모델이기 때문에 `box1`은 trigger에 붙지 않는 한 공중에 고정되어 있을 것이다.

따라서 `DetachableJoint`는 `/box1/detach` 토픽을 Subscribe하여 메시지를 받으면 고정된 Joint를 끊고 `box1`이 지면에 떨어지도록 한다.

Contact 센서를 사용하기 위한 플러그인을 `<world>`태그 내부에 작성해주자

```html
<world>
  ...
  <plugin
    filename="gz-sim-contact-system"
    name="gz::sim::systems::Contact">
  </plugin>
  ...
</world>
```

차량이 이러한 접촉 센서를 밟으면, `TouchPlugin`이 `/trigger/touched` 토픽을 Publish하고, 이를 트리거로 사용하여 `/box1/detach` 메시지를 Publish 할 것이다.

`TouchPlugin`은 접촉이 있을 때만 Publish 하므로, 어떤 메시지가 들어와도 항상 트리거 할 수 있다.

이 때, Matcher를 사용하여 Boolean 입력 메시지가 `true`일 때만 트리거가 동작하도록 해보자.

(아마도, 터치 플러그인의 경우 Publish되는 토픽은 `/<namespace>/touched` 형식으로 되어있는 것 같다.)

```html
<plugin filename="gz-sim-triggered-publisher-system"
        name="gz::sim::systems::TriggeredPublisher">
  <input type="gz.msgs.Boolean" topic="/trigger/touched">
    <match>data: true</match>
  </input>
  <output type="gz.msgs.Empty" topic="/box1/detach"/>
</plugin>
```

`box1` 모델에 고도계를 추가해서, box1이 떨어졌을 때 또다른 박스가 떨어질 수 있도록 하기 위해 box1 모델 내부에 아래 코드를 추가하자.

고도계 사용을 위해서는 `<world>` 태그 내에 플러그인을 추가해야 한다.

```html
<world>
  ...
  <plugin filename="gz-sim-altimeter-system"
    name="gz::sim::systems::Altimeter">
  </plugin>
  ...
  <model name="box1">
  <pose>3 0 8 0 0 0</pose>
  <link name="box_body">
    ...
      <sensor name="altimeter_sensor" type="altimeter">
        <topic>/altimeter</topic>
        <always_on>1</always_on>
        <update_rate>30</update_rate>
        <visualize>true</visualize>
      </sensor>
    </link>
  </model>
</world>
```
추가로 떨어질 `box2`에 대한 내용을 추가하자. `<visual>`과 `<collision>` 태그는 box1과 동일하게 설정하면 된다.
```html
<model name="box2">
  <pose>5 0 8 0 0 0</pose>
  <link name="box2_body">
    <visual name="v1">
      <geometry>
        <box><size>1 1 1</size></box>
      </geometry>
      <material>
        <diffuse>0.8 0.2 0.2 1</diffuse>
        <specular>1.0 0 0 1</specular>
      </material>
    </visual>
    <collision name="c1">
      <geometry>
        <box><size>1 1 1</size></box>
      </geometry>
    </collision>
  </link>
</model>
```
또한 기존 trigger 모델에 `DetachableJoint` 플러그인을 추가로 작성하여 box2와 body에 대한 fixed Joint를 만들자.
```html
<model name="trigger">
  ...
  <plugin filename="gz-sim-detachable-joint-system"
          name="gz::sim::systems::DetachableJoint">
    <parent_link>body</parent_link>
    <child_model>box2</child_model>
    <child_link>box2_body</child_link>
    <detach_topic>/box2/detach</detach_topic>
  </plugin>
</model>
```

`box1`에서 했던 것과 유사하게, 원하는 트리거가 발생하면 `/box2/detach`를 Publish 하도록 해야한다. 이 트리거는 고도계로 부터 만들어 내자

고도계에서 나오는 토픽 메시지인 `gz.msgs.Altimeter` 메시지 내에는 `vertical_position` 필드가 존재한다.

다른 필드값은 신경쓰지 말고, 특정 필드만 일치시키는 `TriggeredPublisher` 매처를 생성하자.

이 `vertical_position` 필드 값은 열결된 링크의 초기 고도에 대한 상대적인 고도를 나타낸다. `box1`이 지면으로 떨어지면 고도계 값은 약 -7.5로 읽히게 된다.

하지만 정확한 값은 알 수 없고 부동 소수점 수의 정확한 비교는 권장되지 않으므로 0.2의 허용오차(Tolerance)를 설정해두자.

```html
<plugin filename="gz-sim-triggered-publisher-system"
        name="gz::sim::systems::TriggeredPublisher">
  <input type="gz.msgs.Altimeter" topic="/altimeter">
    <match field="vertical_position" tol="0.2">-7.5</match>
  </input>
  <output type="gz.msgs.Empty" topic="/box2/detach"/>
</plugin>
```

#### Simulation

gz sim 명령어를 통해 해당 SDF 파일을 실행하고, 새로운 터미널을 실행시켜 아래의 커맨드를 입력한다.
```
# gz topic -t "<topic_name>" -m "<topic_msg_type" -p "<msg>"
gz topic -t "/start" -m gz.msgs.Empty -p " "
```
파란색 차량이 움직이기 시작하여, trigger에 접촉하는 순간 1번 박스가 떨어지고, 1번 박스가 떨어지고 난 후 2번 박스가 떨어지는 것을 확인할 수 있다.

만약에, 두 박스가 떨어지고 시뮬레이션을 초기화 할 경우, 로봇의 속도를 0으로 설정함과 동시에 위치 또한 초기 위치로 설정활 수 있다.

`<service>` 태그는 `/world/<file_name>/set_pose` 서비스를 호출하기 위한 여러 데이터를 포함하고 있다. 

`reqMsg`는 Google Protobuf 메시지로 표현되며 여러개의 `<service>` 태그와 `<output>` 태그를 함께 사용할 수 있다.

(아마도 input 토픽이 들어오면 output 토픽을 출력함과 동시에, service가 호출되는 구조가 아닌가 싶다)

```html
<plugin filename="gz-sim-triggered-publisher-system"
  name="gz::sim::systems::TriggeredPublisher">
  <input type="gz.msgs.Empty" topic="/reset_robot"/>
  <output type="gz.msgs.Twist" topic="/cmd_vel">
      linear: {x: 0}
  </output>
  <service
    name="/world/triggered_publisher/set_pose"
    reqType="gz.msgs.Pose"
    repType="gz.msgs.Boolean"
    timeout="3000"
    reqMsg="name: 'blue_vehicle', id: 8, position: {x: -3, z: 1}">
  </service>
</plugin>
```

시뮬레이션이 진행되고 두 박스가 떨어지고 나서 아래 명령어로 토픽을 보내 시뮬레이션을 리셋해보자.
```
gz topic -t "/reset_robot" -m gz.msgs.Empty -p " "
```
