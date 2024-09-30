`Mouse Drag` 플러그인은 마우스를 사용하여 물체들을 드래그 하고 Force 혹은 Torque를 적용할 수 있게 한다.

기본적으로 Rotation 과 Translation, 2가지 모드가 있으며 2가지 방법을 통해 해당 플러그인 사용이 가능하다.

1. SDF 파일에 Plugin을 추가하여 사용 (SDF Configuration)
   
```
<sdf version="1.6">
  <world name="mouse_drag">
    <gui>
      <plugin filename="MouseDrag">
        <rotation_stiffness>100.0</rotation_stiffness>
        <position_stiffness>100.0</position_stiffness>
      </plugin>
      ...
    </gui>
    ...
  </world>
</sdf>
```

2. Gazebo 우측 상단 플러그인 리스트에서 `Mouse Drag`를 추가하여 사용

## Rotation Mode

`Rotation Mode`를 활성화 시키기 위해, `LCtrl`+`L Click`을 꾹 누르면 된다.

반투명의 바운딩박스가 물체 주변에 나타나고, 현재 마우스 위치에 따라 물체가 회전하게 된다.

토크의 `Magnitude`는 `Spring-Damper` 시스템을 사용하여 계산되는데 이 때 `constant stiffness` 와 `critial damping`가 사용된다. (링크의 `Inertia`와 관련있음)

따라서 `constant stiffness` 일지라도 물체에 따라 효과가 다르게 나타날 수 있다. `Rotational Stiffness`는 인터페이스에서 변경할 수 있다.

## Translation Mode

`Translation Mode`를 활성화 시키기 위해서는, 반대로 `LCtrl`+`R Click`을 꾹 누르면 된다.

마우스를 드래그 하면 마우스의 위치로 링크가 움직이는 것을 볼 수 있다. 

이 때 `Apply force to center of mass` 옵션을 선택하면 항상 물체의 질량 중심을 기준으로 힘이 적용된다. 그렇지 않을 경우, 내가 클릭한 곳이 기준점이 된다.

질량 중심이 선택되지 않으면, 힘 적용 지점의 오프셋을 고려해야 하므로 추가적인 토크가 적용된다. 이 경우 물체의 회전도 강성에 따라 감쇠되기도 하며, 위치 강성은 인터페이스에서 수정할 수 있다.
