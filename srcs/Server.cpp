#include "Replies.hpp"
#include "Server.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

const std::string Server::SERVER_NAME = "ft_irc";

// ========================================
// Constructeur / Destructeur
// ========================================

Server::Server(int port, const std::string &password) : _port(port),
	_password(password), _serverFd(-1)
{
	_initSocket();
}

/*
** TODO: Personne A
** Destructeur: fermer tous les fds clients (close + delete),
** delete tous les channels, fermer _serverFd.
*/
Server::~Server(void)
{
	// TODO: Personne A
}

// ========================================
// Utilitaires partagées
// (Implémentées ensemble, utilisées par les deux)
// ========================================

/*
** TODO: Partagé
** sendToClient: ajoute message + "\r\n" dans le sendBuffer du client.
** La boucle poll() de Personne A se chargera d'envoyer.
*/
void Server::sendToClient(Client *client, const std::string &message)
{
	(void)client;
	(void)message;
	// TODO
}

/*
** TODO: Partagé
** sendReply: formate ":ft_irc <code> <nickname> <params>"
** et appelle sendToClient.
*/
void Server::sendReply(Client *client, const std::string &code,
	const std::string &params)
{
	(void)client;
	(void)code;
	(void)params;
	// TODO
}

/*
** TODO: Partagé
** getClientByNick: parcourt _clients, retourne le Client*
** dont le nickname correspond. NULL si pas trouvé.
*/
Client *Server::getClientByNick(const std::string &nickname)
{
	(void)nickname;
	// TODO
	return (NULL);
}

/*
** TODO: Partagé
** getChannel: cherche dans _channels par nom. NULL si pas trouvé.
*/
Channel *Server::getChannel(const std::string &name)
{
	(void)name;
	// TODO
	return (NULL);
}

/*
** TODO: Partagé
** createChannel: new Channel(name), l'ajouter à _channels,
	retourner le pointeur.
*/
Channel *Server::createChannel(const std::string &name)
{
	(void)name;
	// TODO
	return (NULL);
}

/*
** TODO: Partagé
** removeChannel: delete le channel et le retirer de _channels.
*/
void Server::removeChannel(const std::string &name)
{
	(void)name;
	// TODO
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
}

/*
** TODO: Personne A
** run: boucle principale
**   1. Installer signal handlers (SIGINT -> arrêt propre, SIGPIPE -> SIG_IGN)
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
	// TODO: Personne A
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
	(void)fd;
	// TODO: Personne A
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
	(void)fd;
	// TODO: Personne A
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
	(void)fd;
	// TODO: Personne A
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
	(void)client;
	// TODO: Personne A
}

// ========================================
// Dispatch — Partagé
// ========================================

/*
** TODO: Partagé
** _executeCommand: router la commande vers le bon handler.
**
** Avant enregistrement (accessibles toujours):
**   PASS, NICK, USER, QUIT, CAP (ignorer), PING, PONG
**
** Après enregistrement (vérifier client->isRegistered()):
**   JOIN, PART, PRIVMSG, NOTICE, KICK, INVITE, TOPIC, MODE, WHO
**
** Commande inconnue -> sendReply(ERR_UNKNOWNCOMMAND)
** Client pas registered pour une commande post-registration
	-> sendReply(ERR_NOTREGISTERED)
*/
void Server::_executeCommand(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
	// TODO: Partagé
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
		sendReply(client, ERR_ALREADYREGISTERED, "PASS :You may not reregister");
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
	sendReply(client, RPL_WELCOME, "Welcome to the IRC Network " + client->getPrefix());
	sendReply(client, RPL_YOURHOST, "Your host is ft_irc, running version 1.0");
	sendReply(client, RPL_CREATED, "This server was created <date>");
	sendReply(client, RPL_MYINFO, "ft_irc 1.0 o itkol");
}

void Server::_handleNick(Client *client, const Message &msg)
{
	Channel	*channel;

	if (msg.params.empty())
	{
		sendReply(client, ERR_NONICKNAMEGIVEN, "NICK :No nickname given");
		return ;
	}
	if (!nick_is_valid(msg.params[0]))
	{
		sendReply(client, ERR_ERRONEUSNICKNAME, msg.params[0] + ":Erroneous nickname");
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
		for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			channel = it->second;
			if (channel->isMember(client))
				channel->broadcast(":" + client->getNickname() + " NICK " + msg.params[0], client);
		}
		sendToClient(client, ":" + client->getNickname() + " NICK " + msg.params[0]);
	}
	bool wasRegistered = client->isRegistered();
	client->setNickname(msg.params[0]);
	if (!wasRegistered && client->isRegistered())
		_sendWelcome(client);
}

void Server::_handleUser(Client *client, const Message &msg)
{
	if (msg.params.size() != 4)
	{
		sendReply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return ;
	}
	if (client->isRegistered())
	{
		sendReply(client, ERR_ALREADYREGISTERED, "USER :You may not reregister");
		return ;
	}
	bool wasRegistered = client->isRegistered();
	client->setUsername(msg.params[0]);
	client->setRealname(msg.params[3]);
	if (!wasRegistered && client->isRegistered())
		_sendWelcome(client);
}

/*
** TODO: Personne B — QUIT
** - Récupérer le message de quit (params[0] ou "Quit")
** - Informer tous les channels où le client est présent
** - Marquer le client pour déconnexion
*/
void Server::_handleQuit(Client *client, const Message &msg)
{
}

/*
** TODO: Personne B — JOIN
** - Vérifier params non vide (sinon ERR_NEEDMOREPARAMS)
** - Gérer JOIN avec key si channel +k
** - Si channel n'existe pas -> le créer, le client devient opérateur
** - Vérifier les modes: +i (invité?), +k (bonne key?), +l (pas plein?)
** - Ajouter le client au channel
** - Broadcast ":<prefix> JOIN <channel>" à tous les membres
** - Envoyer le TOPIC (332 ou 331) et NAMES (353 + 366) au client
*/
void Server::_handleJoin(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — PART
** - Vérifier params non vide
** - Vérifier que le channel existe et que le client en est membre
** - Broadcast ":<prefix> PART <channel> :<reason>" aux membres
** - Retirer le client du channel
** - Si channel vide -> le supprimer
*/
void Server::_handlePart(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — PRIVMSG
** - Vérifier params: au moins 2 (cible + message)
** - Si cible commence par '#' -> message vers un channel
**		- Vérifier que le channel existe et que le client en est membre
**		- Broadcast le message aux autres membres
** - Sinon -> message privé vers un utilisateur
**		- Vérifier que le nick existe avec getClientByNick()
**		- Envoyer le message directement
*/
void Server::_handlePrivmsg(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — NOTICE
** Comme PRIVMSG mais ne génère AUCUNE réponse d'erreur.
*/
void Server::_handleNotice(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — KICK
** - Vérifier params: channel + nick à kick
** - Vérifier que le client est opérateur du channel
** - Vérifier que la cible est membre du channel
** - Broadcast ":<prefix> KICK <channel> <nick> :<reason>"
** - Retirer la cible du channel
*/
void Server::_handleKick(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — INVITE
** - Vérifier params: nick + channel
** - Vérifier que le client est membre (et opérateur si +i) du channel
** - Vérifier que la cible existe et n'est pas déjà dans le channel
** - Ajouter la cible à la liste des invités
** - Envoyer RPL_INVITING au client
** - Envoyer ":<prefix> INVITE <nick> <channel>" à la cible
*/
void Server::_handleInvite(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — TOPIC
** - Si pas de params[1]: afficher le topic actuel (332 ou 331)
** - Si params[1] présent: changer le topic
**		- Si channel +t, vérifier que le client est opérateur
** - Broadcast le changement de topic aux membres
*/
void Server::_handleTopic(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — MODE
** - Parser le mode string (ex: "+ik password", "-l", "+o nick")
** - Pour chaque flag:
**		i: setInviteOnly
**		t: setTopicRestricted
**		k: setKey / removeKey
**		o: addOperator / removeOperator
**		l: setUserLimit / removeUserLimit
** - Vérifier que le client est opérateur du channel
** - Broadcast les changements de mode aux membres
** - Si pas de mode string: afficher les modes actuels (RPL_CHANNELMODEIS)
*/
void Server::_handleMode(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*
** PING — déjà simple, peut être fait par l'un ou l'autre.
** Répondre PONG avec le même paramètre.
*/
void Server::_handlePing(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
	// TODO: répondre ":ft_irc PONG ft_irc :<param>"
}

void Server::_handlePong(Client *client, const Message &msg)
{
	// Rien à faire, juste ignorer
	(void)client;
	(void)msg;
}

/*
** TODO: Personne B — WHO (optionnel mais WeeChat l'envoie)
** - Si la cible est un channel: lister les membres
** - Envoyer RPL_WHOREPLY (352) pour chaque membre + RPL_ENDOFWHO (315)
*/
void Server::_handleWho(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}
