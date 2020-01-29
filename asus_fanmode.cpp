/*
 * asus_fanmode.cpp
 *
 *  Created on: Nov 12, 2019
 *      Author: Leonid Maksymchuk
 */

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "config.hpp"
#include "file.hpp"

#define FAILCNT_LIMIT	5
#define FILEBUF_SIZE	32
#define FORMATBUF_SIZE	1024

static struct Config {
	std::string		sCPUTemp;
	unsigned		nCPUTempDivider;

	std::string		sFanMode;

	unsigned		nModeSilent;
	unsigned		nModeNormal;
	unsigned		nModeOverboost;

	unsigned		nPollInterval;

	unsigned		nNormalHigh;
	unsigned		nNormalLow;

	unsigned		nOverboostHigh;
	unsigned		nOverboostLow;
} s_cCfg;

static bool s_bRunning = true;

std::string format(const char* pcFmt, ...) {
	char vcBuf[FORMATBUF_SIZE];

	va_list vaArgs;
	va_start(vaArgs, pcFmt);

	int nLen = vsprintf(vcBuf, pcFmt, vaArgs);

	return std::string(vcBuf, nLen);
}

bool readString(File& rfFile, std::string& rsStr) {
	if (!rfFile.isValid()) {
		return false;
	}

	ssize_t nSize;
	char vcBuf[FILEBUF_SIZE + 1];

	if ((nSize = rfFile.read(vcBuf, FILEBUF_SIZE)) <= 0) {
		return false;
	}

	rsStr = std::string(vcBuf, nSize);
	return true;
}

bool searchCPUHwmon() {
	int nIndex = 0;

	while (true) {
		std::string sPath, sName;
		sPath = format("/sys/class/hwmon/hwmon%d/", nIndex);

		File fName(sPath + "name");

		if (!fName.isValid() || !readString(fName, sName)) {
			break;
		}

		if (sName.compare(0, 7, "k10temp") == 0 ||
				sName.compare(0, 8, "coretemp") == 0) {
			s_cCfg.sCPUTemp = sPath + "temp1_input";
			return true;
		}

		nIndex++;
	}

	return false;
}

bool readUint(File& rfFile, unsigned& rnValue) {
	if (!rfFile.isValid()) {
		return false;
	}

	ssize_t nSize;
	char vcBuf[FILEBUF_SIZE + 1];

	if ((nSize = rfFile.read(vcBuf, FILEBUF_SIZE)) <= 0) {
		return false;
	}

	vcBuf[nSize] = '\0';
	rnValue = strtoul(vcBuf, nullptr, 10);

	return true;
}

bool searchFanMode() {
	const std::string vsPaths[2] = {
		"/sys/devices/platform/asus-nb-wmi/fan_boost_mode",
		"/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy"
	};

	for (int n = 0; n < 2; n++) {
		File fFanMode(vsPaths[n]);
		unsigned nValue;

		if (fFanMode.isValid() && readUint(fFanMode, nValue)) {
			s_cCfg.sFanMode = vsPaths[n];
			return true;
		}
	}

	return false;
}

bool readTemp(unsigned& rnTemp) {
	File fTemp(s_cCfg.sCPUTemp);
	return readUint(fTemp, rnTemp);
}

bool readFanMode(unsigned& rnMode) {
	File fFanMode(s_cCfg.sFanMode);
	return readUint(fFanMode, rnMode);
}

bool writeFanMode(unsigned nMode) {
	File fFanMode(s_cCfg.sFanMode, O_WRONLY);
	if (!fFanMode.isValid()) {
		return false;
	}

	char vcBuf[12];
	int nLength = sprintf(vcBuf, "%u", nMode);

	return fFanMode.write(vcBuf, nLength) == nLength;
}

const char* fanModeToStr(unsigned nMode) {
	if (nMode == s_cCfg.nModeSilent) {
		return "Silent";
	}
	else if (nMode == s_cCfg.nModeNormal) {
		return "Normal";
	}
	else if (nMode == s_cCfg.nModeOverboost) {
		return "Overboost";
	}
	return "Unknown";
}

int printerr(const char* pcFmt, ...) {
	va_list vaArgs;
	va_start(vaArgs, pcFmt);

	int nLen = vfprintf(stderr, pcFmt, vaArgs);
	nLen += fprintf(stderr, " Error: %s\n", strerror(errno));

	va_end(vaArgs);
	return nLen;
}

void processSigTerm(int nSignal) {
	fprintf(stderr, "Terminating...\n");
	s_bRunning = false;
}

int main() {
	signal(SIGINT, processSigTerm);
	signal(SIGTERM, processSigTerm);

	ConfigParser cpConf;
	cpConf.addOption(s_cCfg.sCPUTemp,			"cpu_temp");
	cpConf.addOption(s_cCfg.nCPUTempDivider,	"cpu_temp_divider");
	cpConf.addOption(s_cCfg.sFanMode,			"fan_mode");
	cpConf.addOption(s_cCfg.nModeSilent,		"mode_silent");
	cpConf.addOption(s_cCfg.nModeNormal,		"mode_normal");
	cpConf.addOption(s_cCfg.nModeOverboost,		"mode_overboost");
	cpConf.addOption(s_cCfg.nPollInterval,		"poll");
	cpConf.addOption(s_cCfg.nNormalHigh,		"normal_high");
	cpConf.addOption(s_cCfg.nNormalLow,			"normal_low");
	cpConf.addOption(s_cCfg.nOverboostHigh,		"overboost_high");
	cpConf.addOption(s_cCfg.nOverboostLow,		"overboost_low");

	if (!cpConf.loadConfig("/etc/asus_fanmode.conf") &&
			!cpConf.loadConfig("asus_fanmode.conf")) {
		fprintf(stderr, "Cannot find configuration file!\n");
		return ENOENT;
	}

	if (s_cCfg.sCPUTemp.empty()) {
		if (!searchCPUHwmon()) {
			fprintf(stderr, "Cannot find CPU hwmon sysfs entry!\n");
			return ENODEV;
		}

		printf("Found CPU hwmon: %s\n", s_cCfg.sCPUTemp.c_str());
	}

	if (s_cCfg.sFanMode.empty()) {
		if (!searchFanMode()) {
			fprintf(stderr, "Cannot find Fan Mode sysfs entry!\n");
			return ENODEV;
		}

		printf("Found Fan Mode: %s\n", s_cCfg.sFanMode.c_str());
	}

	unsigned nCurrentTemp;
	unsigned nCurrentMode;
	int nFailCounter = 0;

	if (!readTemp(nCurrentTemp) || (nCurrentTemp / s_cCfg.nCPUTempDivider) == 0) {
		printerr("Cannot read CPU temperature: %s\n", s_cCfg.sCPUTemp.c_str());
		return ENODEV;
	}

	printf("Current CPU temperature: %u\n", nCurrentTemp / s_cCfg.nCPUTempDivider);

	if (!readFanMode(nCurrentMode) || nCurrentMode > 2) {
		printerr("Cannot read fan mode: %s\n", s_cCfg.sFanMode.c_str());
		return ENODEV;
	}

	printf("Current Fan Mode: %s\n", fanModeToStr(nCurrentMode));
	fflush(stdout);

	while (s_bRunning) {
		if (!readTemp(nCurrentTemp)) {
			printerr("Cannot read CPU temperature!\n");
			nFailCounter++;
		}
		else if (!readFanMode(nCurrentMode)) {
			printerr("Cannot read fan mode!\n");
			nFailCounter++;
		}
		else {
			unsigned nTemp = nCurrentTemp / s_cCfg.nCPUTempDivider;
			unsigned nTargetMode = nCurrentMode;

			if (nCurrentMode == s_cCfg.nModeSilent) {
				if (nTemp >= s_cCfg.nOverboostHigh) {
					nTargetMode = s_cCfg.nModeOverboost;
				}
				else if (nTemp >= s_cCfg.nNormalHigh) {
					nTargetMode = s_cCfg.nModeNormal;
				}
			}
			else if (nCurrentMode == s_cCfg.nModeNormal) {
				if (nTemp >= s_cCfg.nOverboostHigh) {
					nTargetMode = s_cCfg.nModeOverboost;
				}
				else if (nTemp < s_cCfg.nNormalLow) {
					nTargetMode = s_cCfg.nModeSilent;
				}
			}
			else if (nCurrentMode == s_cCfg.nModeOverboost) {
				if (nTemp < s_cCfg.nOverboostLow) {
					nTargetMode = s_cCfg.nModeNormal;
				}
			}

			if (nTargetMode != nCurrentMode) {
				printf("Switching fan mode to \'%s\', temperature: %u\n",
						fanModeToStr(nTargetMode), nTemp);
				fflush(stdout);

				if (!writeFanMode(nTargetMode)) {
					printerr("Cannot switch fan mode!\n");
					nFailCounter++;
				}
			}
		}

		if (nFailCounter > FAILCNT_LIMIT) {
			fprintf(stderr, "Too many errors. Exiting.\n");
			return EIO;
		}

		usleep(s_cCfg.nPollInterval * 1000);
	}

	return 0;
}
