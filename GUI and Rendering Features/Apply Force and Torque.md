`Apply Force Torque` 플러그인은 말 그대로 Force와 Torque를 `Link`에 적용시킬 수 있는 플러그인이다.

다음의 예제를 실행시켜 연습해보자.

```
gz sim shapes.sdf
```

우측 상단의 리스트에서, `Apply Force Torque` 플러그인을 선택하자. 

`Apply Force Torque` 플러그인에서 Force 와 Torque를 적용하기 위한 모델을 선택하게 되면, `Link` 탭에 해당 모델의 링크정보가 나타난다.

이후 각 축에 대한 값들을 설정하고 `Apply Force/Torque` 버튼을 눌러 물체가 움직이는지 확인한다.

<strong> 만약 물체가 움직이지 않는다면, 시뮬레이션이 `Play`되어있는지 확인하거나 너무 작은값이 입력되지 않았는지 확인한다. </strong>

<strong> [ 일시정지 된 상태에서 지속적으로 Force와 Torque를 적용하면, `Play`시에 누른만큼 중첩되어 한번에 적용되는점 참고 ] </strong>

각 요소들에 대해 정리해보면 아래와 같다.

## Force

- <strong>Force X,Y,Z</strong> : 얼마 만큼의 Force를 특정 축에 적용할지, N(Newton) 단위로 입력한다. 이때 Frame은 해당 `Link`를 기준으로 한다.
- <strong>Mag</strong> : 전체 Force의 크기를 나타내며, 세 축의 Force에 대한 "Euclidean norm" 값이다.
- <strong>Offset X,Y,Z</strong> : 기본값은 `Link`의 "Center of Mass" 이다. 이 때 단위는 m(Meter). 이 질량 중심을 기준으로 얼마만큼 Offset 시킨 곳에 힘을 가할지를 정하는 요소
  * `Select Mode`를 누르고, 모델 선택 후 우클릭을 하면 `View->Center of Mass` 옵션이 있고, 잘 보이게 하고 싶으면 `View->Transparent` 옵션 또한 골라주면 된다.
- <strong>Apply Force</strong> : 입력한 설정에 따른 Force를 "one time step" 동안 적용시킨다. 이 time step은 일만적으로 millisecond 이므로 Force 값은 변화를 보기 위해서는 상대적으로 큰 값을 입력해야 한다.

## Torque

- <strong>Force X,Y,Z</strong> : 얼마 만큼의 Torque를 특정 축에 적용할지, N(Newton) 단위로 입력한다. 이때 Frame은 해당 `Link`를 기준으로 한다.
- <strong>Mag</strong> : 전체 Torque의 크기를 나타내며, 세 축의 Torque에 대한 "Euclidean norm" 값이다.
- <strong>Apply Force</strong> : 입력한 설정에 따른 Torque를 "one time step" 동안 적용시킨다. 이 time step은 일만적으로 millisecond 이므로 Torque 값은 변화를 보기 위해서는 상대적으로 큰 값을 입력해야 한다.
  * `Torque`는 항상 `Center of Mass`를 기준으로 적용된다.

참고로 Force, Torque를 나타내는 벡터를 선택하면, 물체의 회전 없이 원하는 방향 및 오프셋으로 Force, Torque 벡터를 변경할 수 있지만

모델을 선택하여 Rotation Tool로 회전시킬 경우, 모델의 Frame에 맞게 Force, Torque 벡터가 자동으로 변경되는 점을 유의하자.
 
## Apply All

말 그대로 Force 와 Torque를 각각이 아니라, 한번에 모두 적용시키는 기능이다.
