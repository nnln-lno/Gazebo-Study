시뮬레이션에 Smoke, fog와 같은 Particle 효과를 넣어보자. 

이러한 효과가 센서에 어떤 영향을 미치는지도 살펴보자.

## Particle Emitter System

[예제 전체 코드](https://github.com/gazebosim/gz-sim/blob/main/examples/worlds/particle_emitter.sdf)

기본적으로 Particle Emitter 사용을 위해서는 플러그인을 추가해주어야 한다.

```html
<plugin
  filename="gz-sim-particle-emitter-system"
  name="gz::sim::systems::ParticleEmitter">
</plugin>
```
예제에서는 Fog를 만드는 에미터는 Gazebo fuel로 부터 가져왔는데, 시뮬레이션은 정상 구동되나 Texture 가져오지 못해 에러가 났다

따라서 아래 부분을 직접 SDF 파일로부터 옮겨 작성하였다. 이 때 png 파일 경로는 사용자 경로에 맞게 변경해주자

(Gazebo Fuel에서 가져온 Fog Generator 에 대한 SDF 모델은 아래와 같다.)
```html
<include>
  <uri>https://fuel.gazebosim.org/1.0/openrobotics/models/fog generator</uri>
</include>

# 위 내용을 아래 코드로 변경

<model name="fog_generator">
  <pose>0 0 0 0 -1.5707 0</pose>
  <static>true</static>
  <link name="fog_link">
    <particle_emitter name="emitter" type="box">
      <emitting>true</emitting>
      <size>10 10 0</size>
      <particle_size>1 1 1</particle_size>
      <lifetime>25</lifetime>
      <min_velocity>0.1</min_velocity>
      <max_velocity>0.2</max_velocity>
      <scale_rate>0.5</scale_rate>
      <rate>5</rate>
      <material>
        <diffuse>0.7 0.7 0.7</diffuse>
        <specular>1.0 1.0 1.0</specular>
        <pbr>
          <metal>
            <albedo_map>/home/minho/gazebo_ws/model/textures/fog.png</albedo_map>
          </metal>
        </pbr>
      </material>
      <color_range_image>/home/minho/gazebo_ws/model/textures/fogcolors.png</color_range_image>
    </particle_emitter>
  </link>
</model>

```

* SDF 1.6+ 부터는 `<link>`태그의 자식으로 `<particle_emitter>` 이라는 태그를 사용할 수 있다. Particle에 대한 예제는 [여기](https://gazebosim.org/api/rendering/8/particles.html)를 참고하자.
* 여기서 사용된 Fog Generator의 경우 박스 타입의 파티클을 사용하였고 10m x 10m 영역에 Fog를 만든다.
* 기본적으로 Particle은 +X 방향으로 뿌려지지만, Pitch rotation = -90을 통해, +Z 축 방향인 공중으로 파티클이 뿌려지도록 하였다.

시뮬레이션을 실행시켜보자.
```
gz sim -v 4 -r <path-to>/particle_emitter.sdf
```

이 파티클에 대한 몇가지 속성을 바꿔보고자 몇가지 커맨드를 입력해보자.

```
# 파티클 이펙트 멈추기 - 서서히 사라짐
gz topic -t /model/fog_generator/link/fog_link/particle_emitter/emitter/cmd -m gz.msgs.ParticleEmitter -p 'emitting: {data: false}'
# 파티클 이펙트 뿌리기
gz topic -t /model/fog_generator/link/fog_link/particle_emitter/emitter/cmd -m gz.msgs.ParticleEmitter -p 'emitting: {data: true}'
# 파티클 Rate 높이기
gz topic -t /model/fog_generator/link/fog_link/particle_emitter/emitter/cmd -m gz.msgs.ParticleEmitter -p 'rate: {data: 100}'
```

## How particle Effects on Sensors ??

파티클은 사실 시뮬레이션에 시각적으로 보일 뿐만 아니라, 센서 시뮬레이션에도 영향을 준다. 영향을 받는 몇가지 센서는 아래와 같다.

- camera : 일반 카메라에서 파티클이 보임
- depth_camera : 파티클이 뎁스 데이터 상에 뿌려져 보임
- rgbd_camera : RGB 상의 파티클, 그리고 Depth 카메라에는 파티클 영향을 받아 나타남
- gpu_lidar : 파티클에 영향을 받아 거리값이 뽑힘
- thermal_camera : 파티클이 열화상 이미지에 나타나지 않음

센서에 나타나는 효과를 보기 위해 [여기](https://gist.github.com/iche033/bcd3b7d3f4874e1e707e392d6dbb0aa0) 코드를 참고하여 SDF 파일을 만들자

이후 SDF파일을 실행시켜 파티클이 센서에 미치는 영향을 살펴보자.

