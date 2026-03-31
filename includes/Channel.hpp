#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <map>

class Client;

/*
** Représente un channel IRC avec ses modes et ses membres.
**
** Modes à implémenter:
**   i - invite-only
**   t - topic restreint aux opérateurs
**   k - channel protégé par mot de passe (key)
**   o - privilège opérateur (par utilisateur)
**   l - limite d'utilisateurs
*/

class Channel
{
public:
	Channel(const std::string &name);
	Channel(const Channel &src);
	Channel &operator=(const Channel &rhs);
	~Channel(void);

	// --- Getters ---
	const std::string				&getName(void) const;
	const std::string				&getTopic(void) const;
	const std::string				&getKey(void) const;
	size_t							getUserLimit(void) const;
	const std::vector<Client *>		&getMembers(void) const;

	// --- Modes ---
	bool	isInviteOnly(void) const;
	bool	isTopicRestricted(void) const;
	bool	hasKey(void) const;
	bool	hasUserLimit(void) const;

	void	setInviteOnly(bool flag);
	void	setTopicRestricted(bool flag);
	void	setKey(const std::string &key);
	void	removeKey(void);
	void	setUserLimit(size_t limit);
	void	removeUserLimit(void);
	void	setTopic(const std::string &topic);

	// --- Gestion des membres ---
	void	addMember(Client *client);
	void	removeMember(Client *client);
	bool	isMember(Client *client) const;
	bool	isEmpty(void) const;

	// --- Opérateurs ---
	void	addOperator(Client *client);
	void	removeOperator(Client *client);
	bool	isOperator(Client *client) const;

	// --- Invitations ---
	void	addInvited(Client *client);
	bool	isInvited(Client *client) const;
	void	removeInvited(Client *client);

	// --- Broadcast ---
	void	broadcast(const std::string &message, Client *except);

private:
	Channel(void);

	std::string				_name;
	std::string				_topic;
	std::string				_key;
	size_t					_userLimit;

	bool					_inviteOnly;
	bool					_topicRestricted;
	bool					_hasKey;
	bool					_hasUserLimit;

	std::vector<Client *>	_members;
	std::vector<Client *>	_operators;
	std::vector<Client *>	_invited;
};

#endif
