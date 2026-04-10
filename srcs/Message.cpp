#include "Message.hpp"

Message::Message(void) {}

Message::Message(const Message &src)
	: prefix(src.prefix), command(src.command), params(src.params) {}

Message &Message::operator=(const Message &rhs)
{
	if (this != &rhs)
	{
		prefix = rhs.prefix;
		command = rhs.command;
		params = rhs.params;
	}
	return *this;
}

Message::~Message(void) {}

Message Message::parse(const std::string &raw)
{
	Message msg;
	if (raw.empty())
		return msg;

	std::istringstream iss(raw);
	std::string token;

	if (raw[0] == ':')
	{
		iss >> msg.prefix;
		msg.prefix = msg.prefix.substr(1);
	}

	//Command
	iss >> msg.command;
	for (size_t i = 0; i < msg.command.size(); i++)
		msg.command[i] = std::toupper(msg.command[i]);

	//Params
	while (iss >> token)
	{
		if (token[0] == ':')
		{
			std::string trailing = token.substr(1);
			std::string rest;
			if (std::getline(iss, rest))
				trailing += rest;
			while (!trailing.empty() && trailing[0] == ':')
				trailing = trailing.substr(1);
			msg.params.push_back(trailing);
			break;
		}
		msg.params.push_back(token);
	}

	return msg;
}

std::string	Message::toString(void) const
{
	std::string result;
	if (!prefix.empty())
		result += ":" + prefix + " ";
	result += command;
	for(size_t i = 0; i < params.size(); i++)
	{
		if (params[i].find(' ') != std::string::npos || params[i][0] == ':')
			result += " :" + params[i];
		else
			result += " " + params[i];
	}
	return result;
}
