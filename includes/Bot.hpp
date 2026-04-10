#ifndef BOT_HPP
#define BOT_HPP

#include <string>

class Bot
{
  public:
	Bot(const std::string &host, int port, const std::string &password);
	~Bot();
	void run();

  private:
	int _fd;
	std::string _host;
	int _port;
	std::string _password;

	void _connect();
	void _authenticate();
	void _handleMessage(const std::string &line);
	void _sendRaw(const std::string &msg);
};

#endif