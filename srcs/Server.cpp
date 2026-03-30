#include "Server.hpp"
#include "Replies.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

const std::string Server::SERVER_NAME = "ft_irc";

// ========================================
// Constructeur / Destructeur
// ========================================

Server::Server(int port, const std::string &password)
	: _port(port), _password(password), _serverFd(-1)
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
void	Server::sendToClient(Client *client, const std::string &message)
{
	(void)client; (void)message;
	// TODO
}

/*
** TODO: Partagé
** sendReply: formate ":ft_irc <code> <nickname> <params>"
** et appelle sendToClient.
*/
void	Server::sendReply(Client *client, const std::string &code,
						const std::string &params)
{
	(void)client; (void)code; (void)params;
	// TODO
}

/*
** TODO: Partagé
** getClientByNick: parcourt _clients, retourne le Client*
** dont le nickname correspond. NULL si pas trouvé.
*/
Client	*Server::getClientByNick(const std::string &nickname)
{
	(void)nickname;
	// TODO
	return NULL;
}

/*
** TODO: Partagé
** getChannel: cherche dans _channels par nom. NULL si pas trouvé.
*/
Channel	*Server::getChannel(const std::string &name)
{
	(void)name;
	// TODO
	return NULL;
}

/*
** TODO: Partagé
** createChannel: new Channel(name), l'ajouter à _channels, retourner le pointeur.
*/
Channel	*Server::createChannel(const std::string &name)
{
	(void)name;
	// TODO
	return NULL;
}

/*
** TODO: Partagé
** removeChannel: delete le channel et le retirer de _channels.
*/
void	Server::removeChannel(const std::string &name)
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
void	Server::_initSocket(void)
{
	// TODO: Personne A
}

/*
** TODO: Personne A
** run: boucle principale
**   1. Installer signal handlers (SIGINT -> arrêt propre, SIGPIPE -> SIG_IGN)
**   2. while (running):
**      a. poll(_pollFds, timeout)
**      b. Parcourir les fds:
**         - _serverFd + POLLIN  -> _acceptClient()
**         - client fd + POLLIN  -> _receiveData(fd)
**         - client fd + POLLOUT + sendBuffer non vide -> _sendData(fd)
**         - POLLERR / POLLHUP   -> _disconnectClient(fd)
*/
void	Server::run(void)
{
	// TODO: Personne A
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
void	Server::_acceptClient(void)
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
void	Server::_receiveData(int fd)
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
void	Server::_sendData(int fd)
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
void	Server::_disconnectClient(int fd)
{
	(void)fd;
	// TODO: Personne A
}

/*
** TODO: Personne A
** _processLines:
**   Tant que client->extractLine(line) retourne true:
**     - Si line est vide, continuer
**     - Message msg = Message::parse(line)
**     - Si msg.command non vide -> _executeCommand(client, msg)
*/
void	Server::_processLines(Client *client)
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
** Client pas registered pour une commande post-registration -> sendReply(ERR_NOTREGISTERED)
*/
void	Server::_executeCommand(Client *client, const Message &msg)
{
	(void)client; (void)msg;
	// TODO: Partagé
}

// ========================================
// Handlers — Personne B
// ========================================

/*
** TODO: Personne B — PASS
** - Vérifier params non vide (sinon ERR_NEEDMOREPARAMS)
** - Si déjà registered -> ERR_ALREADYREGISTERED
** - Comparer params[0] avec _password
** - Si ok -> client->setPassValidated(true)
** - Si ko -> ERR_PASSWDMISMATCH
*/
void	Server::_handlePass(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — NICK
** - Vérifier params non vide (sinon ERR_NONICKNAMEGIVEN)
** - Valider le format du nickname (pas de caractères interdits)
** - Vérifier unicité avec getClientByNick() (sinon ERR_NICKNAMEINUSE)
** - Si le client était déjà registered, broadcast le changement de nick
** - client->setNickname(newNick)
** - Si maintenant isRegistered() et ne l'était pas avant -> envoyer RPL_WELCOME
*/
void	Server::_handleNick(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — USER
** - Vérifier au moins 4 params (sinon ERR_NEEDMOREPARAMS)
** - Si déjà registered -> ERR_ALREADYREGISTERED
** - client->setUsername(params[0])
** - client->setRealname(params[3]) (le trailing)
** - Si maintenant isRegistered() -> envoyer RPL_WELCOME (001, 002, 003, 004)
*/
void	Server::_handleUser(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — QUIT
** - Récupérer le message de quit (params[0] ou "Quit")
** - Informer tous les channels où le client est présent
** - Marquer le client pour déconnexion
*/
void	Server::_handleQuit(Client *client, const Message &msg)
{
	(void)client; (void)msg;
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
void	Server::_handleJoin(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — PART
** - Vérifier params non vide
** - Vérifier que le channel existe et que le client en est membre
** - Broadcast ":<prefix> PART <channel> :<reason>" aux membres
** - Retirer le client du channel
** - Si channel vide -> le supprimer
*/
void	Server::_handlePart(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — PRIVMSG
** - Vérifier params: au moins 2 (cible + message)
** - Si cible commence par '#' -> message vers un channel
**     - Vérifier que le channel existe et que le client en est membre
**     - Broadcast le message aux autres membres
** - Sinon -> message privé vers un utilisateur
**     - Vérifier que le nick existe avec getClientByNick()
**     - Envoyer le message directement
*/
void	Server::_handlePrivmsg(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — NOTICE
** Comme PRIVMSG mais ne génère AUCUNE réponse d'erreur.
*/
void	Server::_handleNotice(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — KICK
** - Vérifier params: channel + nick à kick
** - Vérifier que le client est opérateur du channel
** - Vérifier que la cible est membre du channel
** - Broadcast ":<prefix> KICK <channel> <nick> :<reason>"
** - Retirer la cible du channel
*/
void	Server::_handleKick(Client *client, const Message &msg)
{
	(void)client; (void)msg;
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
void	Server::_handleInvite(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — TOPIC
** - Si pas de params[1]: afficher le topic actuel (332 ou 331)
** - Si params[1] présent: changer le topic
**     - Si channel +t, vérifier que le client est opérateur
** - Broadcast le changement de topic aux membres
*/
void	Server::_handleTopic(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** TODO: Personne B — MODE
** - Parser le mode string (ex: "+ik password", "-l", "+o nick")
** - Pour chaque flag:
**     i: setInviteOnly
**     t: setTopicRestricted
**     k: setKey / removeKey
**     o: addOperator / removeOperator
**     l: setUserLimit / removeUserLimit
** - Vérifier que le client est opérateur du channel
** - Broadcast les changements de mode aux membres
** - Si pas de mode string: afficher les modes actuels (RPL_CHANNELMODEIS)
*/
void	Server::_handleMode(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}

/*
** PING — déjà simple, peut être fait par l'un ou l'autre.
** Répondre PONG avec le même paramètre.
*/
void	Server::_handlePing(Client *client, const Message &msg)
{
	(void)client; (void)msg;
	// TODO: répondre ":ft_irc PONG ft_irc :<param>"
}

void	Server::_handlePong(Client *client, const Message &msg)
{
	// Rien à faire, juste ignorer
	(void)client; (void)msg;
}

/*
** TODO: Personne B — WHO (optionnel mais WeeChat l'envoie)
** - Si la cible est un channel: lister les membres
** - Envoyer RPL_WHOREPLY (352) pour chaque membre + RPL_ENDOFWHO (315)
*/
void	Server::_handleWho(Client *client, const Message &msg)
{
	(void)client; (void)msg;
}
