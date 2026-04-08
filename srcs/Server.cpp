#include "Replies.hpp"
#include "Server.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

const std::string Server::SERVER_NAME = "ft_irc";
bool Server::running = false;

// ========================================
// Constructeur / Destructeur
// ========================================

Server::Server(int port, const std::string &password) : _port(port),
	_password(password), _serverFd(-1)
{
	_initSocket();
	_initSignals();
}

/*
** TODO: Personne A
** Destructeur: fermer tous les fds clients (close + delete),
** delete tous les channels, fermer _serverFd.
*/
Server::~Server(void)
{
	for (std::map<int,
		Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	// ferme/supprime tous les client
	{
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	for (std::map<std::string,
		Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		// supprime tous les channels
		delete it->second;
	_channels.clear();
	// 3. Fermer le serverFd
	if (_serverFd >= 0)
		close(_serverFd);
	std::cout << "[LOG] Serveur arrêté proprement" << std::endl;
}

// ========================================
// Utilitaires partagées
// (Implémentées ensemble, utilisées par les deux)
// ========================================

void Server::sendToClient(Client *client, const std::string &message)
{
	client->appendSendBuffer(message + "\r\n");
}

void Server::sendReply(Client *client, const std::string &code,
	const std::string &params)
{
	sendToClient(client, ":ft_irc " + code + " " + client->getNickname() + " "
		+ params);
}

Client *Server::getClientByNick(const std::string &nickname)
{
	for (std::map<int,
		Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return (it->second);
	}
	return (NULL);
}

Channel *Server::getChannel(const std::string &name)
{
	for (std::map<std::string,
		Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->second->getName() == name)
			return (it->second);
	}
	return (NULL);
}

Channel *Server::createChannel(const std::string &name)
{
	Channel	*ptr;

	ptr = new Channel(name);
	_channels[name] = ptr;
	return (ptr);
}

void Server::removeChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it != _channels.end())
	{
		delete (it->second);
		_channels.erase(it);
	}
}

// ========================================
// Réseau — Personne A
// ========================================

/*
** TODO: Personne A
** _initSocket:
**   1. socket(AF_INET, SOCK_STREAM, 0)
**   2. setsockopt SO_REUSEADDR
**   3. bind() sur _port
**   4. listen()
**   5. fcntl(_serverFd, F_SETFL, O_NONBLOCK)
**   6. Ajouter _serverFd au vecteur _pollFds avec POLLIN
*/
void Server::_initSocket(void)
{
	int					opt;
	struct sockaddr_in	addr;
	struct pollfd		pfd;

	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
	{
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error setting SO_REUSEADDR: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(_serverFd, SOMAXCONN) < 0)
	{
		std::cerr << "Error listening: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	pfd.fd = _serverFd;
	pfd.events = POLLIN; // surveille les nouvelles connexions
	pfd.revents = 0;
	_pollFds.push_back(pfd);
	Server::running = true;
}

void Server::_handleSigint(int sig)
{
	(void)sig;
	std::cout << "Server shutting down..." << std::endl;
	Server::running = false;
	std::exit(0);
}

void Server::_initSignals()
{
	std::signal(SIGINT, Server::_handleSigint);
	std::signal(SIGPIPE, SIG_IGN);
}

/*
** TODO: Personne A
** run: boucle principale
**   2. while (running):
**		a. poll(_pollFds, timeout)
**		b. Parcourir les fds:
**		- _serverFd + POLLIN  -> _acceptClient()
**		- client fd + POLLIN  -> _receiveData(fd)
**		- client fd + POLLOUT + sendBuffer non vide -> _sendData(fd)
**		- POLLERR / POLLHUP   -> _disconnectClient(fd)
*/
void Server::run(void)
{
	int	ret;

	while (Server::running)
	{
		ret = poll(_pollFds.data(), _pollFds.size(), -1);
		if (ret < 0)
		{
			std::cerr << "Error in poll: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].fd == _serverFd && _pollFds[i].revents & POLLIN)
				// nouvelle connexion
				_acceptClient();
			else if (_pollFds[i].revents & POLLIN) // données à recevoir
			{
				_receiveData(_pollFds[i].fd);
			}
			else if (_pollFds[i].revents & POLLOUT)
			// prêt à envoyer des données
			{
				_sendData(_pollFds[i].fd);
			}
			else if (_pollFds[i].revents & (POLLERR | POLLHUP))
			// erreur ou déconnexion
			{
				_disconnectClient(_pollFds[i].fd);
				i--;
			}
		}
	}
}
/*
** TODO: Personne A
** _acceptClient:
**   1. accept() sur _serverFd
**   2. fcntl(new_fd, F_SETFL, O_NONBLOCK)
**   3. new Client(new_fd), l'ajouter à _clients[new_fd]
**   4. Ajouter new_fd à _pollFds (POLLIN | POLLOUT)
**   5. Récupérer le hostname du client avec getaddrinfo/inet_ntop
*/
void Server::_acceptClient(void)
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen;
	int					clientFd;
	char				hostname[INET_ADDRSTRLEN];
	Client				*newClient;
	struct pollfd		pfd;

	clientLen = sizeof(clientAddr);
	clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd < 0)
	{
		std::cerr << "Error accepting client: " << strerror(errno) << std::endl;
		return ;
	}
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	inet_ntop(AF_INET, &clientAddr.sin_addr, hostname, sizeof(hostname));
	newClient = new Client(clientFd);
	_clients[clientFd] = newClient;
	pfd.fd = clientFd;
	pfd.events = POLLIN | POLLOUT;
	// surveille données à recevoir et possibilité d'envoyer
	pfd.revents = 0;
	_pollFds.push_back(pfd);
	std::cout << "[LOG] New client connected: " << hostname << " (fd=" << clientFd << ")" << std::endl;
}

/*
** TODO: Personne A
** _receiveData:
**   1. char buf[4096]; recv(fd, buf, sizeof(buf), 0)
**   2. Si recv <= 0 -> _disconnectClient(fd)
**   3. Sinon client->appendRecvBuffer(buf) puis _processLines(client)
*/
void Server::_receiveData(int fd)
{
	char	buf[4096];
	int		bytesRead;
	Client	*client;

	memset(buf, 0, sizeof(buf));
	bytesRead = recv(fd, buf, sizeof(buf) - 1, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0) // Client déconnecté proprement
			std::cout << "[LOG] Client fd=" << fd << " properly disconnected" << std::endl;
		else // Erreur de recv
			std::cerr << "[LOG] Erreur recv() fd=" << fd << ": " << strerror(errno) << std::endl;
		_disconnectClient(fd);
		return ;
	}
	client = _clients[fd];
	client->appendRecvBuffer(std::string(buf, bytesRead));
	_processLines(client);
}

/*
** TODO: Personne A
** _sendData:
**   1. Récupérer client->getSendBuffer()
**   2. send(fd, buffer.c_str(), buffer.size(), 0)
**   3. Si send < buffer.size() -> garder le reste dans le buffer
**   4. Si tout envoyé -> clearSendBuffer()
*/
void Server::_sendData(int fd)
{
	Client	*client;
	int		bytes;

	if (_clients.find(fd) == _clients.end())
		return ;
	client = _clients[fd];
	const std::string &buf = client->getSendBuffer();
	if (buf.empty())
		return ;
	bytes = send(fd, buf.c_str(), buf.size(), 0);
	if (bytes < 0)
	{
		std::cerr << "[LOG] Erreur send() fd=" << fd << ": " << strerror(errno) << std::endl;
		_disconnectClient(fd);
		return ;
	}
	else if (bytes < static_cast<int>(buf.size()))
	{
		std::string reste = buf.substr(bytes);
		client->clearSendBuffer();
		client->appendSendBuffer(reste);
	}
	else
		client->clearSendBuffer();
}

/*
** TODO: Personne A
** _disconnectClient:
**   1. Retirer le client de tous les channels (parcourir _channels)
**   2. close(fd)
**   3. Retirer de _pollFds
**   4. delete _clients[fd] et retirer de _clients
*/
void Server::_disconnectClient(int fd)
{
	Client	*client;

	if (_clients.find(fd) == _clients.end())
		return ;
	client = _clients[fd];
	std::map<std::string, Channel *>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		it->second->removeMember(client);
		if (it->second->isEmpty())
		{
			delete it->second;
			_channels.erase(it++);
		}
		else
			++it;
	}
	close(fd);
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break ;
		}
	}
	std::cout << "[LOG] Client déconnecté fd=" << fd << std::endl;
	delete	client;
	_clients.erase(fd);
}

/*
** TODO: Personne A
** _processLines:
**   Tant que client->extractLine(line) retourne true:
**		- Si line est vide, continuer
**		- Message msg = Message::parse(line)
**		- Si msg.command non vide -> _executeCommand(client, msg)
*/
void Server::_processLines(Client *client)
{
	Message	msg;

	std::string line;
	while (client->extractLine(line))
	{
		if (line.empty())
			continue ;
		msg = Message::parse(line);
		if (!msg.command.empty())
			_executeCommand(client, msg);
	}
}

// ========================================
// Dispatch — Partagé
// ========================================

void Server::_executeCommand(Client *client, const Message &msg)
{
	if (msg.command == "CAP")
		return ;
	if (msg.command == "PASS")
		return (_handlePass(client, msg));
	if (msg.command == "NICK")
		return (_handleNick(client, msg));
	if (msg.command == "USER")
		return (_handleUser(client, msg));
	if (msg.command == "QUIT")
		return (_handleQuit(client, msg));
	if (msg.command == "PING")
		return (_handlePing(client, msg));
	if (msg.command == "PONG")
		return (_handlePong(client, msg));
	if (!client->isRegistered())
	{
		sendReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}
	if (msg.command == "JOIN")
		return (_handleJoin(client, msg));
	if (msg.command == "PART")
		return (_handlePart(client, msg));
	if (msg.command == "PRIVMSG")
		return (_handlePrivmsg(client, msg));
	if (msg.command == "NOTICE")
		return (_handleNotice(client, msg));
	if (msg.command == "KICK")
		return (_handleKick(client, msg));
	if (msg.command == "INVITE")
		return (_handleInvite(client, msg));
	if (msg.command == "TOPIC")
		return (_handleTopic(client, msg));
	if (msg.command == "MODE")
		return (_handleMode(client, msg));
	if (msg.command == "WHO")
		return (_handleWho(client, msg));
	sendReply(client, ERR_UNKNOWNCOMMAND, msg.command + ":Unknown command");
}

// ========================================
// Handlers — Personne B
// ========================================

void Server::_handlePass(Client *client, const Message &msg)
{
	if (msg.params.empty())
	{
		sendReply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		return ;
	}
	if (client->isRegistered())
	{
		sendReply(client, ERR_ALREADYREGISTERED,
			"PASS :You may not reregister");
		return ;
	}
	if (_password != msg.params[0])
	{
		sendReply(client, ERR_PASSWDMISMATCH, "PASS :Password incorrect");
		return ;
	}
	client->setPassValidated(true);
}

bool	nick_is_valid(const std::string &nickname)
{
	if (nickname.empty())
		return (false);
	if (!isalpha(nickname[0])
		&& std::string("[]\\`^{}_").find(nickname[0]) == std::string::npos)
		return (false);
	for (size_t i = 1; i < nickname.size(); i++)
	{
		if (!isalnum(nickname[i]) && (nickname[i] != '-')
			&& (std::string("[]\\`^{}_").find(nickname[i]) == std::string::npos))
			return (false);
	}
	return (true);
}

void Server::_sendWelcome(Client *client)
{
	sendReply(client, RPL_WELCOME, "Welcome to the IRC Network "
		+ client->getPrefix());
	sendReply(client, RPL_YOURHOST, "Your host is ft_irc, running version 1.0");
	sendReply(client, RPL_CREATED, "This server was created <date>");
	sendReply(client, RPL_MYINFO, "ft_irc 1.0 o itkol");
}

void Server::_handleNick(Client *client, const Message &msg)
{
	Channel	*channel;
	bool	wasRegistered;

	if (msg.params.empty())
	{
		sendReply(client, ERR_NONICKNAMEGIVEN, "NICK :No nickname given");
		return ;
	}
	if (!nick_is_valid(msg.params[0]))
	{
		sendReply(client, ERR_ERRONEUSNICKNAME, msg.params[0]
			+ ":Erroneous nickname");
		return ;
	}
	if (getClientByNick(msg.params[0])
		&& (getClientByNick(msg.params[0]) != client))
	{
		sendReply(client, ERR_NICKNAMEINUSE, msg.params[0]
			+ ":Nickname is already in use");
		return ;
	}
	if (client->isRegistered())
	{
		for (std::map<std::string,
			Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			channel = it->second;
			if (channel->isMember(client))
				channel->broadcast(":" + client->getNickname() + " NICK "
					+ msg.params[0], client);
		}
		sendToClient(client, ":" + client->getNickname() + " NICK "
			+ msg.params[0]);
	}
	wasRegistered = client->isRegistered();
	client->setNickname(msg.params[0]);
	if (!wasRegistered && client->isRegistered())
		_sendWelcome(client);
}

void Server::_handleUser(Client *client, const Message &msg)
{
	bool	wasRegistered;

	if (msg.params.size() != 4)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return ;
	}
	if (client->isRegistered())
	{
		sendReply(client, ERR_ALREADYREGISTERED,
			"USER :You may not reregister");
		return ;
	}
	wasRegistered = client->isRegistered();
	client->setUsername(msg.params[0]);
	client->setRealname(msg.params[3]);
	if (!wasRegistered && client->isRegistered())
		_sendWelcome(client);
}

void Server::_handleQuit(Client *client, const Message &msg)
{
	std::string reason = msg.params.empty() ? "Quit" : msg.params[0];
	for (std::map<std::string,
		Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->second->isMember(client))
		{
			it->second->broadcast(":" + client->getPrefix() + " QUIT :"
				+ reason, client);
		}
	}
	_disconnectClient(client->getFd());
}

void Server::_handleJoin(Client *client, const Message &msg)
{
	Channel	*chan;

	if (msg.params.empty())
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ ":Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[0]);
	if (!chan)
	{
		chan = createChannel(msg.params[0]);
		chan->addOperator(client);
	}
	if (chan->isMember(client))
		return ;
	if (chan->isInviteOnly() && !chan->isInvited(client))
	{
		sendReply(client, ERR_INVITEONLYCHAN, msg.params[0]
			+ ":Cannot join channel (+i)");
		return ;
	}
	if (chan->hasKey())
	{
		if (msg.params.size() < 2 || msg.params[1] != chan->getKey())
		{
			sendReply(client, ERR_BADCHANNELKEY, msg.params[0]
				+ ":Cannot join channel (+k)");
			return ;
		}
	}
	if (chan->hasUserLimit()
		&& (chan->getMembers().size() >= chan->getUserLimit()))
	{
		sendReply(client, ERR_CHANNELISFULL, msg.params[0]
			+ ":Cannot join channel (+l)");
		return ;
	}
	chan->addMember(client);
	chan->removeInvited(client);
	chan->broadcast(":" + client->getPrefix() + " JOIN " + msg.params[0], NULL);
	if (chan->getTopic().empty())
		sendReply(client, RPL_NOTOPIC, msg.params[0] + " :No topic is set");
	else
		sendReply(client, RPL_TOPIC, msg.params[0] + " :" + chan->getTopic());
	std::string list;
	const std::vector<Client *> &members = chan->getMembers();
	for (size_t i = 0; i < members.size(); i++)
	{
		if (chan->isOperator(members[i]))
			list += "@";
		list += members[i]->getNickname();
		if (i + 1 < members.size())
			list += " ";
	}
	sendReply(client, RPL_NAMREPLY, "= " + msg.params[0] + " :" + list);
	sendReply(client, RPL_ENDOFNAMES, msg.params[0] + " :End of /NAMES list");
}

void Server::_handlePart(Client *client, const Message &msg)
{
	Channel	*chan;

	if (msg.params.empty())
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ ":Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[0]);
	if (!chan)
	{
		sendReply(client, ERR_NOSUCHCHANNEL, msg.params[0]
			+ " :No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendReply(client, ERR_NOTONCHANNEL, msg.params[0]
			+ " :You're not on that channel");
		return ;
	}
	std::string reason = msg.params.size() > 1 ? " :" + msg.params[1] : "";
	chan->broadcast(":" + client->getPrefix() + " PART " + msg.params[0]
		+ reason, NULL);
	chan->removeMember(client);
	if (chan->isEmpty())
		removeChannel(msg.params[0]);
}

void Server::_handlePrivmsg(Client *client, const Message &msg)
{
	Client	*targetClient;

	if (msg.params.size() < 2)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	const std::string &target = msg.params[0];
	const std::string &text = msg.params[1];
	if (!target.empty() && target[0] == '#')
	{
		std::map<std::string, Channel *>::iterator it = _channels.find(target);
		if (it != _channels.end())
		{
			if (it->second->isMember(client))
				it->second->broadcast(":" + client->getPrefix() + " PRIVMSG "
					+ target + " :" + text, client);
			else
				sendReply(client, ERR_CANNOTSENDTOCHAN, target
					+ " :Cannot send to channel");
		}
		else
			sendReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
	}
	else
	{
		targetClient = getClientByNick(target);
		if (targetClient)
		{
			std::string reply = ":" + client->getPrefix() + " PRIVMSG " + target
				+ " :" + text;
			sendToClient(targetClient, reply);
		}
		else
			sendReply(client, ERR_NOSUCHNICK, target + " :No such nick");
	}
}

void Server::_handleNotice(Client *client, const Message &msg)
{
	Client	*targetClient;

	if (msg.params.size() < 2)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	const std::string &target = msg.params[0];
	const std::string &text = msg.params[1];
	if (!target.empty() && target[0] == '#')
	{
		std::map<std::string, Channel *>::iterator it = _channels.find(target);
		if (it != _channels.end())
		{
			if (it->second->isMember(client))
				it->second->broadcast(":" + client->getPrefix() + " NOTICE "
					+ target + " :" + text, client);
			else
				return ;
		}
		else
			return ;
	}
	else
	{
		targetClient = getClientByNick(target);
		if (targetClient)
		{
			std::string reply = ":" + client->getPrefix() + " NOTICE " + target
				+ " :" + text;
			sendToClient(targetClient, reply);
		}
		else
			return ;
	}
}

void Server::_handleKick(Client *client, const Message &msg)
{
	Channel	*chan;
	Client	*target;

	if (msg.params.size() < 2)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[0]);
	if (!chan)
	{
		sendReply(client, ERR_NOSUCHCHANNEL, msg.params[0]
			+ " :No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendReply(client, ERR_NOTONCHANNEL, msg.params[0]
			+ " :You're not on that channel");
		return ;
	}
	if (!chan->isOperator(client))
	{
		sendReply(client, ERR_CHANOPRIVSNEEDED, msg.params[0]
			+ " :You're not channel operator");
		return ;
	}
	target = getClientByNick(msg.params[1]);
	if (!target)
	{
		sendReply(client, ERR_NOSUCHNICK, msg.params[1] + " :No such nick");
		return ;
	}
	if (!chan->isMember(target))
	{
		sendReply(client, ERR_USERNOTINCHANNEL, msg.params[1] + " "
			+ chan->getName() + " :They aren't on that channel");
		return ;
	}
	std::string reason = msg.params.size() > 2 ? " :" + msg.params[2] : "";
	chan->broadcast(":" + client->getPrefix() + " KICK " + msg.params[0] + " "
		+ msg.params[1] + reason, NULL);
	chan->removeMember(target);
}

void Server::_handleInvite(Client *client, const Message &msg)
{
	Channel	*chan;
	Client	*target;

	if (msg.params.size() < 2)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[1]);
	if (!chan)
	{
		sendReply(client, ERR_NOSUCHCHANNEL, msg.params[1]
			+ " :No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendReply(client, ERR_NOTONCHANNEL, msg.params[1]
			+ " :You're not on that channel");
		return ;
	}
	if (chan->isInviteOnly() && !chan->isOperator(client))
	{
		sendReply(client, ERR_CHANOPRIVSNEEDED, msg.params[1]
			+ " :You're not channel operator");
		return ;
	}
	target = getClientByNick(msg.params[0]);
	if (!target)
	{
		sendReply(client, ERR_NOSUCHNICK, msg.params[0] + " :No such nick");
		return ;
	}
	if (chan->isMember(target))
	{
		sendReply(client, ERR_USERONCHANNEL, msg.params[0] + " "
			+ chan->getName() + " :is already on channel");
		return ;
	}
	chan->addInvited(target);
	sendReply(client, RPL_INVITING, chan->getName() + " " + msg.params[0]);
	sendToClient(target, ":" + client->getPrefix() + " INVITE " + msg.params[0]
		+ " " + msg.params[1]);
}

void Server::_handleTopic(Client *client, const Message &msg)
{
	Channel	*chan;

	if (msg.params.size() < 1)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[0]);
	if (!chan)
	{
		sendReply(client, ERR_NOSUCHCHANNEL, msg.params[0]
			+ " :No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendReply(client, ERR_NOTONCHANNEL, msg.params[0]
			+ " :You're not on that channel");
		return ;
	}
	if (msg.params.size() == 1)
	{
		if (!chan->getTopic().empty())
			sendReply(client, RPL_TOPIC, msg.params[0] + " :"
				+ chan->getTopic());
		else
			sendReply(client, RPL_NOTOPIC, msg.params[0] + " :No topic is set");
		return ;
	}
	if (chan->isTopicRestricted() && !chan->isOperator(client))
	{
		sendReply(client, ERR_CHANOPRIVSNEEDED, msg.params[0]
			+ " :You're not channel operator");
		return ;
	}
	chan->setTopic(msg.params[1]);
	chan->broadcast(":" + client->getPrefix() + " TOPIC " + msg.params[0] + " :"
		+ msg.params[1], NULL);
}

void Server::_applyModes(Channel *chan, Client *client, const Message &msg)
{
	bool	adding;
	size_t	argIndex;
	char	c;
	Client	*target;

	std::string modeStr = msg.params[1];
	adding = true;
	argIndex = 2;
	for (size_t i = 0; i < modeStr.size(); i++)
	{
		c = modeStr[i];
		if (c == '+')
			adding = true;
		else if (c == '-')
			adding = false;
		else if (c == 'i')
			chan->setInviteOnly(adding);
		else if (c == 't')
			chan->setTopicRestricted(adding);
		else if (c == 'k')
		{
			if (adding)
			{
				if (argIndex < msg.params.size())
					chan->setKey(msg.params[argIndex++]);
			}
			else
				chan->removeKey();
		}
		else if (c == 'o')
		{
			if (argIndex < msg.params.size())
			{
				target = getClientByNick(msg.params[argIndex++]);
				if (target && chan->isMember(target))
				{
					if (adding)
						chan->addOperator(target);
					else
						chan->removeOperator(target);
				}
			}
		}
		else if (c == 'l')
		{
			if (adding)
			{
				if (argIndex < msg.params.size())
					chan->setUserLimit(std::atoi(msg.params[argIndex++].c_str()));
			}
			else
				chan->removeUserLimit();
		}
	}
	std::string fullMode = msg.params[1];
	for (size_t j = 2; j < msg.params.size(); j++)
		fullMode += " " + msg.params[j];
	chan->broadcast(":" + client->getPrefix() + " MODE " + msg.params[0] + " "
		+ fullMode, NULL);
}

void Server::_showChannelModes(Channel *chan, Client *client,
	const std::string &chanName)
{
	std::string modes = "+";
	std::string params = "";
	if (chan->isInviteOnly())
		modes += "i";
	if (chan->isTopicRestricted())
		modes += "t";
	if (chan->hasKey())
	{
		modes += "k";
		params += " " + chan->getKey();
	}
	if (chan->hasUserLimit())
	{
		modes += "l";
		std::ostringstream oss;
		oss << chan->getUserLimit();
		params += " " + oss.str();
	}
	sendReply(client, RPL_CHANNELMODEIS, chanName + " " + modes + params);
}

void Server::_handleMode(Client *client, const Message &msg)
{
	Channel	*chan;

	if (msg.params.empty())
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[0]);
	if (!chan)
	{
		sendReply(client, ERR_NOSUCHCHANNEL, msg.params[0]
			+ " :No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendReply(client, ERR_NOTONCHANNEL, msg.params[0]
			+ " :You're not on that channel");
		return ;
	}
	if (msg.params.size() == 1)
		return (_showChannelModes(chan, client, msg.params[0]));
	if (!chan->isOperator(client))
	{
		sendReply(client, ERR_CHANOPRIVSNEEDED, msg.params[0]
			+ " :You're not channel operator");
		return ;
	}
	_applyModes(chan, client, msg);
}

void Server::_handlePing(Client *client, const Message &msg)
{
	if (!msg.params.empty())
		sendToClient(client, ":ft_irc PONG ft_irc :" + msg.params[0]);
}

void Server::_handlePong(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

void Server::_handleWho(Client *client, const Message &msg)
{
	Channel	*chan;

	if (msg.params.empty())
	{
		sendReply(client, ERR_NEEDMOREPARAMS, msg.command
			+ " :Not enough parameters");
		return ;
	}
	chan = getChannel(msg.params[0]);
	if (!chan)
	{
		sendReply(client, ERR_NOSUCHCHANNEL, msg.params[0]
			+ " :No such channel");
		return ;
	}
	if (!chan->isMember(client))
	{
		sendReply(client, ERR_NOTONCHANNEL, msg.params[0]
			+ " :You're not on that channel");
		return ;
	}
	const std::vector<Client *> &members = chan->getMembers();
	for (size_t i = 0; i < members.size(); i++)
	{
		std::string flag = chan->isOperator(members[i]) ? "H@" : "H";
		sendReply(client, RPL_WHOREPLY, msg.params[0] + " "
			+ members[i]->getUsername() + " " + members[i]->getHostname()
			+ " ft_irc " + members[i]->getNickname() + " " + flag + " :0 "
			+ members[i]->getRealname());
	}
	sendReply(client, RPL_ENDOFWHO, msg.params[0] + " :End of /WHO list");
}
