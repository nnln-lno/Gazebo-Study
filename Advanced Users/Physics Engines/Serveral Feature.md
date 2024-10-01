물리 엔진의 몇가지 특징들을 알아보자

## FeatureList Definitions

이 FeatureList 목록은 Dartsim과 TPE-Plugin에 구현되어 있다.

사용자는 자신의 플러그인 구현을 굳이 이처럼 할 필요는 없다.

![Screenshot from 2024-10-01 20-04-57](https://github.com/user-attachments/assets/76dcef78-cba1-42b8-8957-626347e47541)

## Dart vs TPE

- DART (Dynamic Animation and Toolkit)

로봇 공학 및 컴퓨터 애니메이션에서 운동학 및 동적 응용을 위한 데이터 구조와 알고리즘을 제공하는 오픈 소스 라이브러리.

Gazebo 시뮬레이션에서 사용되는 기본 물리엔진이기도 하며, 소스코드는 Gazebo Physics의 dartsim 디렉토리에서 찾을 수 있다.

- TPE (Trivial Physics Engine)

Open Robotics에서 만든 오픈 소스 라이브러리도, 대규모 엔티티의 빠르고 가벼운 운동학 시뮬레이션을 가능하게 함.

실제 물리적 요소(예 : 중력, 힘, 제약 등) 없이 고차원 함대 동역학화 다중 기계 동기화를 지원한다.

Gazebo가 지원하는 TPE는 Gazebo Citadel 이후 릴리스에서 사용 가능하며, Gazebo Physics의 tpe/plugin 디렉토리에서 소스를 찾을 수 있다.

---

각 물리엔진이 지원하는 기능 목록은 다음과 같다.

- Entity Comparison
  
![Screenshot from 2024-10-01 20-13-42](https://github.com/user-attachments/assets/30a35bb1-8e7b-4b8c-a181-319c5aa9501d)

- Feature Comparison
  
![Screenshot from 2024-10-01 20-14-48](https://github.com/user-attachments/assets/85e2f792-1546-44a3-94d8-8e6d9f77901a)
  


