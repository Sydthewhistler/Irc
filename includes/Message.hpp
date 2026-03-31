#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>
# include <sstream>

/*
** Structure représentant un message IRC parsé.
** Format IRC: [:<prefix>] <command> [<params>] [:<trailing>]\r\n
**
** Exemple: ":nick!user@host PRIVMSG #channel :Hello world\r\n"
**   -> prefix   = "nick!user@host"
**   -> command  = "PRIVMSG"
**   -> params   = ["#channel", "Hello world"]
**
** C'est l'interface clé entre Personne A (réseau/parsing)
** et Personne B (logique des commandes).
*/

struct Message
{
	std::string				prefix;		// optionnel, vide si absent
	std::string				command;	// ex: "NICK", "JOIN", "PRIVMSG"
	std::vector<std::string>	params;		// paramètres de la commande

	Message(void);
	Message(const Message &src);
	Message &operator=(const Message &rhs);
	~Message(void);

	// Parse une ligne brute IRC en Message
	// La ligne ne doit PAS contenir le \r\n final
	static Message	parse(const std::string &raw);

	// Reconstruit le message en string IRC (sans \r\n)
	std::string		toString(void) const;
};

#endif
