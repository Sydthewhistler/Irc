#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include "Client.hpp"
# include "Channel.hpp"
# include "Message.hpp"

/*
** Classe principale du serveur IRC.
** Gère la boucle poll(), les connexions, et dispatche les commandes.
**
** Personne A implémente : constructeur, run(), _acceptClient(),
**   _receiveData(), _sendData(), _disconnectClient()
**
** Personne B implémente : tous les _handle*() et les méthodes utilitaires
**   de réponse (sendReply, sendError, etc.)
*/

class Server
{
public:
	Server(int port, const std::string &password);
	~Server(void);

	void	run(void);	// Lance la boucle principale

	// --- Utilitaires partagées ---
	// Envoie un message brut à un client (ajoute \r\n)
	void	sendToClient(Client *client, const std::string &message);
	// Envoie une numeric reply formatée: ":servername CODE nick :message"
	void	sendReply(Client *client, const std::string &code,
					const std::string &params);
	// Cherche un client par nickname (NULL si introuvable)
	Client	*getClientByNick(const std::string &nickname);
	// Cherche un channel par nom (NULL si introuvable)
	Channel	*getChannel(const std::string &name);
	// Crée un channel et retourne un pointeur dessus
	Channel	*createChannel(const std::string &name);
	// Supprime un channel vide
	void	removeChannel(const std::string &name);

	// Nom du serveur (pour les préfixes de réponse)
	static const std::string	SERVER_NAME;

private:
	Server(void);
	Server(const Server &src);
	Server &operator=(const Server &rhs);

	// --- Réseau (Personne A) ---
	void	_initSocket(void);
	void	_acceptClient(void);
	void	_receiveData(int fd);
	void	_sendData(int fd);
	void	_disconnectClient(int fd);
	void	_processLines(Client *client);

	// --- Dispatch des commandes ---
	void	_executeCommand(Client *client, const Message &msg);

	// --- Handlers de commandes (Personne B) ---
	void	_handlePass(Client *client, const Message &msg);
	void	_handleNick(Client *client, const Message &msg);
	void	_handleUser(Client *client, const Message &msg);
	void	_handleQuit(Client *client, const Message &msg);
	void	_handleJoin(Client *client, const Message &msg);
	void	_handlePart(Client *client, const Message &msg);
	void	_handlePrivmsg(Client *client, const Message &msg);
	void	_handleNotice(Client *client, const Message &msg);
	void	_handleKick(Client *client, const Message &msg);
	void	_handleInvite(Client *client, const Message &msg);
	void	_handleTopic(Client *client, const Message &msg);
	void	_handleMode(Client *client, const Message &msg);
	void	_handlePing(Client *client, const Message &msg);
	void	_handlePong(Client *client, const Message &msg);
	void	_handleWho(Client *client, const Message &msg);

	// --- Données du serveur ---
	int								_port;
	std::string						_password;
	int								_serverFd;
	std::vector<struct pollfd>		_pollFds;
	std::map<int, Client *>			_clients;		// fd -> Client*
	std::map<std::string, Channel *>	_channels;	// name -> Channel*

	void	_sendWelcome(Client *client);
};

bool	nick_is_valid(const std::string &nickname);

#endif
