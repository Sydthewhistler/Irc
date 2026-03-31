#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string &name)
	: _name(name), _topic(""), _key(""), _userLimit(0),
	  _inviteOnly(false), _topicRestricted(false),
	  _hasKey(false), _hasUserLimit(false)
{
}

Channel::Channel(const Channel &src)
	: _name(src._name), _topic(src._topic), _key(src._key),
	  _userLimit(src._userLimit), _inviteOnly(src._inviteOnly),
	  _topicRestricted(src._topicRestricted), _hasKey(src._hasKey),
	  _hasUserLimit(src._hasUserLimit), _members(src._members),
	  _operators(src._operators), _invited(src._invited)
{
}

Channel &Channel::operator=(const Channel &rhs)
{
	if (this != &rhs)
	{
		_name = rhs._name;
		_topic = rhs._topic;
		_key = rhs._key;
		_userLimit = rhs._userLimit;
		_inviteOnly = rhs._inviteOnly;
		_topicRestricted = rhs._topicRestricted;
		_hasKey = rhs._hasKey;
		_hasUserLimit = rhs._hasUserLimit;
		_members = rhs._members;
		_operators = rhs._operators;
		_invited = rhs._invited;
	}
	return *this;
}

Channel::~Channel(void) {}

const std::string				&Channel::getName(void) const { return _name; }
const std::string				&Channel::getTopic(void) const { return _topic; }
const std::string				&Channel::getKey(void) const { return _key; }
size_t							Channel::getUserLimit(void) const { return _userLimit; }
const std::vector<Client *>		&Channel::getMembers(void) const { return _members; }

bool	Channel::isInviteOnly(void) const { return _inviteOnly; }
bool	Channel::isTopicRestricted(void) const { return _topicRestricted; }
bool	Channel::hasKey(void) const { return _hasKey; }
bool	Channel::hasUserLimit(void) const { return _hasUserLimit; }

void	Channel::setInviteOnly(bool flag) { _inviteOnly = flag; }
void	Channel::setTopicRestricted(bool flag) { _topicRestricted = flag; }
void	Channel::setTopic(const std::string &topic) { _topic = topic; }

void	Channel::setKey(const std::string &key) { _key = key; _hasKey = true; }
void	Channel::removeKey(void) { _key = ""; _hasKey = false; }
void	Channel::setUserLimit(size_t limit) { _userLimit = limit; _hasUserLimit = true; }
void	Channel::removeUserLimit(void) { _userLimit = 0; _hasUserLimit = false; }

void	Channel::addMember(Client *client)
{
	if (!isMember(client))
		_members.push_back(client);
}

void	Channel::removeMember(Client *client)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == client)
		{
			_members.erase(_members.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
		{
			_operators.erase(_operators.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _invited.size(); i++)
	{
		if (_invited[i] == client)
		{
			_invited.erase(_invited.begin() + i);
			break;
		}
	}
}

bool	Channel::isMember(Client *client) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == client)
			return true;
	}
	return false;
}

bool	Channel::isEmpty(void) const
{
	if (_members.size() > 0)
		return false;
	return true;
}

void	Channel::addOperator(Client *client)
{
	if (!isOperator(client))
		_operators.push_back(client);
}

void	Channel::removeOperator(Client *client)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
		{
			_operators.erase(_operators.begin() + i);
			break;
		}
	}
}

bool	Channel::isOperator(Client *client) const
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
			return true;
	}
	return false;
}

void	Channel::addInvited(Client *client)
{
	if (!isInvited(client))
		_invited.push_back(client);
}

bool	Channel::isInvited(Client *client) const
{
	for (size_t i = 0; i < _invited.size(); i++)
	{
		if (_invited[i] == client)
			return true;
	}
	return false;
}

void	Channel::removeInvited(Client *client)
{
	for (size_t i = 0; i < _invited.size(); i++)
	{
		if (_invited[i] == client)
		{
			_invited.erase(_invited.begin() + i);
			break;
		}
	}
}

void	Channel::broadcast(const std::string &message, Client *except)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] != except)
			_members[i]->appendSendBuffer(message + "\r\n");
	}
}
