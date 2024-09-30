Gazebo는 2가지 타입의 정보를 기록할 수 있음

1. Console Message
   - GUI 에서가 아닌, 서버 프로세스로 부터의 콘솔 메시지
   - `server_console.log` 파일로 로그됨
   - 항상 기록됨
2. Simulation state
   - 엔티티의 poses, 삽입과 제거
   - `Gazebo Transport state.tlog` 파일로 로그됨
   - 기록은 반드시 command line 혹은 C++ API를 통해서 가능함
   - command line 혹은 C++ API를 사용하여 Play back 할 수 있음.
  
## How to record ?

### record using Command Line

`--record` 옵션을 사용하여 예제를 실행시키자. (기본 저장 경로는 `~/.gz/sim/log/<timestamp>` 이다.)

```

gz sim -v 4 -r --record pose_publisher.sdf
gz sim -v 4 -r --record-path <path> pose_publisher.sdf

```

record 와 관련한 다른 옵션들은 아래와 같다.

- `--record-resources` : states 뿐만 아니라 모델과 텍스쳐 또한 기록할 수 있음. `--record` 옵션은 기본으로 적용되어있음
- `--log-overwrite` : 만약에, record path가 이미 존재한다면 덮어 쓰는 옵션. 기본은 `false`가 옵션이며 그럴 경우에, 덮어쓴 파일들은 뒤에 숫자가 붙어있음. (예 /tmp/log(1), /tmp/log(2))
- `--log-compress` : 기록한 파일을 압축함

### record using C++ API

```
gz::sim::ServerConfig serverConfig;
serverConfig.SetUseLogRecord(true);
serverConfig.SetLogRecordPath("custom_path");
 
gz::sim::Server server(serverConfig);
```

### record using plugin in SDF

`<world>` 태그 아래에 플러그인을 추가하여 사용할 수 있다.

현재는 Gazebo가 실행되는 동안, 오직 하나의 LogRecord 인스턴스만 사용할 수 있음

```
<world name="default">
    ...
    <plugin
      filename="gz-sim-log-system"
      name="gz::sim::systems::LogRecord">
    </plugin>
    ...
</world>

```

### Record Path

최종적으로 기록된 파일 경로는 옵션에 따라 달라진다.

- 만약 record와 관련된 어떤 옵션이 없다면, console log는 자동으로 `/.gz/sim/log/<timestamp>`에 기록된다.
- `--record`만 있다면, 모든 기록 파일은 `/.gz/sim/log/<timestamp>`에 기록된다.
- `--record-path`가 특정되어 있다면,
  - 만약 해당 경로가 없는 경우, 해당 경로가 만들어지며 저장된다.
  - 만약 해당 경로가 있을 경우
    - `--log-overwrite`가 없을 경우, 위에서 설명한 바와 같이 숫자가 붙어 파일이 저장되고, 옵션이 있는 경우 해당 경로 내부가 삭제되고 log가 기록된다.
   
## How to Playback?

### From Command Line

Playback의 경우, 커맨드 라인에서 `--playback` 옵션과 `<path>`를 통해 특정 파일을 지정할 수 있다.

```
gz sim -r -v 4 --playback <path>
```

현재 Playback과 Record를 동시에 하는것은 허용되지 않음. (개발중)
