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
// Constructor / Destructor
// ========================================

Server::Server(int port, const std::string &password) : _port(port),
	_password(password), _serverFd(-1)
{
	_initSocket();
	_initSignals();
}

Server::~Server(void)
{
	for (std::map<int,
		Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	for (std::map<std::string,
		Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	_channels.clear();
	if (_serverFd >= 0)
		close(_serverFd);
	std::cout << "[LOG] Server stopped cleanly" << std::endl;
}

// ========================================
// Utils
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
		delete it->second;
		_channels.erase(it);
	}
}

// ========================================
// Network
// ========================================

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
	pfd.events = POLLIN;
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
				_acceptClient();
			else if (_pollFds[i].revents & POLLIN)
				_receiveData(_pollFds[i].fd);
			else if (_pollFds[i].revents & POLLOUT)
				_sendData(_pollFds[i].fd);
			else if (_pollFds[i].revents & (POLLERR | POLLHUP))
			{
				_disconnectClient(_pollFds[i].fd);
				i--;
			}
		}
	}
}

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
	newClient->setHostname(hostname);
	_clients[clientFd] = newClient;
	pfd.fd = clientFd;
	pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
	std::cout << "[LOG] New client connected: " << hostname << " (fd=" << clientFd << ")" << std::endl;
}

void Server::_receiveData(int fd)
{
	char	buf[4096];
	int		bytesRead;
	Client	*client;

	memset(buf, 0, sizeof(buf));
	bytesRead = recv(fd, buf, sizeof(buf) - 1, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
			std::cout << "[LOG] Client fd=" << fd << " properly disconnected" << std::endl;
		else
			std::cerr << "[LOG] Erreur recv() fd=" << fd << ": " << strerror(errno) << std::endl;
		_disconnectClient(fd);
		return ;
	}
	client = _clients[fd];
	client->appendRecvBuffer(std::string(buf, bytesRead));
	_processLines(client);
}

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
	std::cout << "[LOG] Client disconnected fd=" << fd << std::endl;
	_clients.erase(fd);
	delete	client;
}

void Server::_processLines(Client *client)
{
	Message msg;
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