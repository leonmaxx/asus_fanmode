/*
 * config.hpp
 *
 *  Created on: Sep 4, 2019
 *      Author: Leonid Maksymchuk
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <vector>

class ConfigParser {
public:
					ConfigParser();
					~ConfigParser();

	void			addOption(int& rnInt, const std::string& rsName);
	void			addOption(unsigned& rnUint, const std::string& rsName);
	void			addOption(bool& rbBool, const std::string& rsName);
	void			addOption(float& rfFloat, const std::string& rsName);
	void			addOption(std::string& rsStr, const std::string& rsName);

	bool			loadConfig(const std::string& rsFileName);

private:
	enum OptionType {
		OPT_INT			= 0,
		OPT_UNSIGNED	= 1,
		OPT_BOOL		= 2,
		OPT_FLOAT		= 3,
		OPT_STRING		= 4
	};

	struct ConfigOption {
		std::string		sName;
		OptionType		otType;
		void*			pvPtr;
	};

	std::vector<ConfigOption> m_voOptions;

	void			trimString(std::string& rsStr);
	bool			splitString(std::string& rsName, std::string& rsVal);
	bool			getLine(char** ppcBuf, std::string& rsLine);
	ConfigOption*	findOption(const std::string& rsName);
};

#endif /* CONFIG_HPP_ */
