Gazebo는 3가지의 Joint Controller 플러그인을 제공한다.

- JointController
- JointPositionController
- JointTrajectoryController

# Joint Controller

- Joint Controller는 모델에 부착하여 사용하는 단일 Joint 제어
- 현재는 관절의 첫번째 축만 구동할 수 있음

### Modes of JointController

1. Velocity Mode : 사용자가 원하는 속도로 Joint를 제어할 수 있음
2. Force Mode : PID 제어기를 사용하여 관절 속도를 제어하고자 하는 경우에 사용

* Force 모드의 경우, 속도 제어를 위해, PID 제어기를 튜닝하고자 하는 사용자를 위함. [관련 파라미터](https://gazebosim.org/api/sim/8/classgz_1_1sim_1_1systems_1_1JointController.html)

* `cmd_vel` 토픽은 기본적으로 `/model/<model_name>/joint/<joint_name>/cmd_vel` 의 형태로 Publish 되거나 Subscribe 된다. (Double type)

#### Velocity Mode

아래 SDF 파일을 만들고 실행시켜보자.
```html
<?xml version="1.0" ?>
<sdf version="1.6">
  <world name="default">
    <light type="directional" name="sun">
      <cast_shadows>true</cast_shadows>
      <pose>0 0 10 0 0 0</pose>
      <diffuse>1 1 1 1</diffuse>
      <specular>0.5 0.5 0.5 1</specular>
      <attenuation>
        <range>1000</range>
        <constant>0.9</constant>
        <linear>0.01</linear>
        <quadratic>0.001</quadratic>
      </attenuation>
      <direction>-0.5 0.1 -0.9</direction>
    </light>
    <model name="joint_controller_demo">
      <pose>0 0 0 0 0 0</pose>
      <link name="base_link">
        <pose>0.0 0.0 0.0 0 0 0</pose>
        <inertial>
          <inertia>
            <ixx>2.501</ixx>
            <ixy>0</ixy>
            <ixz>0</ixz>
            <iyy>2.501</iyy>
            <iyz>0</iyz>
            <izz>5</izz>
          </inertia>
          <mass>120.0</mass>
        </inertial>
        <visual name="base_visual">
          <pose>0.0 0.0 0.0 0 0 0</pose>
          <geometry>
            <box>
              <size>0.5 0.5 0.01</size>
            </box>
          </geometry>
        </visual>
        <collision name="base_collision">
          <pose>0.0 0.0 0.0 0 0 0</pose>
          <geometry>
            <box>
              <size>0.5 0.5 0.01</size>
            </box>
          </geometry>
        </collision>
      </link>
      <link name="rotor">
        <pose>0.0 0.0 0.1 0 0 0</pose>
        <inertial>
          <inertia>
            <ixx>0.032</ixx>
            <ixy>0</ixy>
            <ixz>0</ixz>
            <iyy>0.032</iyy>
            <iyz>0</iyz>
            <izz>0.00012</izz>
          </inertia>
          <mass>0.6</mass>
        </inertial>
        <visual name="visual">
          <geometry>
            <box>
              <size>0.25 0.1 0.05</size>
            </box>
          </geometry>
          <material>
            <ambient>0.2 0.8 0.2 1</ambient>
            <diffuse>0.8 0 0 1</diffuse>
          </material>
        </visual>
        <collision name="collision">
          <geometry>
            <box>
              <size>0.25 0.1 0.05</size>
            </box>
          </geometry>
        </collision>
      </link>
 
      <joint name="world_fixed" type="fixed">
        <parent>world</parent>
        <child>base_link</child>
      </joint>
 
      <joint name="j1" type="revolute">
        <pose>0 0 -0.5 0 0 0</pose>
        <parent>base_link</parent>
        <child>rotor</child>
        <axis>
          <xyz>0 0 1</xyz>
        </axis>
      </joint>      
    </model>
  </world>
</sdf>
```
Joint Controller 플러그인을 사용하기 위해 아래 코드도 추가하자. 이 때, `<topic>` 태그로 특정 토픽으로 속도를 변경할 수 있도록 하였다.
```html
<plugin
 filename="gz-sim-joint-controller-system"
 name="gz::sim::systems::JointController">
 <topic>joint_vel</topic>
 <joint_name>j1</joint_name>
 <initial_velocity>1.0</initial_velocity>
</plugin>
```
시뮬레이션을 실행하면, 처음에 빨간블럭이 빙글빙글 초기값(1.0 rad/s)로 회전할것이다.

아래 명령어로 속도를 변경시켜보자.
```
gz topic -t "/joint_vel" -m gz.msgs.Double -p "data: 10.0"
```
토픽으로 속도값을 전달하여, 기존보다 더 빠르게 회전하는 것을 볼 수 있다.

#### Force Mode

Force 모드의 경우, 기존에 Joint Controller를 아래와 같이 수정하고, 값을 보기 위해 `JointStatePublisher` 시스템도 추가하자

```html
<plugin
 filename="gz-sim-joint-controller-system"
 name="gz::sim::systems::JointController">
 <joint_name>j1</joint_name>
 <use_force_commands>true</use_force_commands>
 <p_gain>2.0</p_gain>
 <i_gain>0.01</i_gain>
</plugin>
<plugin
 filename="gz-sim-joint-state-publisher-system"
 name="gz::sim::systems::JointStatePublisher">
 <joint_name>j1</joint_name>
</plugin>
```

그리고 아래 명령어를 통해 Joint state를 확인해보자.
```
gz topic -e -t /world/default/model/joint_controller_demo/joint_state
```
그리고 기존에 Desired velocity Topic을 아래와 같이 변경해서 주면..
```
gz topic -t "/model/joint_controller_demo/joint/j1/cmd_vel" -m gz.msgs.Double -p "data: 2.0"
```
`<p_gain>`이 2.0으로 설정될 경우, 원하는 속도를 얻어낼 수 없음을 확인할 수 있다 (튜닝 필요 - 0.2로 설정하면 올바르게 동작함)

# Joint Position Controller

- Joint Position Controller 또한, 모델에 부착하여 사용하는 단일 Joint 제어
- 사용자는 제어하려는 관절의 축을 지정할 수 있음

동일하게 PID를 사용하여 위치를 제어하며 관련 파라미터는 [여기](https://gazebosim.org/api/sim/8/classgz_1_1sim_1_1systems_1_1JointPositionController.html)에 있다.

명령 위치 `/cmd_pos`는 기본적으로 `/model/<model_name>/joint/<joint_name>/cmd_pos` 의 형태로 Publish 되거나 Subscribe 된다. (Double type)

SDF 파일에 Position Controller 플러그인을 추가하자
```html
<plugin
 filename="gz-sim-joint-position-controller-system"
 name="gz::sim::systems::JointPositionController">
 <joint_name>j1</joint_name>
 <topic>topic_name</topic>
 <p_gain>1</p_gain>
 <i_gain>0.1</i_gain>
 <d_gain>0.01</d_gain>
 <i_max>1</i_max>
 <i_min>-1</i_min>
 <cmd_max>1000</cmd_max>
 <cmd_min>-1000</cmd_min>
</plugin>
```
커맨드로 다음의 명령을 주면 빨간 로터가 해당 위치로 이동하는 것을 알 수 있다. (단위는 rad 인것 같다)
```
gz topic -t "/topic_name" -m gz.msgs.Double -p "data: -1.0" 
```
# Joint Trajectory Controller

- Joint Trajectory Controller는 하나 이상의 축을 참조하여 관절이 궤적을 따라 움직이도록 한다.

사용자가 시간에 따라 필요한 위치,속도 및 힘을 지정할 수 있으며 속도/위치 제어를 위해서는 PID 제어기가 사용된다. [관련 파라미터](https://gazebosim.org/api/sim/8/classgz_1_1sim_1_1systems_1_1JointTrajectoryController.html)

기본적으로 Trajectory 메시지는 `/model/<model_name>/joint_trajectory` 토픽을 Publish하거나 Subscribe 한다. (JointTrajectory 타입)

다음 코드를 작성하여 SDF 파일로 만들자

```html
<?xml version="1.0"?>
<sdf version="1.6">
  <world name="default">
    <scene>
      <ambient>0.4 0.4 0.4</ambient>
      <grid>false</grid>
    </scene>
    <!--              -->
    <!-- Illumination -->
    <!--              -->
    <light type="directional" name="sun">
      <cast_shadows>false</cast_shadows>
      <pose>5 5 5 0 0 0</pose>
      <diffuse>0.8 0.8 0.8 1</diffuse>
      <specular>0.2 0.2 0.2 1</specular>
      <attenuation>
        <range>1000</range>
        <constant>0.9</constant>
        <linear>0.01</linear>
        <quadratic>0.001</quadratic>
      </attenuation>
      <direction>-1 -1 -1</direction>
    </light>
 
    <!--        -->
    <!-- Models -->
    <!--        -->
    <model name="background_plane">
      <pose>-0.1 0 0 0 1.5708 0</pose>
      <static>true</static>
      <link name="background_plane_link">
        <visual name="background_plane_visual">
          <geometry>
            <plane>
              <normal>0 0 1</normal>
              <size>5 5</size>
            </plane>
          </geometry>
          <material>
            <diffuse>0.5 0.5 0.5 1</diffuse>
            <specular>0.5 0.5 0.5 1</specular>
          </material>
        </visual>
      </link>
    </model>
 
    <!-- RR robot (Red) - Position control -->
    <model name="RR_position_control">
      <pose>0 0 0 0 -3.14159 0</pose>
      <!-- Fix To World -->
      <joint name="RR_position_control_world" type="fixed">
        <parent>world</parent>
        <child>RR_position_control_link0</child>
      </joint>
      <!-- Links -->
      <link name="RR_position_control_link0">
        <collision name="RR_position_control_link0_collision_0">
          <geometry>
            <sphere>
              <radius>0.025</radius>
            </sphere>
          </geometry>
        </collision>
        <visual name="RR_position_control_link0_visual_0">
          <geometry>
            <sphere>
              <radius>0.025</radius>
            </sphere>
          </geometry>
          <material>
            <ambient>0 0.5 0.5 1</ambient>
            <diffuse>0 0.8 0.8 1</diffuse>
            <specular>0.8 0.8 0.8 1</specular>
          </material>
        </visual>
      </link>
      <link name="RR_position_control_link1">
        <pose relative_to="RR_position_control_joint1">0 0 0.1 0 0 0</pose>
        <collision name="RR_position_control_link1_collision_0">
          <geometry>
            <cylinder>
              <radius>0.01</radius>
              <length>0.2</length>
            </cylinder>
          </geometry>
        </collision>
        <collision name="RR_position_control_link1_collision_1">
          <geometry>
            <sphere>
              <radius>0.0125</radius>
            </sphere>
          </geometry>
        </collision>
        <visual name="RR_position_control_link1_visual_0">
          <geometry>
            <cylinder>
              <radius>0.01</radius>
              <length>0.2</length>
            </cylinder>
          </geometry>
          <material>
            <ambient>0.5 0 0 1</ambient>
            <diffuse>0.8 0 0 1</diffuse>
            <specular>0.8 0 0 1</specular>
          </material>
        </visual>
        <visual name="RR_position_control_link1_visual_1">
          <pose relative_to="RR_position_control_joint1">0 0 0.2 0 0 0</pose>
          <geometry>
            <sphere>
              <radius>0.0125</radius>
            </sphere>
          </geometry>
          <material>
            <ambient>0.5 0.5 0.5 1</ambient>
            <diffuse>0.8 0.8 0.8 1</diffuse>
            <specular>0.8 0.8 0.8 1</specular>
          </material>
        </visual>
        <inertial>
          <mass>0.1</mass>
          <inertia>
            <ixx>0.0003358</ixx>
            <iyy>0.0003358</iyy>
            <izz>0.000005</izz>
          </inertia>
        </inertial>
      </link>
      <link name="RR_position_control_link2">
        <pose relative_to="RR_position_control_joint2">0 0 0.1 0 0 0</pose>
        <collision name="RR_position_control_link2_collision">
          <geometry>
            <cylinder>
              <radius>0.01</radius>
              <length>0.2</length>
            </cylinder>
          </geometry>
        </collision>
        <visual name="RR_position_control_link2_visual">
          <geometry>
            <cylinder>
              <radius>0.01</radius>
              <length>0.2</length>
            </cylinder>
          </geometry>
          <material>
            <ambient>0.5 0 0 1</ambient>
            <diffuse>0.8 0 0 1</diffuse>
            <specular>0.8 0 0 1</specular>
          </material>
        </visual>
        <inertial>
          <mass>0.1</mass>
          <inertia>
            <ixx>0.0003358</ixx>
            <iyy>0.0003358</iyy>
            <izz>0.000005</izz>
          </inertia>
        </inertial>
      </link>
      <!-- Joints -->
      <joint name="RR_position_control_joint1" type="revolute">
        <pose relative_to="RR_position_control_link0">0 0 0 0 0 0</pose>
        <parent>RR_position_control_link0</parent>
        <child>RR_position_control_link1</child>
        <axis>
          <xyz>1 0 0</xyz>
          <dynamics>
            <damping>0.5</damping>
          </dynamics>
        </axis>
      </joint>
      <joint name="RR_position_control_joint2" type="revolute">
        <pose relative_to="RR_position_control_link1">0 0 0.1 0 0 0</pose>
        <parent>RR_position_control_link1</parent>
        <child>RR_position_control_link2</child>
        <axis>
          <xyz>1 0 0</xyz>
          <dynamics>
            <damping>0.25</damping>
          </dynamics>
        </axis>
      </joint>
      <!-- Controller -->
    </model>
  </world>
</sdf>
```
또한 TrajectoryController 플러그인을 추가하자.
```html
 <plugin
 filename="gz-sim-joint-trajectory-controller-system"
 name="gz::sim::systems::JointTrajectoryController">
 <joint_name>RR_position_control_joint1</joint_name>
 <initial_position>0.7854</initial_position>
 <position_p_gain>20</position_p_gain>
 <position_i_gain>0.4</position_i_gain>
 <position_d_gain>1.0</position_d_gain>
 <position_i_min>-1</position_i_min>
 <position_i_max>1</position_i_max>
 <position_cmd_min>-20</position_cmd_min>
 <position_cmd_max>20</position_cmd_max>
 
 <joint_name>RR_position_control_joint2</joint_name>
 <initial_position>-1.5708</initial_position>
 <position_p_gain>10</position_p_gain>
 <position_i_gain>0.2</position_i_gain>
 <position_d_gain>0.5</position_d_gain>
 <position_i_min>-1</position_i_min>
 <position_i_max>1</position_i_max>
 <position_cmd_min>-10</position_cmd_min>
 <position_cmd_max>10</position_cmd_max>
</plugin>
```
이후 아래 명령어로 Trajectory의 각 조인트에 대한 Position 명령을 주자
```
gz topic -t "/model/RR_position_control/joint_trajectory" -m gz.msgs.JointTrajectory -p '                                                                                                  
    joint_names: "RR_position_control_joint1"
    joint_names: "RR_position_control_joint2"
    points {            
      positions: -0.7854
      positions: 1.5708
      time_from_start { 
        sec: 0         
        nsec: 250000000 
      }           
    }                   
    points {            
      positions: -1.5708
      positions: 0      
      time_from_start { 
        sec: 0          
        nsec: 500000000 
      }                 
    }                  
    points {            
      positions: -1.5708
      positions: -1.5708
      time_from_start {
        sec: 0         
        nsec: 750000000
      }           
    }                  
    points {      
      positions: 0
      positions: 0
      time_from_start {
        sec: 1   
        nsec: 0}}'
```

각 조인트가 움직이는 것을 볼 수 있다. 시뮬레이션으로 전송한 토픽의 결과는 아래의 명령어로 확인해보자
```
gz topic -e -t "/model/RR_position_control/joint_trajectory_progress"
```
