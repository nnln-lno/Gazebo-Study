Gazebo 에서 사용되는 용어들 정리

- <strong>World</strong> : <br/>움직일 수 있거나 고정된 모든 로봇 혹은 객체(Object), 플러그인, GUI 등을 포함한 시뮬레이션에 대한 설명. SDF 파일의 <world> 태그에 해당.
-  <strong>Entity</strong> : <br/>월드상에 존재하는 모든 객체로 모델(model), 링크(link), 충돌(collision), 시각적 요소(visual) 등을 포함하며 여러 컴포넌트가 연결될 수 있음. 이러한 엔티티는 단순한 숫자 ID가 있으며 런타임에 할당된다.
- <strong>Component</strong> : <br/>엔티티에 특정 기능이나 특성(name, pose, material)을 추가함. 다양한 컴포넌트 요소가 있으며 개발하고자 하는 사람들은 `BaseComponent` 클래스를 상속하거나 `Component` 템플릿을 인스턴스화 하여 자신만의 컴포넌트를 만들 수 있음
- <strong>System</strong> : <br/>여러 컴포넌트들을 가진 모든 엔티티에서 동작하는 로직. 런타임에 로드될 수 있는 '플러그인'이며 Gazebo는 여러 시스템을 제공하고 개발자 또한 자신만의 시스템 빌드 가능.
- <strong>Entity-Component-Manager(ECM)</strong> : <br/>엔티티와 컴포넌트를 조회, 생성, 삭제 및 업데이트하는 기능을 제공함.
- <strong>Level</strong> : <br/>박스 형태의 볼륨과 그 안에 있는 정적 엔티티로 정의된 월드의 일부. 엔티티는 하나 이상의 레벨에 속할 수 있으며 어떤 레벨에도 속하지 않을 수 있음. 또한 레벨은 서로 겹칠 수 있고 서로 멀리 떨어져있을 수 있음.
- <strong>Buffer Zone</strong> : <br/>각 레벨에는, 그 경계 밖으로 확장된 버퍼 존이 있으며, 이는 Performer가 레벨에 진입하거나, 레벨에서 벗어나 충분히 멀리 떨어져있을 때 이를 감지하는데 사용된다.
- <strong>Global Entities</strong> : <br/>태양, 지평면, 높이맵과 같이 모든 레벨에 존재하는 엔티티. 이 엔티티들은 모든 `simmulation runner`에 걸쳐 복제된다.
- <strong>Default Level</strong> : <br/>다른 어떤 레벨에도 속하지 않은 모든 엔티티를 처리하는 레벨.
- <strong>Network Manager</strong> : <br/>여러 프로세스에 걸쳐 분산된 시뮬레이션에서 정보의 흐름을 제어함.
- <strong>Primary/Secondary Network Manager</strong> : <br/>월드마다 하나의 기본 네트워크와 하나 이상의 보조 러너가 존재함. 보조 러너는 월드의 일부 레벨을 실행하며, 기본 러너는 모든 보조 러너들을 '동기화'한다. 러너로 나뉘지 않은 월드는 기본 러너를 가지지 않음.
- <strong>Event Manager</strong> : <br/>시스템과 서버 간에 보낼 수 있는 이벤트를 관리함. "플러그인"은 사용자 정의 이벤트를 생성하고 전송할 수 있으며, Gazebo에서 이벤트를 전송하거나 수신할 수 있음.
- <strong>Simulation Runner</strong> : <br/>하나의 월드 또는 월드의 일부 레벨을 실행하며, 동시에 여러 개의 월드를 실행할 수 없음.
    - 실행중인 레벨, 월드, 퍼포머(Performer)와 관련된 모든 엔티티와 컴포넌트가 포함된 단일 ECM을 갖고 있음.
    - 이벤트 관리자를 포함하고 있음
    - 시뮬레이션이 분산된 경우, 네트워크 관리자를 포함하고 있음.
    - 시스템들을 로드함.
- <strong>Server</strong> : <br/>Gazebo Sim의 진입점으로, SDF파일을 로드하고 월드별로 시뮬레이션 러너를 인스턴스화 하는 역할을 함.
    
