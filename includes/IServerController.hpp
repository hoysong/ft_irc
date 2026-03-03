#ifndef ISERVERCONTROLLER_HPP
# define ISERVERCONTROLLER_HPP
# include <string>

class Client;

class IServerController
{
	public:
		virtual ~IServerController( void ) {/*destructor.*/};
		virtual void hardDisconnect( Client &client, const std::string &msg ) = 0;
		virtual void softDisconnect( Client &client, const std::string &msg ) = 0;
};

#endif
