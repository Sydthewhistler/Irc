#include "Channel.hpp"
#include "Client.hpp"

// Forme canonique (partagé)

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

// Getters simples (partagé)

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

/*
** TODO: Personne B — Gestion des membres
** addMember: ajouter si pas déjà membre
** removeMember: retirer + retirer des opérateurs et invités aussi
** isMember: vérifier si le client est dans _members
** isEmpty: vérifier si _members est vide
*/
void	Channel::addMember(Client *client)
{
	(void)client;
	// TODO: Personne B
}

void	Channel::removeMember(Client *client)
{
	(void)client;
	// TODO: Personne B
}

bool	Channel::isMember(Client *client) const
{
	(void)client;
	// TODO: Personne B
	return false;
}

bool	Channel::isEmpty(void) const
{
	// TODO: Personne B
	return true;
}

/*
** TODO: Personne B — Gestion des opérateurs
** addOperator: ajouter si pas déjà opérateur
** removeOperator: retirer de la liste
** isOperator: vérifier si le client est opérateur
*/
void	Channel::addOperator(Client *client)
{
	(void)client;
	// TODO: Personne B
}

void	Channel::removeOperator(Client *client)
{
	(void)client;
	// TODO: Personne B
}

bool	Channel::isOperator(Client *client) const
{
	(void)client;
	// TODO: Personne B
	return false;
}

/*
** TODO: Personne B — Gestion des invitations
** addInvited: ajouter si pas déjà invité
** isInvited: vérifier si le client a été invité
** removeInvited: retirer de la liste
*/
void	Channel::addInvited(Client *client)
{
	(void)client;
	// TODO: Personne B
}

bool	Channel::isInvited(Client *client) const
{
	(void)client;
	// TODO: Personne B
	return false;
}

void	Channel::removeInvited(Client *client)
{
	(void)client;
	// TODO: Personne B
}

/*
** TODO: Personne B — Broadcast
** Envoie un message à tous les membres du channel SAUF 'except'.
** Utiliser client->appendSendBuffer(message + "\r\n") pour chaque membre.
*/
void	Channel::broadcast(const std::string &message, Client *except)
{
	(void)message; (void)except;
	// TODO: Personne B
}
