#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

/*
** Représente un client connecté au serveur.
** Chaque client a un fd unique, un buffer de réception
** (pour gérer les données partielles), et ses infos IRC.
**
** États d'authentification:
**   1. Connexion TCP établie (fd attribué)
**   2. PASS reçu (mot de passe validé)
**   3. NICK reçu (nickname défini)
**   4. USER reçu (username défini)
**   -> Quand les 3 sont faits = client "registered" (envoyer RPL_WELCOME 001)
*/

class Client
{
public:
	Client(int fd);
	Client(const Client &src);
	Client &operator=(const Client &rhs);
	~Client(void);

	// --- Getters ---
	int					getFd(void) const;
	const std::string	&getNickname(void) const;
	const std::string	&getUsername(void) const;
	const std::string	&getRealname(void) const;
	const std::string	&getHostname(void) const;
	const std::string	&getRecvBuffer(void) const;
	const std::string	&getSendBuffer(void) const;
	bool				isRegistered(void) const;
	bool				hasPassValidated(void) const;

	// --- Setters ---
	void	setNickname(const std::string &nickname);
	void	setUsername(const std::string &username);
	void	setRealname(const std::string &realname);
	void	setHostname(const std::string &hostname);
	void	setPassValidated(bool validated);

	// --- Buffer management ---
	// Personne A utilise ces méthodes pour gérer les données partielles
	void		appendRecvBuffer(const std::string &data);
	void		clearRecvBuffer(void);
	void		appendSendBuffer(const std::string &data);
	void		clearSendBuffer(void);
	// Extrait la prochaine ligne complète (terminée par \r\n) du buffer
	// Retourne true si une ligne a été extraite, false sinon
	bool		extractLine(std::string &line);

	// --- Utilitaire ---
	// Retourne le préfixe IRC du client: "nick!user@host"
	std::string	getPrefix(void) const;

private:
	Client(void);

	int			_fd;
	std::string	_nickname;
	std::string	_username;
	std::string	_realname;
	std::string	_hostname;
	std::string	_recvBuffer;	// buffer de réception (données partielles)
	std::string	_sendBuffer;	// buffer d'envoi (si send() ne peut pas tout envoyer)
	bool		_passValidated;
};

#endif
