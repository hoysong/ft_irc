#ifndef IRCERROR_HPP
# define IRCERROR_HPP

enum e_ircError {
	// 400 ~ 410번대: 대상(Target) 관련 에러
	ERR_NOSUCHNICK = 401,		// 지정한 닉네임이나 채널이 존재하지 않음
	ERR_NOSUCHSERVER = 402,		// 지정한 서버가 존재하지 않음
	ERR_NOSUCHCHANNEL = 403,	// 지정한 채널이 존재하지 않음
	ERR_CANNOTSENDTOCHAN = 404,	// 채널에 메시지를 보낼 권한이 없음 (예: 외부 발송 금지, 밴 등)
	ERR_TOOMANYCHANNELS = 405,	// 클라이언트가 가입할 수 있는 최대 채널 수를 초과함
	ERR_WASNOSUCHNICK = 406,	// 이전에 존재했던 닉네임이지만 현재는 없음 (WHOWAS 용)
	ERR_TOOMANYTARGETS = 407,	// 명령어에 지정된 대상(Target)이 너무 많음
	ERR_NOSUCHSERVICE = 408,	// 지정한 서비스가 존재하지 않음
	ERR_NOORIGIN = 409,		// PING/PONG 명령에 출발지(Origin)가 없음

	// 411 ~ 415번대: 메시지 전송 관련 에러
	ERR_NORECIPIENT = 411,		// 수신자(Recipient)가 지정되지 않음
	ERR_NOTEXTTOSEND = 412,		// 전송할 메시지 본문이 없음
	ERR_NOTOPLEVEL = 413,		// 최상위 도메인(Top-level) 마스크 없이 메시지 전송 시도
	ERR_WILDTOPLEVEL = 414,		// 최상위 도메인에 와일드카드 사용 불가
	ERR_BADMASK = 415,		// 잘못된 서버/호스트 마스크 형식

	// 421 ~ 424번대: 명령어/서버 관련 에러
	ERR_UNKNOWNCOMMAND = 421,	// 알 수 없는(미지원) 명령어가 입력됨
	ERR_NOMOTD = 422,		// 서버에 MOTD(접속 환영 메시지) 파일이 없음
	ERR_NOADMININFO = 423,		// 서버 관리자 정보(ADMIN)를 찾을 수 없음
	ERR_FILEERROR = 424,		// 파일 작업(읽기/쓰기) 중 에러 발생

	// 431 ~ 436번대: 닉네임 관련 에러
	ERR_NONICKNAMEGIVEN = 431,	// NICK 명령에 닉네임이 지정되지 않음
	ERR_ERRONEUSNICKNAME = 432,	// 닉네임 형식이 잘못됨 (허용되지 않는 문자 포함 등)
	ERR_NICKNAMEINUSE = 433,	// 지정한 닉네임이 이미 사용 중
	ERR_NICKCOLLISION = 436,	// 서버 간 닉네임 충돌 발생
	ERR_UNAVAILRESOURCE = 437,	// 닉네임 또는 채널이 일시적으로 사용 불가 (예: 닉네임 딜레이)

	// 441 ~ 446번대: 채널 멤버십 관련 에러
	ERR_USERNOTINCHANNEL = 441,	// 지정한 유저가 해당 채널에 없음
	ERR_NOTONCHANNEL = 442,		// 명령을 수행하려면 해당 채널에 있어야 함
	ERR_USERONCHANNEL = 443,	// INVITE 시 대상 유저가 이미 채널에 있음
	ERR_NOLOGIN = 444,		// SUMMON 대상 유저가 로그인되어 있지 않음
	ERR_SUMMONFWD = 445,		// SUMMON 명령이 이 서버에서 지원되지 않음
	ERR_USERSDISABLED = 446,	// USERS 명령이 비활성화되어 있음

	// 451번대: 등록(Registration) 관련 에러
	ERR_NOTREGISTERED = 451,	// 아직 서버에 등록(PASS/NICK/USER)이 완료되지 않음

	// 461 ~ 467번대: 명령 파라미터 및 인증 관련 에러
	ERR_NEEDMOREPARAMS = 461,	// 명령어에 필요한 파라미터가 부족함
	ERR_ALREADYREGISTRED = 462,	// 이미 등록된 클라이언트가 PASS 등을 재시도함
	ERR_NOPERMFORHOST = 463,	// 해당 호스트에서는 서버 접속 권한이 없음
	ERR_PASSWDMISMATCH = 464,	// 서버 접속 비밀번호가 틀림
	ERR_YOUREBANNEDCREEP = 465,	// 해당 클라이언트/호스트는 서버에서 밴(ban)됨
	ERR_YOUWILLBEBANNED = 466,	// 곧 밴될 예정임을 알리는 경고
	ERR_KEYSET = 467,		// 채널 키(Password)가 이미 설정되어 있음

	// 471 ~ 478번대: 채널 모드 및 입장 제한 관련 에러
	ERR_CHANNELISFULL = 471,	// 채널 인원이 최대치(+l 모드)에 도달하여 입장 불가
	ERR_UNKNOWNMODE = 472,		// 알 수 없는 채널 모드 플래그가 사용됨
	ERR_INVITEONLYCHAN = 473,	// 초대 전용 채널(+i)에 초대 없이 입장 시도
	ERR_BANNEDFROMCHAN = 474,	// 밴 목록(+b)에 등록된 유저가 채널 입장 시도
	ERR_BADCHANNELKEY = 475,	// 채널 키(비밀번호)가 틀림
	ERR_BADCHANMASK = 476,		// 잘못된 채널 마스크 형식
	ERR_NOCHANMODES = 477,		// 해당 채널은 모드 설정을 지원하지 않음
	ERR_BANLISTFULL = 478,		// 밴 목록이 가득 찼음

	// 481 ~ 485번대: 권한(Privilege) 관련 에러
	ERR_NOPRIVILEGES = 481,		// IRC 오퍼레이터(OPER) 권한이 필요한 작업을 일반 유저가 시도
	ERR_CHANOPRIVSNEEDED = 482,	// 채널 오퍼레이터(@) 권한이 필요함
	ERR_CANTKILLSERVER = 483,	// KILL 명령으로 서버 자체를 종료할 수 없음
	ERR_RESTRICTED = 484,		// 연결이 제한된 상태여서 해당 권한을 사용할 수 없음
	ERR_UNIQOPPRIVSNEEDED = 485,	// 채널의 설립자(Founder) 또는 특정 권한이 필요함

	ERR_NOOPERHOST = 491,		// 클라이언트의 호스트는 서버 관리자(OPER)가 될 수 없도록 설정됨

	// 500번대 이상: 유저 모드 관련 에러
	ERR_UMODEUNKNOWNFLAG = 501,	// 알 수 없는 유저 모드 플래그가 요청됨
	ERR_USERSDONTMATCH = 502	// 다른 유저의 모드를 변경하거나 조회할 권한이 없음
};

enum e_rpl_numeric {
	/* Registration / connection */
	RPL_WELCOME = 1,	// 001
	RPL_YOURHOST = 2,	// 002
	RPL_CREATED = 3,	// 003
	RPL_MYINFO = 4,	// 004

	/* Channel / mode / topic */
	RPL_CHANNELMODEIS = 324,	// 324
	RPL_NOTOPIC = 331,	// 331
	RPL_TOPIC = 332,	// 332

	/* Invite */
	RPL_INVITING = 341,	// 341

	/* Names list (commonly sent after JOIN or NAMES) */
	RPL_NAMREPLY = 353,	// 353
	RPL_ENDOFNAMES = 366	// 366
};

#endif
