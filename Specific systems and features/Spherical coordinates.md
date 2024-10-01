Gazebo는 LLH(Latitude, Longitude, Height) 좌표계에서의 시뮬레이션을 지원하고 있음 (WGS84 모델 기반)

Gazebo Sim은 항상 Cartesian coordinates(X,Y,Z)에서 수행되지만, 구면 좌표계에서 사용하기 위해 몇가지 설정이 필요하다.

## Coordinates for the world origin

Cartesian Coordinates 는 `Local Tangent Plane`와 연관되어 있다. (지구 표면 특정 지점에서의 Local tangent plane)

이는 기본적으로 ENU(East-North-Up) 방식을 따른다.

![Screenshot from 2024-10-01 14-50-34](https://github.com/user-attachments/assets/3dafa717-f6a6-461d-92a3-2071f3917975)

사용자는 ENU 좌표계의 원점이 지구 표면 어디에 위치할것인지 세 가지의 방법으로 정의할 수 있다.

- SDF 
- GUI
- Gazebo Transport 서비스

원점을 변경하면, 이후에 수행되는 작업에만 영향을 미치게 된다. (이미 세계에 존재하는 모델들은 원점 좌표가 변경되어도 이동하지 않음)

#### SDF 파일

SDF파일에서, `/world/spherical_coordinates` 태그를 사용하여 원점을 정의할 수 있다. (대한민국으로 정의)

```html
<spherical_coordinates>
  <surface_model>EARTH_WGS84</surface_model>
  <world_frame_orientation>ENU</world_frame_orientation>
  <latitude_deg>37.34</latitude_deg>
  <longitude_deg>126.59</longitude_deg>
  <elevation>0</elevation>
  <heading_deg>0</heading_deg>
</spherical_coordinates>
```

지구에 맞는 WGS84 모델 이외에도 달에 맞는 MOON_SCS, 사용자의 설정에 맞는 CUSTOM_SURFACE 등을 지원하고 있다.

현재는 ENU 프레임 만을 지원하고 있다. 예제를 실행시켜보자
```
gz sim spherical_coordinates.sdf
```

#### GUI 파일

GUI의 경우 시뮬레이션 창에 나타나있는 Component Inspector 상에 있는 값들을 변경하면 반영된다.

#### Gazebo Transport Service

좌표계 원점을 서비스 콜을 통해 변경할 수 있는데, `/world/<world_name>/set_spherical_coordinates` 서비스를 사용하면 가능하다.

시뮬레이션을 실행시키고 아래의 커맨드를 입력하자
```
gz service \
-s /world/spherical_coordinates/set_spherical_coordinates \
--reqtype gz.msgs.SphericalCoordinates \
--reptype gz.msgs.Boolean \
--timeout 2000 \
--req "surface_model: EARTH_WGS84, latitude_deg: 35.6, longitude_deg: 140.1, elevation: 10.0"
```

GUI 상의 커맨드가 바뀌어있는것을 볼 수 있다.

## Check using Entity

엔티티를 만들어서 실제로 어떻게 움직이는지 확인해보자 ([엔티티 예제](https://gazebosim.org/api/sim/8/entity_creation.html))

엔티티는 `/world/<world_name>/create` 서비스를 사용하는데, 기존에 입력해야하는 `gz.msgs.EntityFactory`의 `pose` 필드 를 생략하고 `spherical_coordinates` 필드를 사용하였다.
```
gz service -s /world/spherical_coordinates/create \
--reqtype gz.msgs.EntityFactory \
--reptype gz.msgs.Boolean \
--timeout 300 \
--req 'sdf: '\
'"<?xml version=\"1.0\" ?>'\
'<sdf version=\"1.6\">'\
'<model name=\"spawned_model\">'\
'<link name=\"link\">'\
'<visual name=\"visual\">'\
'<geometry><sphere><radius>1.0</radius></sphere></geometry>'\
'</visual>'\
'<collision name=\"visual\">'\
'<geometry><sphere><radius>1.0</radius></sphere></geometry>'\
'</collision>'\
'</link>'\
'</model>'\
'</sdf>" '\
'spherical_coordinates: {latitude_deg: 35.59999, longitude_deg: 140.09999, elevation: 11.0} '
```

이렇게 생성한 엔티티를 서비스를 사용하여 특정 위치로 이동시키자.
```
gz service -s /world/spherical_coordinates/set_spherical_coordinates \
--reqtype gz.msgs.SphericalCoordinates \
--reptype gz.msgs.Boolean \
--timeout 300 \
--req 'entity: {name: "spawned_model", type: MODEL}, latitude_deg: 35.59990, longitude_deg: 140.09990'
```
