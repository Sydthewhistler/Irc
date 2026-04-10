#include "Bot.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Bot::Bot(const std::string &host, int port,
	const std::string &password) : _fd(-1), _host(host), _port(port),
	_password(password)
{
	_connect();
	_authenticate();
}

Bot::~Bot()
{
	if (_fd >= 0)
		close(_fd);
}

void Bot::_connect()
{
	struct sockaddr_in	addr;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
	{
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_host.c_str());
	if (connect(_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Error connecting: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Bot connected!" << std::endl;
}

void Bot::_authenticate()
{
	_sendRaw("PASS " + _password + "\r\n");
	_sendRaw("NICK bot\r\n");
	_sendRaw("USER bot 0 * :Bot\r\n");
	_sendRaw("JOIN #general\r\n");
}

void Bot::_sendRaw(const std::string &msg)
{
	send(_fd, msg.c_str(), msg.size(), 0);
}

void Bot::_handleMessage(const std::string &line)
{
	time_t	now;

	if (line.find("!time") == std::string::npos)
		return ;
	now = time(0);
	std::string timeStr = ctime(&now);
	if (!timeStr.empty() && timeStr[timeStr.size() - 1] == '\n')
		timeStr.erase(timeStr.size() - 1);
	_sendRaw("PRIVMSG #general :" + timeStr + "\r\n");
}

void Bot::run()
{
	char	buf[4096];
	int		bytesRead;

	while (true)
	{
		memset(buf, 0, sizeof(buf));
		bytesRead = recv(_fd, buf, sizeof(buf) - 1, 0);
		if (bytesRead <= 0)
			break ;
		std::string line(buf);
		std::cout << line << std::endl;
		_handleMessage(line);
	}
}

int	main(void)
{
	Bot bot("127.0.0.1", 6667, "test");
	bot.run();
}