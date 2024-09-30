Gazebo Sim에서 제공하는 `level` 기능에 대해 알아보자.

요약하면, 객체가 어떤 환경에 근접함에 따라 시뮬레이션에서 새로운 객체를 로드 및 언로드 하여 대규모 환경에서 시뮬레이션의 성능을 향상시킨다.

먼저, `level`이란 상자 형태로 정의된 세계의 일부이며, 그 안에는 정적 엔티티들로 구성된다. 

엔티티는 여러 레벨에 존재할수도 있고, 어떤 레벨에도 존재하지 않을 수 있음. 

레벨의 경우 그 영역이 겹칠수 있으며 서로 멀리 떨어져 있을 수도 있다.

각 레벨은 버퍼 구역을 가지며, 이는 레벨의 경계 밖으로 부풀려진 레벨의 부피를 말하며 어떤 `Performer`(수행자)가 레벨에 들어오거나 멀리 떨어져서 엔티티를 레벨에서 제외할 수 있을 정도로 나갔는지를 감지하는 데 사용된다.

여기서 `Performer`는 시뮬레이션 중 레벨이 변경될 수 있는 모든 시뮬레이션 엔티티(로봇, 액터, 동적 모델 등)를 말한다. 이는 레벨이나 분산 시뮬레이션의 맥락에서만 의미가 있음.

레벨을 활성화 하기 위해서는 SDF 파일에 적절한 태그를 추가하고, `--levels` 플래그와 함께 Gazebo를 실행하여야 한다. 

(이 모든 설명은 단일 서버에서 실행되는 시뮬레이션을 의미)

## Understanding Level using Figures.

만약에, World에 몇개의 `level`을 돌아다닐 수도 있는 `Performer`가 하나 있다고 하자.

또한 이 수행자를 포함하는 레벨을 `default level`이라고 하며 둘다 초기에 로드 된다.

수행자가 World를 이동함에 따라 `runner`는 레벨을 로드/언로드 할 것이다.

- 레벨을 `load`하는 것은 해당 레벨에서 모델을 위한 엔티티와 컴포넌트를 생성하는것을 의미한다.
- 레벨을 `unload`하는 것은 반대로 해당 레벨에서 엔티티와 컴포넌트를 제거하는것을 의미함.
- 내부 state를 유지하는 시스템은, 엔티티가 추가되거나 제거될때 적절히 반응해야한다.
- `Performer`가 버퍼 구역에 진입하면 레벨이 로드된다.
- `Performer`가 버퍼 구역을 나가면 레벨이 언로드 된다.

아래 그림을 보고 설명하면 다음과 같다.

![image](https://github.com/user-attachments/assets/27b0c6cf-839a-4649-ad4d-e85f542998c0)

- <strong>Green Area</strong>는 시뮬레이션이 이루어지는 world의 구역을 의미한다.
- 보라색 점선으로 구분되어 있는 각각의 구역은 `Level`이라고 하며 현재 그림상으로 3개의 레벨(L1~L3)이 존재함.
- L1 밖의 파란색 영역은 Level 1에 대한 버퍼 구역을 나타내며, L2, L3에 대한 버퍼 구역은 생략함
- 각각의 오렌지 색상의 물체는 world의 `static model`을 나타낸다. (M1~M6)
- 빨간 색상의 물체는 로봇 `Performer`를 나타낸다 (R1)

엔티티는 레벨에 속하는 엔티티와, 글로벌 엔티티로 나뉜다.

- M1 은 L1에 속한다. 따라서 L1이 로드/언로드 될 때, 해당 모델에 대한 엔티티가 생성/제거 된다. L2에 속하는 M2, L3에 속하는 M4, M5도 동일하다.
- M3 는 하나 이상의 레벨에 속하고, 그 레벨들 중 하나가 로드되면 M3가 로드될 것이다.
- M6 는 어떠한 레벨에도 속하지 않기에 `Global Entity`라고 하며, <strong>항상 로드된다</strong>. 이러한 엔티티는 필요한 경우가 아니라면 최소화 해야한다. (예를 들면 무한대로 큰 Ground Plane)

## What happen as the performer moves ??

1. R1 이 L1 안에서 시작하였고, 이는 시뮬레이션은 녹색 선으로 강조해둔 요소들을 초기화하여 로드할 것이다. 로드 되는 요소들을 아래에 나열함
   - R1, `Performer`
   - M1 과 M3, L1에 속해있으므로
   - M6, `Global Entity` 이므로
     
![image](https://github.com/user-attachments/assets/cbb04d9b-64eb-4f10-90d1-393b2e2a718c)


2. Performer가 L3에 방향인, 남쪽으로 이동하여 L3의 버퍼존에 이동하게 되면, M4와 M5를 로드함. 이 때, L1 와 L3 둘다 로드된다.

![image](https://github.com/user-attachments/assets/7a910527-939d-48be-848e-8e06be1a9ff1)

3. Performer가 더 남족으로 이동하여 L1의 버퍼 구역, 그리고 L3 내부로 이동하게 될지라도 L1, L3 모두 로드됨. (R1은 여전히 L1의 버퍼구역에 있으므로

![image](https://github.com/user-attachments/assets/f007e06f-f5d3-43e2-b7c8-8eaa5027a409)

4. 결론적으로, R1이 L1의 버퍼 구역을 벗어나게 되면, L1을 언로드하고 그에따라 M1 엔티티 또한 언로드 된다.

![image](https://github.com/user-attachments/assets/388d638d-8727-43e2-bfe0-81741d621ce7)

## How to use in SDF?

2개의 SDF 요소가 존재함
- `<level>`
- `<performer`

Level과 Performer 개념은 Gazebo에 특화되어 있고, 이를 직접적으로 `<world>` 태그 아래에 두면 SDF의 일반성이 떨어질 수 있음.

이를 위해 `<extension>` 태그가 제안되었지만, 아직 구현되지는 않았기에 현재로서는 `<level>` 및 `<performer>` 태그를 `<plugin name="gz::sim" filename="dummy">` 태그에 추가할것임.

### level 태그

해당 태그는, 레벨이 차지하는 부피와 레벨 내의 엔티티에 대한 정보를 포함하고 있음.

부피의 경우 `<box>`와 같은 기하학적이며, 이를 통해 퍼포머가 레벨 내에 있는지 여부를 결정짓는다.

(현재, 박스 형태는 교차 계산 속도를 높이기 위해 내부적으로 축에 정렬된 박스로 변환된다.)

레벨의 부피는 `<pose>` 태그를 통해 World 프레임에 대해 지정되지만, 방향 부분은 무시된다.

`<buffer>` 태그는 레벨 부피에 대한 버퍼 구역을 표현하는데 사용된다.

레벨과 관련있는 엔티티는 `<ref>` 태그를 사용하여 지정되고, 태그 내부에는 엔티티의 이름을 입력해야 한다.

레벨은 최소 하나 이상의 `<ref>` 태그를 포함할 수 있고, <strong>해당 태그로 지정된 엔티티는 위치가 레벨 밖에 있더라도 레벨의 일부로 간주된다는 점에 유의</strong> 따라서 모든 엔티티가 레벨 부피 내에 있는지 확인이 필요함.

```html
<level name="level1">
  <pose>0 0 5 0 0 0</pose>
  <geometry>
    <box>
      <size>10 10 10</size>
    </box>
  </geometry>
  <buffer>2</buffer>
  <ref>model1</ref>
  <ref>model2</ref>
</level>
```

### Performer 태그

해당 태그는, 퍼포머 엔티티(대부분의 모델)에 대한 참조를 포함. `<ref>` 태그는 퍼포머 엔티티의 이름을 지정이 필수적이며 독립적으로 하나만 있어야함 (여러개의 퍼포머가 동일한 엔티티를 가리킬 수 없음).

또한 태그는 퍼포머가 차지하는 부피에 대한 정보를 포함한다. 이 부피는 `<geometry>` 태그로 지정되며 <strong>현재는 `<box>`태그만 지원된다.</strong>

```html
<performer name="perf1">
  <ref>robot1</ref>
  <geometry>
    <box>
      <size>1 1 1</size>
    </box>
  </geometry>
</performer>
```

퍼포머(Performer)는 이렇게 SDF 형식으로 지정하여 시뮬레이션을 실행할 수 있지만, Service를 콜 하여 퍼포머를 지정할 수 있음.

1. `levels_no_performers.sdf`를 통해 월드를 실행
```
gz sim levels_no_performers.sdf -v 4 --levels
```
시뮬레이션 화면에 2개의 Vehicle 모델이 보일 것이다. 하지만 이 두 모델은 "Level Loading"을 발생시키지 않는다. 게다가 service를 콜 하기 전까지는 Performer가 아니다.

2. Performer 지정하기
```
gz service -s /world/levels/level/set_performer --reqtype gz.msgs.StringMsg --reptype gz.msgs.Boolean --timeout 2000 --req 'data: "vehicle_blue"'
```

해당 시뮬레이션을 SDF로 나타내면 다음과 같다.

![image](https://github.com/user-attachments/assets/27bcec13-71da-41ef-9dd6-156f565677d2)

```html
<?xml version="1.0" ?>
<sdf version="1.6">
<world name="default">
  <model name="M1">
    <static>1</static>
    <pose>-8 8 0 0 0 0</pose>
    <!-- other links -->
  </model>
  <model name="M2">
    <static>1</static>
    <pose>8 5 0 0 0 0</pose>
    <!-- other links -->
  </model>
  <model name="M3">
    <static>1</static>
    <pose>0 0 0 0 0 0</pose>
    <!-- other links -->
  </model>
  <model name="M4">
    <static>1</static>
    <pose>-8 -8 0 0 0 0</pose>
    <!-- other links -->
  </model>
  <model name="M5">
    <static>1</static>
    <pose>-5 -5 0 0 0 0</pose>
    <!-- other links -->
  </model>
  <model name="M6">
    <static>1</static>
    <pose>-12 -8 0 0 0 0</pose>
    <!-- other links -->
  </model>
 
  <model name="R1">
    <pose>-5 5 0 0 0 0</pose>
    <!-- other links and joints-->
  </model>
  <model name="R2">
    <pose>-5 8 0 0 0 0</pose>
    <!-- other links and joints-->
  </model>
  <model name="R3">
    <pose>5 2 0 0 0 0</pose>
    <!-- other links and joints-->
  </model>
 
  <plugin name="gz::sim" filename="dummy">
    <performer name="perf1">
      <ref>R1</ref>
      <geometry>
        <box>
          <size>2 2 2</size>
        </box>
      </geometry>
    </performer>
    <performer name="perf2">
      <ref>R2</ref>
      <geometry>
        <box>
          <size>2 2 2</size>
        </box>
      </geometry>
    </performer>
    <performer name="perf3">
      <ref>R3</ref>
      <geometry>
        <box>
          <size>2 2 2</size>
        </box>
      </geometry>
    </performer>
 
    <level name="L1">
      <pose>-5 5 5 0 0 0</pose>
      <geometry>
        <box>
          <size>10 10 10</size>
        </box>
      </geometry>
      <buffer>2</buffer>
      <ref>M1</ref>
      <ref>M3</ref>
    </level>
    <level name="L2">
      <pose>5 5 5 0 0 0</pose>
      <geometry>
        <box>
          <size>10 10 10</size>
        </box>
      </geometry>
      <buffer>2</buffer>
      <ref>M2</ref>
      <ref>M3</ref>
    </level>
    <level name="L3">
      <pose>-5 -5 5 0 0 0</pose>
      <geometry>
        <box>
          <size>10 10 10</size>
        </box>
      </geometry>
      <buffer>2</buffer>
      <ref>M3</ref>
      <ref>M4</ref>
      <ref>M5</ref>
    </level>
  </plugin>
</world>
</sdf>
```




