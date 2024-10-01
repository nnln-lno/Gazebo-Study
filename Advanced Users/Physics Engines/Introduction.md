Gazebo Physics(gz-physics)는 Gazebo의 라이브러리 구성 요소로, 로봇 및 시뮬레이션을 신속하게 개발하기 위해 설계된 라이브러리임.

라이브러리의 주 목표는, 다양한 물리 엔진에 대한 추상적인 계층 구조를 제공하는 것으로 사용자가 코드를 최소한으로 변경하여 물리 엔진을 사용할 수 있게 하는것.

사용자는 특정 기능을 지원하는 물리 엔진을 요청할 수 있고, 플러그인을 로딩하는 매커니즘은 요청된 기능을 구현하는 엔진만 로드함.

사용자가 선택한 물리 엔진은 호환 가능한 플러그인 인터페이스를 통해 기존 코드 베이스에 통합할 수 있으며, 이는 확장성과 모듈성을 제공함

## Overview

![image](https://github.com/user-attachments/assets/d125fe7a-6afb-495e-8648-a3f4033e7034)

* 본 그림은 이전 버전인 Ignition(Ign)으로 설명되었지만, gz-physics로 이해하면 된다. (Gazebo Physics 7.3.0 기준 작성)

일반적으로 gz-sim 은 다양한 라이브러리의 구성으로 구동되는 시뮬레이션 라이브러리다. 

- 그래픽 그리기는 `gz-rendering`에 의해 지원
- 시뮬레이션 센서는 `gz-sensors`에서 정의되고 구현
- 물리엔진에 대한 인터페이스는 `gz-physics`에서 제공
- 객체의 동적 변화와 상호작용을 시뮬레이션 하는 것은 `gz-sim`에서 수행

이러한 모든 라이브러리들은 C++코드로 상호 연결되어 있다.

Gazebo Physics는 각 물리 엔진이 플러그인으로 구현되고 런타임에 로드될 수 있는 플러그인 아키텍처를 사용한다.

사용자가 물리엔진을 택할 수 있도록 `Gazebo Physics`는 다음 네 가지의 주요 요소를 도입함.

1. <strong>Entity</strong> : 모든 객체를 추상화 하는 기본 클래스, 해당 객체를 가리키는 최소한의 설명 데이터를 포함한다. (고유식별자, 참조 등)
2. <strong>FeaturePolicy</strong> : 기능에 대한 메타데이터를 제공하는 Policy 클래스이다. 이는 차원의 수(2D or 3D)와 소수점 타입(float or double)에 따라 Gazebo Physics의 API를 사용자 정의 할 수 있도록 해줌
   - Policy-based Class : 작은 단위의 클래스들을 결합하여 복합된 새로운 클래스를 디자인하는 방법론
3. <strong>Feature</strong> : 물리 엔진의 기능을 정의하는 개념
4. <strong>FeatureList</strong> : 기능 목록의 집합

사용되는 외부 물리 엔진(DART, TPE, Bullet 등)에 따라 내부 물리 엔진 인터페이스는 다를 수 있지만, Gazebo Physics가 제공하는 API는 물리 엔진간 변하지 않음.

