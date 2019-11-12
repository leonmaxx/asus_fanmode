/*
 * config.cpp
 *
 *  Created on: Sep 4, 2019
 *      Author: Leonid Maksymchuk
 */

#include <string.h>
#include "config.hpp"
#include "file.hpp"

ConfigParser::ConfigParser() {
}

ConfigParser::~ConfigParser() {
}

#define CPOPTION(t,o) \
	void ConfigParser::addOption(t& rOpt, const std::string& rsName) { \
		ConfigOption coOption; \
		coOption.sName = rsName; \
		coOption.otType = o; \
		coOption.pvPtr = &rOpt; \
		m_voOptions.push_back(coOption); \
	}

CPOPTION(int, OPT_INT)
CPOPTION(unsigned, OPT_UNSIGNED)
CPOPTION(bool, OPT_BOOL)
CPOPTION(float, OPT_FLOAT)
CPOPTION(std::string, OPT_STRING)

bool ConfigParser::loadConfig(const std::string& rsFileName) {
	File fCfg(rsFileName);

	if (!fCfg.isValid()) {
		return false;
	}

	size_t nSize = fCfg.size();
	char vcBuf[nSize + 1];
	char* pcBufPtr = vcBuf;

	if (fCfg.read(vcBuf, nSize) != (ssize_t) nSize) {
		return false;
	}

	std::string sName, sValue;
	vcBuf[nSize] = 0;

	while (getLine(&pcBufPtr, sName)) {
		std::string::size_type nPos;

		if ((nPos = sName.find('#')) != std::string::npos) {
			// remove comment
			sName.erase(nPos);
		}

		if (!splitString(sName, sValue) || sName.empty()) {
			//
			continue;
		}

		ConfigOption* poOption = findOption(sName);

		if (!poOption) {
			fprintf(stderr, "Option \"%s\" ignored.\n", sName.c_str());
			continue;
		}

#ifdef _DEBUG
		printf("Option \"%s\": type %u, value \"%s\".\n",
				sName.c_str(), poOption->otType, sValue.c_str());
#endif

		switch (poOption->otType) {
		case OPT_INT:
			*(int*)poOption->pvPtr = strtol(sValue.c_str(), nullptr, 10);
			break;
		case OPT_UNSIGNED:
			*(unsigned*)poOption->pvPtr = strtoul(sValue.c_str(), nullptr, 10);
			break;
		case OPT_BOOL:
			*(bool*)poOption->pvPtr = sValue.compare("true") == 0;
			break;
		case OPT_FLOAT:
			*(float*)poOption->pvPtr = strtof(sValue.c_str(), nullptr);
			break;
		case OPT_STRING:
			((std::string*)poOption->pvPtr)->assign(sValue);
			break;
		default:
			fprintf(stderr, "Option \"%s\" type %u is invalid.\n",
					sName.c_str(), poOption->otType);
		}
	}

	return true;
}

void ConfigParser::trimString(std::string& rsStr) {
	std::string sWs(" \t\r\n");
	std::string::size_type nPos = 0;

	while (sWs.find(rsStr[nPos]) != std::string::npos) {
		nPos++;
	}

	if (nPos) {
		rsStr.erase(0, nPos);
	}

	nPos = rsStr.size();
	while (nPos >= 1 && sWs.find(rsStr[nPos-1]) != std::string::npos) {
		nPos--;
	}

	if (nPos < rsStr.size()) {
		rsStr.erase(nPos);
	}
}

bool ConfigParser::splitString(std::string& rsName, std::string& rsVal) {
	std::string::size_type nPos = rsName.find('=');

	if (nPos == std::string::npos) {
		return false;
	}

	rsVal = rsName.substr(nPos + 1);
	rsName.erase(nPos);

	trimString(rsName);
	trimString(rsVal);

	return true;
}

bool ConfigParser::getLine(char** ppcBuf, std::string& rsLine) {
	while ((**ppcBuf) != 0) {
		char* pcNL = strchr(*ppcBuf, '\n');
		size_t nLen;

		if (pcNL == *ppcBuf) {
			// new line, skip
			(*ppcBuf)++;
			continue;
		}

		if (pcNL == nullptr) {
			// last line
			nLen = strlen(*ppcBuf);
		}
		else {
			nLen = pcNL - *ppcBuf;
		}

		rsLine = std::string(*ppcBuf, nLen);
		(*ppcBuf) += nLen;
		return true;
	}

	return false;
}

ConfigParser::ConfigOption* ConfigParser::findOption(const std::string& rsName) {
	for (unsigned n = 0; n < m_voOptions.size(); n++) {
		if (m_voOptions[n].sName == rsName) {
			return &m_voOptions[n];
		}
	}

	return nullptr;
}
