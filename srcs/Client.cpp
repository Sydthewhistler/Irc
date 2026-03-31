#include "Client.hpp"

// Forme canonique (partagé)

Client::Client(int fd)
	: _fd(fd), _nickname("*"), _username(""), _realname(""),
	  _hostname(""), _recvBuffer(""), _sendBuffer(""),
	  _passValidated(false)
{
}

Client::Client(const Client &src)
	: _fd(src._fd), _nickname(src._nickname), _username(src._username),
	  _realname(src._realname), _hostname(src._hostname),
	  _recvBuffer(src._recvBuffer), _sendBuffer(src._sendBuffer),
	  _passValidated(src._passValidated)
{
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_nickname = rhs._nickname;
		_username = rhs._username;
		_realname = rhs._realname;
		_hostname = rhs._hostname;
		_recvBuffer = rhs._recvBuffer;
		_sendBuffer = rhs._sendBuffer;
		_passValidated = rhs._passValidated;
	}
	return *this;
}

Client::~Client(void) {}

// Getters simples (partagé)

int					Client::getFd(void) const { return _fd; }
const std::string	&Client::getNickname(void) const { return _nickname; }
const std::string	&Client::getUsername(void) const { return _username; }
const std::string	&Client::getRealname(void) const { return _realname; }
const std::string	&Client::getHostname(void) const { return _hostname; }
const std::string	&Client::getRecvBuffer(void) const { return _recvBuffer; }
const std::string	&Client::getSendBuffer(void) const { return _sendBuffer; }
bool				Client::hasPassValidated(void) const { return _passValidated; }

// Setters simples (partagé)

void	Client::setNickname(const std::string &nickname) { _nickname = nickname; }
void	Client::setUsername(const std::string &username) { _username = username; }
void	Client::setRealname(const std::string &realname) { _realname = realname; }
void	Client::setHostname(const std::string &hostname) { _hostname = hostname; }
void	Client::setPassValidated(bool validated) { _passValidated = validated; }

bool	Client::isRegistered(void) const
{
	if (_passValidated == true)
	{
		if (_nickname != "" && (_nickname != "*"))
		{
			if (_username != "")
				return (true);
		}
	}
	return false;
}

// --- Buffer management ---

void	Client::appendRecvBuffer(const std::string &data) { _recvBuffer += data; }
void	Client::clearRecvBuffer(void) { _recvBuffer.clear(); }
void	Client::appendSendBuffer(const std::string &data) { _sendBuffer += data; }
void	Client::clearSendBuffer(void) { _sendBuffer.clear(); }

/*
** TODO: Personne A
** extractLine(): cherche \r\n (ou \n seul pour compatibilité WeeChat)
** dans _recvBuffer.
** Si trouvé: extraire la ligne (sans le \r\n), la mettre dans 'line',
** retirer la ligne + le délimiteur du buffer, retourner true.
** Si pas trouvé: retourner false (données partielles, on attend la suite).
*/
bool	Client::extractLine(std::string &line)
{
	(void)line;
	// TODO: Personne A
	return false;
}

std::string	Client::getPrefix(void) const
{
	return (_nickname + "!" + _username + "@" + _hostname);
}
