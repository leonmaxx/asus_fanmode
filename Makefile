#
# Makefile
#
#  Created on: Nov 12, 2019
#      Author: Leonid Maksymchuk
#

.PHONY: all clean install

CXX?=g++
CXXFLAGS+=-std=c++11 -O2 -g -Wall
RM=rm
INSTALL=/usr/bin/install

all: asus_fanmode

asus_fanmode: asus_fanmode.cpp config.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) asus_fanmode

install: asus_fanmode
	$(INSTALL) -Dm644 asus_fanmode.conf $(DESTDIR)/etc/asus_fanmode.conf
	$(INSTALL) -Dm644 asus_fanmode.service $(DESTDIR)/usr/lib/systemd/system/asus_fanmode.service
	$(INSTALL) -Dm755 asus_fanmode $(DESTDIR)/usr/bin/asus_fanmode
