#ifndef STATEMACHINE_HPP
# define STATEMACHINE_HPP

class Client; // 전방선언.

class StateMachine
{
	protected:
	public:
		virtual ~StateMachine( void );
		virtual void handleBuffer( Client &client, bool &loop ) = 0;
};

class RegisteringState : public StateMachine
{
	protected:
	public:
		~RegisteringState( void );
		void handleBuffer( Client &client, bool &loop );
		/* 상태전이용 함수. */
		static RegisteringState &getRegisteringState( void );
		bool getOneLine( std::string &oneLineRef, std::string &buffRef);
		bool passAuthentication(std::string &oneLine, Client &client, bool &loop );
};

#endif
