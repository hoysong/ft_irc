#include "Client.hpp"
#include "StateMachine.hpp"
#include "MyLibft.hpp"
#include <iostream>

/* StateMachine. */
StateMachine::~StateMachine( void )
{
}

/**********************/
/* class RegistState. */
/**********************/

RegisteringState::~RegisteringState( void )
{
}

bool RegisteringState::getOneLine( std::string &oneLineRef, std::string &buffRef )
{
	size_t crlfPos = buffRef.find("\r\n");
	if ( crlfPos == std::string::npos )
	{ /* 아직 라인이 완성되지 않음. */
		return (false);
	}
	oneLineRef = buffRef.substr(0, crlfPos);
	buffRef.erase(0, crlfPos + 2);
	return (true);
}

bool RegisteringState::passAuthentication(std::string &oneLine, Client &client, bool &loop )
{
	/* if pass fail?
	 * 	loop = false;
	 * 	client.stateSet(false);
	 * 	return false;
	 */
	size_t seperatorPos = oneLine.find(' ');
	if ( seperatorPos != std::string::npos )
	{
	}
	else
	{
	}
	return (true);
}

void RegisteringState::handleBuffer( Client &client, bool &loop)
{
	std::cout << "\t\t[RegistState::handleBuffer]" << std::endl;

	std::string oneLine;
	std::string command;
	std::string argument;

//	if (!getOneLine(oneLine, client.takeBuffer()))
	{
		loop = false;
		return ;
	}
	if (!passAuthentication(oneLine, client, loop))
	{
	}
}
/* 상태전이용 함수. */
RegisteringState &RegisteringState::getRegisteringState( void )
{
	static RegisteringState instance;
	return (instance);
}

/**********************/
/* class RegistState. */
/**********************/
