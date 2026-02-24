#ifndef MODEMASK_HPP
# define MODEMASK_HPP

// UserMode 비트 정의
enum UserMode {
	UMODE_WALLOPS   = 1 << 1,  // +w
	UMODE_INVISIBLE = 1 << 2 // +i
};

// 비트마스크 타입 (원하는 정수형 사용)
typedef unsigned char ModeMask;

// 헬퍼 함수들
inline void setMode(ModeMask &m, UserMode flag) {
	m |= flag;
}

inline void clearMode(ModeMask &m, UserMode flag) {
	m &= ~flag;
}

inline bool hasMode(ModeMask m, UserMode flag) {
	return (m & flag) != 0;
}

inline void toggleMode(ModeMask &m, UserMode flag) {
	m ^= flag;
}


#endif
