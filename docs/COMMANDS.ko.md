# IRC 명령어 사용법

이 문서는 ft_irc 서버가 지원하는 IRC 명령어의 사용 형식을 설명합니다.

- 명령어는 대소문자를 구별하지 않습니다.
- 공백을 포함하는 마지막 매개변수 앞에는 `:`을 붙입니다.
- 등록을 마치기 전에는 `PASS`, `NICK`, `USER`, `QUIT` 외의 명령을 사용할 수 없습니다.

[한국어 README로 돌아가기](../README.ko.md)

## 1. 기능 미리보기

### 1.1. 연결 및 등록

- **PASS** — 등록 전에 연결 비밀번호 설정
- **NICK** — 사용자의 닉네임 설정 또는 변경
- **USER** — 등록 과정에서 사용자 이름과 실제 이름 설정

### 1.2. 메시지

- **PRIVMSG** — 사용자 또는 채널에 메시지 전송
- **NOTICE** — 사용자 또는 채널에 알림 전송(자동 응답 없음)

### 1.3. 채널 작업

- **JOIN** — 하나 이상의 채널에 입장
- **PART** — 하나 이상의 채널에서 퇴장
- **TOPIC** — 채널 주제 조회 또는 변경
- **INVITE** — 사용자를 채널에 초대
- **KICK** — 사용자를 채널에서 내보내기

### 1.4. 채널 모드(MODE)

| 모드 | 매개변수  | 설명                                 |
| ---- | --------- | ------------------------------------ |
| `i`  | 없음      | 초대 전용 채널 설정/해제             |
| `t`  | 없음      | TOPIC 변경 권한을 채널 운영자로 제한 |
| `k`  | `<key>`   | 채널 키(비밀번호) 설정/해제          |
| `o`  | `<nick>`  | 채널 운영자 권한 부여/회수           |
| `l`  | `<limit>` | 채널의 최대 사용자 수 설정/해제      |

### 1.5. 서버 유틸리티

- **PING** — 연결 유지
- **QUIT** — 서버 연결 종료

## 2. 기능 상세 설명

### 2.1. 연결 및 등록

#### PASS

등록 전에 서버의 연결 비밀번호를 제출합니다.

```text
PASS <server_password>
```

#### NICK

닉네임을 설정하거나 변경합니다.

```text
NICK <nickname>
```

#### USER

등록 과정에서 사용자 이름과 실제 이름을 설정합니다.

```text
USER <username> <mode> <unused> :<realname>
```

### 2.2. 메시지

#### PRIVMSG

사용자 또는 채널에 메시지를 보냅니다. 쉼표로 구분하여 한 번에 최대 4개의 대상을 지정할 수 있습니다.

```text
PRIVMSG <nickname> :<message>
PRIVMSG #<channel> :<message>
PRIVMSG <target1>,<target2> :<message>
```

#### NOTICE

사용자 또는 채널에 알림을 보냅니다. 대상이나 메시지가 잘못되어도 서버가 오류를 자동으로 응답하지 않습니다.

```text
NOTICE <nickname> :<message>
NOTICE #<channel> :<message>
```

### 2.3. 채널 작업

#### JOIN

하나 이상의 채널에 입장합니다. 채널에 키가 설정되어 있다면 채널과 같은 순서로 키를 전달합니다. `JOIN 0`은 참여 중인 모든 채널에서 퇴장합니다.

```text
JOIN #<channel>
JOIN #<channel> <key>
JOIN #<channel1>,#<channel2> <key1>,<key2>
JOIN 0
```

#### PART

하나 이상의 채널에서 퇴장하며, 선택적으로 퇴장 메시지를 전달할 수 있습니다.

```text
PART #<channel>
PART #<channel1>,#<channel2>
PART #<channel> :<message>
```

#### TOPIC

채널 주제를 조회하거나 변경합니다. 빈 주제를 전달하면 기존 주제를 삭제합니다.

```text
TOPIC #<channel>
TOPIC #<channel> :<topic>
TOPIC #<channel> :
```

#### INVITE

사용자를 채널에 초대합니다.

```text
INVITE <nickname> #<channel>
```

#### KICK

채널에서 사용자를 내보냅니다. 하나의 채널에서 여러 사용자를 내보내거나, 같은 수의 채널과 사용자를 순서대로 대응시킬 수 있습니다.

```text
KICK #<channel> <nickname>
KICK #<channel> <nickname> :<reason>
KICK #<channel> <nick1>,<nick2> :<reason>
KICK #<channel1>,#<channel2> <nick1>,<nick2> :<reason>
```

### 2.4. 채널 모드(MODE)

`MODE`에 채널 이름만 전달하면 현재 모드를 조회합니다. 모드 변경은 해당 채널의 운영자만 할 수 있습니다.

```text
MODE #<channel>
MODE #<channel> +i
MODE #<channel> -i
MODE #<channel> +t
MODE #<channel> -t
MODE #<channel> +k <key>
MODE #<channel> -k <key>
MODE #<channel> +o <nickname>
MODE #<channel> -o <nickname>
MODE #<channel> +l <limit>
MODE #<channel> -l
```

여러 모드를 한 명령으로 함께 변경할 수 있습니다. 매개변수가 필요한 모드는 모드 문자열에 적힌 순서대로 값을 전달합니다.

```text
MODE #<channel> +it
MODE #<channel> +kol <key> <nickname> <limit>
```

### 2.5. 서버 유틸리티

#### PING

연결 상태를 확인합니다. 서버는 전달받은 토큰을 포함한 `PONG`으로 응답합니다.

```text
PING <token>
```

#### QUIT

서버와의 연결을 종료합니다. 종료 사유는 같은 채널에 참여 중인 사용자에게 전달됩니다.

```text
QUIT
QUIT :<reason>
```
