#ifndef ISERVERCONTROLLER_HPP
# define ISERVERCONTROLLER_HPP

class Client;

class IServerController
{
	public:
		virtual ~IServerController( void ) {/*destructor.*/};
		virtual void hardDisconnect( Client &client ) = 0;
		virtual void softDisconnect( Client &client ) = 0;
		virtual void addClientToRemove( Client &client ) = 0;
};

#endif
