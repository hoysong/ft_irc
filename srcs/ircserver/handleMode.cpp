#include "IRCServer.hpp"
#include "ircError.hpp"
#include "Msg.hpp"
#include <vector>

void IRCServer::setChannelMode(Client &client,
		const std::string &target,
		const std::string &modes,
		std::vector<std::string> &modeTargets)
{
	Channel *channel;
	if (!m_channelManager.getChannel(target, channel))
	{
		Msg().errNoSuchChannel(client.getNickName(), target).sendTo(client, *this);
		return ;
	}
	if (modes.empty())
	{ // 모드 조회
		Msg().rplChannelModeIs(client.getNickName(), channel->getChannelName(), channel->modeToString()).sendTo(client, *this);
		return ;
	}
	if(!channel->findMember(client))
	{ // 멤버가 아님.
		Msg().errNotOnChannel(client.getNickName(), target).sendTo(client, *this);
		return ;
	}
	else if (!channel->isChannelOper(client))
	{ // 오퍼가 아님.
		Msg().errChanOpPrivsNeeded(client.getNickName(), target).sendTo(client, *this);
		return ;
	}
	bool add = false;
	bool remove = false;
	std::vector<std::string>::iterator iter = modeTargets.begin();
	std::vector<std::string>::iterator iter_end = modeTargets.end();
	std::string trailingBuffer;
	std::vector<std::string> params;
	for(size_t i = 0; modes[i] != '\0'; i++)
	{
		if (modes[i] == '+' || modes[i] == '-')
		{
			if (modes[i] == '+')
			{
				add = true;
				remove = false;
			}
			else if (modes[i] == '-')
			{
				add = false;
				remove = true;
			}
			if (trailingBuffer.empty())
				trailingBuffer += modes[i];
			else if (!trailingBuffer.empty())
			{
				if (*(trailingBuffer.rbegin()) == '-' || *(trailingBuffer.rbegin()) == '+')
					*(trailingBuffer.rbegin()) = modes[i];
				else
					trailingBuffer += modes[i];
			}
		}
		else if (modes[i] == 'i')
		{
			if (!channel->isInviteMode() && add)
			{
				channel->setInviteMode(true);
				trailingBuffer += "i";
			}
			else if (channel->isInviteMode() && remove)
			{
				channel->setInviteMode(false);
				trailingBuffer += "i";
			}
		}
		else if (modes[i] == 't')
		{
			if (!channel->isTopicMode() && add)
			{
				channel->setTopicMode(true);
				trailingBuffer += "t";
			}
			else if (channel->isTopicMode() && remove)
			{
				channel->setTopicMode(false);
				trailingBuffer += "t";
			}
		}
		else if (modes[i] == 'k')
		{
			if (iter != iter_end)
			{
				if (!channel->isKeyMode() && add)
				{ // 비밀번호 설정
					channel->setKeyMode(*iter);
					trailingBuffer += "k";
					params.push_back(*iter);
				}
				else if (channel->isKeyMode() && add && (channel->getPasswd() != *iter))
				{ // 비밀번호 변경 기존과 다른 경우에만 수행.
					channel->setKeyMode(*iter);
					trailingBuffer += "k";
					params.push_back(*iter);
				}
				else if (channel->isKeyMode() && channel->getPasswd() == *iter && remove)
				{ // 비밀번호 삭제
					channel->setKeyMode("");
					trailingBuffer += "k";
					params.push_back(*iter);
				}
				iter++;
			}
		}
		else if (modes[i] == 'o')
		{
			if (iter != iter_end && channel->findMember(*iter))
			{ // 멤버 있으면 실행.
				if (!channel->isChannelOper(*iter) && add)
				{
					channel->addChannelOper(*iter);
					trailingBuffer += "o";
					params.push_back(*iter);
				}
				else if (channel->isChannelOper(*iter) && remove)
				{
					channel->removeChannelOper(*iter);
					trailingBuffer += "o";
					params.push_back(*iter);
				}
				iter++;
			}
		}
		else if (modes[i] == 'l')
		{
			if (iter != iter_end)
			{
				if (!channel->isLimitMode() && add)
				{
					if (channel->setLimitMode(*iter))
					{
						trailingBuffer += "l";
						iter++;
						params.push_back(channel->getStringChannelLimit());
					}
				}
			}
			else if (channel->isLimitMode() && remove)
			{
				channel->setLimitMode("-1");
				trailingBuffer += "l";
				iter++;
			}
		}
	}
	channel->modeToString();
	if (!trailingBuffer.empty())
	{ // 변경 완료 메시지.
		for(std::vector<std::string>::iterator iter = params.begin(); iter != params.end(); iter++)
			trailingBuffer += " " + *iter;
		channel->broadcastMsg(
				Msg()
				.setPrefix(client.getMsgPrefix())
				.addParam("MODE")
				.addParam(target)
				.addParam(trailingBuffer)
				.trailing(false)
				.serialize()
			);
	}
}

void IRCServer::setCliientMode(Client &client,
		const std::string &target,
		const std::string &modes,
		std::vector<std::string> &modeTargets)
{
	if (modes.empty())
	{ // 모드 조회 요청.

		if (!m_clientManager.isNickExists(target))
		{
			Msg()
				.setPrefix(SERVER_PREFIX)
				.numeric(401)
				.addParam(client.getNickName())
				.addParam(target)
				.addParam("No such nick")
				.sendTo(client, *this);
		}
		else if (target != client.getNickName())
		{
			Msg()
				.setPrefix(SERVER_PREFIX)
				.numeric(502)
				.addParam(client.getNickName())
				.addParam("Cannot change mode for other user")
				.sendTo(client, *this);
		}
		else
			{
				std::string trailing = "+";
				if (client.isInvisible())
					trailing += 'i';
				if (client.isWallopos())
					trailing += 'w';
				Msg()
					.setPrefix(SERVER_PREFIX)
					.numeric(RPL_UMODEIS)
					.addParam(client.getNickName())
					.addParam(trailing)
					.sendTo(client, *this);
			}
		return ;
	}

	bool add = false;
	bool remove = false;
	std::vector<std::string>::iterator iter = modeTargets.begin();
	std::vector<std::string>::iterator iter_end = modeTargets.end();
	std::string trailingBuffer;
	for(size_t i = 0; modes[i] != '\0'; i++)
	{
		if (modes[i] == '+')
		{
			add = true;
			remove = false;
		}
		else if (modes[i] == '-')
		{
			add = false;
			remove = true;
		}
		else if (modes[i] == 'i')
		{
			if (!client.isInvisible() && add)
			{
				client.setInvisible(true);
				trailingBuffer += "+i";
			}
			else if (client.isInvisible() && remove)
			{
				client.setInvisible(false);
				trailingBuffer += "-i";
			}
		}
		else if (modes[i] == 'w')
		{
			if (!client.isWallopos() && add)
			{
				client.setWallops(true);
				trailingBuffer += "+w";
			}
			else if (client.isWallopos() && remove)
			{
				client.setWallops(false);
				trailingBuffer += "-w";
			}
		}
	}
	if (!trailingBuffer.empty())
	{ // 변경 완료 메시지.
		Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam("MODE")
			.addParam(client.getNickName())
			.addParam(trailingBuffer)
			.trailing(false)
			.sendTo(client, *this);
	}
}
void IRCServer::modeProcess(Client &client,
		const std::string &target,
		const std::string &modes,
		std::vector<std::string> &modeTargets)
{
	if (target[0] == '#')
	{ // 채널모드
		setChannelMode(client, target, modes, modeTargets);
	}
	else
	{ // 유저모드
		setCliientMode(client, target, modes, modeTargets);
	}
}

void    IRCServer::handleMode(Client& client, const paramVector& params)
{
	/* 파라미터 따로 취급해야 함. */
	std::string target; // channelName or userName
	std::string modes; // modes (+lko)
	paramVector modeTargets; // users
	paramVector::const_iterator iter = params.begin();
	if (iter != params.end())
	{
		target = *iter;
		iter++;
	}
	if (iter != params.end())
	{
		modes = *iter;
		iter++;
	}
	if (iter != params.end())
	{
		for(; iter != params.end(); iter++)
			modeTargets.push_back(*iter);
	}
	if (target.empty())
	{ // 타겟 비었으면 파라미터가 없음.
		Msg().errNotEnoughParam(client.getNickName(), "MODE").sendTo(client, *this);
		return ;
	}
	modeProcess(client, target, modes, modeTargets);
}
