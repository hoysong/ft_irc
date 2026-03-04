#include "IRCServer.hpp"
#include "Msg.hpp"

extern void splitMultiTarget(std::string str , std::vector<std::string> &vect);

void IRCServer::kickProcess(Client &client, std::vector<std::string> &channels, std::vector<std::string> &targets, const std::string &reason)
{
	std::vector<std::string>::iterator channelIter = channels.begin();
	std::vector<std::string>::iterator channelIterEnd = channels.end();
	std::vector<std::string>::iterator targetlIter = targets.begin();
	std::vector<std::string>::iterator targetlIterEnd = targets.end();
	std::string msg;
	std::string trailing;

	/* trailing 설정. */
	if (reason.empty())
		trailing = client.getNickName(); // 비었으면 오퍼닉.
	else
		trailing = reason; // 있으면 그대로.

	if (channels.size() == 1)
		while (targetlIter != targetlIterEnd)
		{
			msg = Msg()
				.setPrefix(client.getMsgPrefix())
				.addParam("KICK")
				.addParam(*channelIter)
				.addParam(*targetlIter)
				.addParam(trailing)
				.serialize();
			m_channelManager.kickClientFromChannel(client, *channelIter, *targetlIter, msg);
			targetlIter++;
		}
	else
		while (channelIter != channelIterEnd)
		{
			msg = Msg()
				.setPrefix(client.getMsgPrefix())
				.addParam("KICK")
				.addParam(*channelIter)
				.addParam(*targetlIter)
				.addParam(trailing)
				.serialize();
			m_channelManager.kickClientFromChannel(client, *channelIter, *targetlIter, msg);
			channelIter++;
			targetlIter++;
		}
}
void    IRCServer::handleKick(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
	}
	
	/*서버 멀티타겟 스플릿.*/
	std::vector<std::string> channels;
	splitMultiTarget(params[0], channels);
	if (channels.empty())
	{
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
		return ;
	}

	/*유저 멀티타겟 스플릿.*/
	std::vector<std::string> clients;
	splitMultiTarget(params[1], clients);
	if (clients.empty())
	{
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
		return ;
	}

	if (channels.size() != 1 && (channels.size() != clients.size()))
	{ // 1대1 매칭인 경우 파라미터 에러 반환
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
		return ;
	}
	if (params.size() < 3)
		kickProcess(client, channels, clients, "");
	else
		kickProcess(client, channels, clients, params[2]);
}

