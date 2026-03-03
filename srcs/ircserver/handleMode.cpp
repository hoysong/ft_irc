#include "IRCServer.hpp"
#include "ircError.hpp"
#include "Msg.hpp"
#include <vector>

//| 441 | `ERR_USERNOTINCHANNEL` | `<nick> <channel> :They aren't on that channel` |
//| 461 | `ERR_NEEDMOREPARAMS` | `MODE :Not enough parameters` |
//| 467 | `ERR_KEYSET` | `<channel> :Channel key already set` |
//| 472 | `ERR_UNKNOWNMODE` | `<char> :is unknown mode char to me for <channel>` |
//| 477 | `ERR_NOCHANMODES` | `<channel> :Channel doesn't support modes` |
//| 482 | `ERR_CHANOPRIVSNEEDED` | `<channel> :You're not channel operator` |

//#### 정상 응답 (조회 시)
//| 324 | `RPL_CHANNELMODEIS` | `<channel> <mode> <mode params>` |
//| 329 | `RPL_CREATIONTIME` | `<channel> <creation timestamp>` |
//| 367 | `RPL_BANLIST` | `<channel> <banmask>` |
//| 368 | `RPL_ENDOFBANLIST` | `<channel> :End of channel ban list` |
//
//클라이언트 요청:
//MODE hoysong +i
//서버의 성공 응답 (에코):
//:hoysong!user@host MODE hoysong :+i
void IRCServer::setChannelMode(Client &client,
		const std::string &target,
		const std::string &modes,
		std::vector<std::string> &modeTargets)
{
//	C → S (요청): MODE #ft_irc
//	S → C (응답): :irc.local 324 hoysong #ft_irc +kl secret_key 50
	Channel *channel;
	if (!m_channelManager.getChannel(target, channel))
	{
//		sendMsg(client.getFd(), errMsg( ERR_NOSUCHCHANNEL, client,
//					target, "No such channel"));
		Msg().errNoSuchChannel(client.getNickName(), target).sendTo(client.getFd());
		return ;
	}
	if (modes.empty())
	{ // 모드 조회
//		sendMsg(client.getFd(), rplChannelMode(client, *channel));
		Msg()
			.setPrefix(SERVER_PREFIX)
			.numeric(RPL_CHANNELMODEIS)
			.addParam(client.getNickName())
			.addParam(channel->getChannelName())
			.addParam(channel->modeToString())
			.trailing(false)
			.sendTo(client.getFd());
		return ;
	}
	else
	{ // 모드 적용
		if(!channel->findMember(client))
		{ // 멤버가 아님.
//			sendMsg(client.getFd(), errMsg(ERR_NOTONCHANNEL, client,
//						target, "You're not on that channel"));
			Msg()
				.setPrefix(SERVER_PREFIX)
				.numeric(ERR_NOTONCHANNEL)
				.addParam(client.getNickName())
				.addParam(target)
				.addParam("You're not on that channel")
				.sendTo(client.getFd());
			return ;
		}
		else if (!channel->isChannelOper(client))
		{ // 오퍼가 아님.
//			sendMsg(client.getFd(), errMsg(ERR_CHANOPRIVSNEEDED, client,
//						target, "You're not channel operator"));
			Msg().errChanOpPrivsNeeded(client.getNickName(), target).sendTo(client.getFd());
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
						iter++;
					}
					else if (channel->isKeyMode() && add && (channel->getPasswd() != *iter))
					{ // 비밀번호 변경 기존과 다른 경우에만 수행.
						channel->setKeyMode(*iter);
						trailingBuffer += "k";
						params.push_back(*iter);
						iter++;
					}
					else if (channel->isKeyMode() && channel->getPasswd() == *iter && remove)
					{ // 비밀번호 삭제
						channel->setKeyMode("");
						trailingBuffer += "k";
						params.push_back(*iter);
						iter++;
					}
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
						iter++;
					}
					else if (channel->isChannelOper(*iter) && remove)
					{
						channel->addChannelOper(*iter);
						trailingBuffer += "o";
						params.push_back(*iter);
						iter++;
					}
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
					else if (channel->isLimitMode() && remove)
					{
						channel->setLimitMode("-1");
						trailingBuffer += "l";
						iter++;
					}
				}
			}
		}
		if (!trailingBuffer.empty())
		{ // 변경 완료 메시지.
			for(std::vector<std::string>::iterator iter = params.begin(); iter != params.end(); iter++)
				trailingBuffer += " " + *iter;
			channel->broadcastMsg(
					Msg()
					.setPrefix(client.getMsgPrefix())
					.addParam(client.getNickName())
					.addParam(target)
					.addParam(trailingBuffer)
					.trailing(false)
					.serialize()
				);
		}
	}
}

void IRCServer::setCliientMode(Client &client,
		const std::string &target,
		const std::string &modes,
		std::vector<std::string> &modeTargets)
{
	if (modes.empty())
	{ // 모드 조회 요청.
		if (target != client.getNickName())
		{
//			sendMsg(client.getFd(), errMsg(ERR_USERSDONTMATCH, client,
//						target,
//						"Cannot change mode for other user"));
			Msg()
				.setPrefix(SERVER_PREFIX)
				.addParam(client.getNickName())
				.addParam(target)
				.addParam("Cannot change mode for other user")
				.sendTo(client.getFd());
		}
		else
//			sendMsg(client.getFd(), rplUserMode(client));
			{
				std::string trailing = ":+";
				if (client.isInvisible())
					trailing += 'i';
				if (client.isWallopos())
					trailing += 'w';
				Msg()
					.setPrefix(SERVER_PREFIX)
					.numeric(RPL_UMODEIS)
					.addParam(client.getNickName())
					.addParam(trailing)
					.sendTo(client.getFd());
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
//		sendMsg(client.getFd(), goodMsg(client, "MODE", client.getNickName(), trailingBuffer));
		Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam("MODE")
			.addParam(client.getNickName())
			.addParam(trailingBuffer)
			.trailing(false)
			.sendTo(client.getFd());
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
//		sendMsg(client.getFd(), notEnoughParam(client, "MODE"));
		Msg().errNotEnoughParam(client.getNickName(), "MODE").sendTo(client.getFd());
		return ;
	}
	modeProcess(client, target, modes, modeTargets);
}
