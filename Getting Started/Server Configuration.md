기본적인 Gz Sim의 동작 방식에 대한 이해

## Default configuration

```
gz sim empty.sdf
```

해당 명령어를 실행시키고 `Entity Tree`의 `ground_plane`을 제거한 후에 블럭을 생성하고 왼쪽 하단의 `Play`버튼을 누르면 중력으로 블럭이 떨어지는 것을 볼 수 있다.

이러한 과정에서 몇가지 시스템이 동작한다.

- 블럭(구, 육면체, 원기둥)이 올바르게 삽입되는 것은, `user-commands-system`이 로드 되었기 때문.
- 중력으로 인해, 블럭이 떨어지는 것은 `physics-system`이 로드 되었기 때문.
- 블럭의 Shape가 GUI를 통해 떨어지는 것을 볼 수 있는 것은, `scene-broadcaster-system`이 로드 되었기 때문.

기본적으로 터미널에서 위의 명령어를 실행하면, `$HOME/.gz/sim/<#>/server.config`가 로드된다. (여기서 <#>은 버젼)

사용자가 해당 파일을 제거하지 않는 한, 절대로 다시 만들어지지 않으며 이는 사용자가 선호에 따라 자신만의 "Config"를 만들 수 있음을 의미한다. (이는 Gazebo가 실행될 때 마다 적용될것임)

해당파일을 아래와 같이 수정해보자 (physics-system 주석처리)

```
<server_config>
  <plugins>
    <!-- <plugin entity_name="*"
            entity_type="world"
            filename="gz-sim-physics-system"
            name="gz::sim::systems::Physics">
    </plugin> -->
    <plugin entity_name="*"
            entity_type="world"
            filename="gz-sim-user-commands-system"
            name="gz::sim::systems::UserCommands">
    </plugin>
    <plugin entity_name="*"
            entity_type="world"
            filename="gz-sim-scene-broadcaster-system"
            name="gz::sim::systems::SceneBroadcaster">
    </plugin>
  </plugins>
</server_config>

```

이 후, `empty.sdf`를 불러와 위의 과정을 똑같이 따라하면, 블럭이 떨어지지 않고 제자리에 있게 된다. (physics-system 이 로드되지 않았기 때문)

혹시, 백업파일이 없어 초기로 되돌리고 싶은 경우 Modify한 config 파일을 삭제하면 다음번에 Gazebo를 실행할 시에 자동으로 만들어짐.

## SDF

새로운 파일을 만들고 각자 원하는 에디터를 사용하여 아래의 코드를 작성하여 `fuel_preview.sdf`로 저장하자.

```
<?xml version="1.0" ?>
<sdf version="1.6">
  <world name="fuel_preview">
    <plugin
      filename="gz-sim-scene-broadcaster-system"
      name="gz::sim::systems::SceneBroadcaster">
    </plugin>
 
    <gui fullscreen="0">
 
      <!-- 3D scene -->
      <plugin filename="MinimalScene" name="3D View">
        <gz-gui>
          <title>3D View</title>
          <property type="bool" key="showTitleBar">false</property>
          <property type="string" key="state">docked</property>
        </gz-gui>
 
        <engine>ogre2</engine>
        <scene>scene</scene>
        <ambient_light>1.0 1.0 1.0</ambient_light>
        <background_color>0.4 0.6 1.0</background_color>
        <camera_pose>8.3 7 7.8 0 0.5 -2.4</camera_pose>
      </plugin>
      <plugin filename="GzSceneManager" name="Scene Manager">
        <gz-gui>
          <property key="resizable" type="bool">false</property>
          <property key="width" type="double">5</property>
          <property key="height" type="double">5</property>
          <property key="state" type="string">floating</property>
          <property key="showTitleBar" type="bool">false</property>
        </gz-gui>
      </plugin>
      <plugin filename="InteractiveViewControl" name="Interactive view control">
        <gz-gui>
          <property key="resizable" type="bool">false</property>
          <property key="width" type="double">5</property>
          <property key="height" type="double">5</property>
          <property key="state" type="string">floating</property>
          <property key="showTitleBar" type="bool">false</property>
        </gz-gui>
      </plugin>
 
    </gui>
 
    <include>
      <uri>https://fuel.gazebosim.org/1.0/OpenRobotics/models/Sun</uri>
    </include>
 
    <include>
      <uri>https://fuel.gazebosim.org/1.0/OpenRobotics/models/Construction Cone</uri>
    </include>
 
  </world>
</sdf>
```

 이후 아래의 명령어로 Gazebo를 실행해보자. 이 때, `-r` 옵션은 우리가 `play`버튼을 누르지 않아도 Gazebo 실행과 동시에 자동으로 `Play`하는 역할을 함.

```
gz sim -r <path to>/fuel_preview.sdf
```

위에 server.config를 원래대로 돌려놓고 실행했음에도, 물체가 떨어지지 않는 것을 볼 수 있다. 왜그럴까??

만약 SDF 파일에 `<plugin>` 요소가 있다면, 기본으로 실행되는 `GZ_SIM_SERVER_CONFIG_PATH=$HOME/.gz/sim/<#>/server.config`가 무시되고 SDF 내부에 있는 플러그인들로 동작하게 되어있음.

따라서 SDF 파일의 `<world>`태그 바로 아래의 scene-broadcaster-system에 해당하는 `<plugin>` 블록을 제거하면 기본으로 실행되는 `server.config`가 로드되고, 물체가 정상적으로 떨어지게 된다.

## Environment variable

아무래도 SDF에 직접 `plugin`들을 집어넣는것은 불편하다. 그러면서 여러 시뮬레이션 각각에 대한 config, 그리고 그 안의 `plugin`들을 넣는것 또한 불편하다.

이러한 불편한 점들을 해소하고자, Gazebo에서는 `environment variable`이라는 것을 지원하고 있다.

아래의 코드를 작성하고, `simple_camera.sdf`로 저장해두자.

```
<?xml version="1.0" ?>
<sdf version="1.6">
  <world name="simple_camera">
 
    <gui fullscreen="0">
 
      <!-- 3D scene -->
      <plugin filename="MinimalScene" name="3D View">
        <gz-gui>
          <title>3D View</title>
          <property type="bool" key="showTitleBar">false</property>
          <property type="string" key="state">docked</property>
        </gz-gui>
 
        <engine>ogre2</engine>
        <scene>scene</scene>
        <ambient_light>1.0 1.0 1.0</ambient_light>
        <background_color>0.4 0.6 1.0</background_color>
        <camera_pose>8.3 7 7.8 0 0.5 -2.4</camera_pose>
      </plugin>
      <plugin filename="GzSceneManager" name="Scene Manager">
        <gz-gui>
          <property key="resizable" type="bool">false</property>
          <property key="width" type="double">5</property>
          <property key="height" type="double">5</property>
          <property key="state" type="string">floating</property>
          <property key="showTitleBar" type="bool">false</property>
        </gz-gui>
      </plugin>
      <plugin filename="InteractiveViewControl" name="Interactive view control">
        <gz-gui>
          <property key="resizable" type="bool">false</property>
          <property key="width" type="double">5</property>
          <property key="height" type="double">5</property>
          <property key="state" type="string">floating</property>
          <property key="showTitleBar" type="bool">false</property>
        </gz-gui>
      </plugin>
 
      <plugin filename="ImageDisplay" name="Image Display">
        <gz-gui>
          <property key="state" type="string">floating</property>
        </gz-gui>
      </plugin>
 
    </gui>
 
    <include>
      <uri>https://fuel.gazebosim.org/1.0/OpenRobotics/models/Sun</uri>
    </include>
 
    <include>
      <pose>0 0 1 0 0 0</pose>
      <uri>https://fuel.gazebosim.org/1.0/OpenRobotics/models/Gazebo</uri>
    </include>
 
    <model name="camera">
      <static>true</static>
      <pose>20 0 1.0 0 0.0 3.14</pose>
      <link name="link">
        <pose>0.05 0.05 0.05 0 0 0</pose>
        <visual name="visual">
          <geometry>
            <box>
              <size>0.1 0.1 0.1</size>
            </box>
          </geometry>
        </visual>
        <sensor name="camera" type="camera">
          <camera>
            <horizontal_fov>1.047</horizontal_fov>
            <image>
              <width>320</width>
              <height>240</height>
            </image>
          </camera>
          <update_rate>30</update_rate>
        </sensor>
      </link>
    </model>
 
  </world>
</sdf>
```

Gazebo를 통해 확인해보자.

```
gz sim -r <path to>/simple_camera.sdf
```

좌측 상단에 `Image Display`가 보이지만, Refresh를 해도 어떠한 토픽도 나오지 않는다. 분명 SDF에 카메라 모델을 담았음에도 말이다.

이는 기본으로 로드되는 `server.config` 내부에는 `sensor system`을 포함하고 있지 않기 때문이다. <strong>(rendering-based sensor 에 필수적인 플러그인)</strong>

따라서 `$HOME/.gz/sim/<#>` 폴더 내에 아래의 코드를 붙여넣어 새로운 config 파일을 만들고 `rendering_sensors_server.config`로 저장하자.

```
<server_config>
  <plugins>
    <plugin entity_name="*"
            entity_type="world"
            filename="gz-sim-scene-broadcaster-system"
            name="gz::sim::systems::SceneBroadcaster">
    </plugin>
    <plugin entity_name="*"
            entity_type="world"
            filename="gz-sim-sensors-system"
            name="gz::sim::systems::Sensors">
      <render_engine>ogre</render_engine>
    </plugin>
  </plugins>
</server_config>
```

이후, 환경 변수를 지정해주자.

```
export GZ_SIM_SERVER_CONFIG_PATH=$HOME/.gz/sim/rendering_sensors_server.config
```

`sensors-system` 플러그인을 추가하자, 기존에 보이지 않던 토픽이 나타나고 카메라 데이터가 보이는 것을 확인할 수 있다.
