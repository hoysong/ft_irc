#include "Channel.hpp"

void channel::addMember( Client &client )
{
}
void Channel::removeMember( Client &client )
{
}

bool Channel::isChannelEmpty( void )
{
	return (m_members.empty());
}

// ======================================================================
// constructor/destructor.
// ======================================================================

Channel::Channel( const std::string &str ) :
	m_channelName(str)
{
}

Channel::~Channel( void )
{
}
