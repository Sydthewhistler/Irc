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

/*
** TODO: Personne A
** Parse une ligne IRC brute (sans \r\n).
** Format: [:<prefix> ] <command> [<params>] [:<trailing>]
**
** Exemple: ":nick!user@host PRIVMSG #channel :Hello world"
**   -> prefix   = "nick!user@host"
**   -> command  = "PRIVMSG"
**   -> params   = ["#channel", "Hello world"]
**
** Etapes:
**   1. Si la ligne commence par ':', extraire le prefix (jusqu'au premier espace)
**   2. Extraire la commande (premier mot après le prefix)
**   3. Extraire les paramètres:
**      - Chaque mot séparé par un espace = un paramètre
**      - Si un paramètre commence par ':', tout le reste est UN seul paramètre (trailing)
**   4. Mettre la commande en majuscules (NICK, nick, Nick -> NICK)
*/
Message	Message::parse(const std::string &raw)
{
	Message msg;
	(void)raw;
	// TODO: Personne A
	return msg;
}

/*
** TODO: Personne A
** Reconstruit le message en string IRC (sans \r\n).
** Si prefix non vide -> ":<prefix> "
** Puis la commande, puis les params séparés par des espaces
** Le dernier param contenant un espace doit être précédé de ':'
*/
std::string	Message::toString(void) const
{
	// TODO: Personne A
	return "";
}
