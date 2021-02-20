# Copyright (c) 2021 by Thomas A. Early N7TAE
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# locations for the executibles and other files are set here
# NOTE: IF YOU CHANGE THESE, YOU WILL NEED TO UPDATE THE service.* FILES AND
# if you change these locations, make sure the sgs.service file is updated!
# you will also break hard coded paths in the dashboard file, index.php.

CFGDIR=/usr/local/etc
BINDIR=/usr/local/bin
WWWDIR=/usr/local/www
#WWWDIR=/var/www/html
GATDIR=../DMRGateway
SYSDIR=/etc/systemd/system

CPPFLAGS  = -g -Wall -std=c++11 -DCFG_DIR=\"$(CFGDIR)\"
LIBS = -l sqlite3 -lpthread

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

EXE = doh

$(EXE) : $(OBJS)
	g++ $(OBJS) $(CFLAGS) $(LIBS) -o $(EXE)

%.o : %.cpp
	g++ -MMD -MD $(CPPFLAGS) -c $< -o $@

.PHONY : clean

clean :
	$(RM) $(EXE) *.o *.d

-include $(DEPS)

install : $(EXE) dmr.cfg
	/bin/cp -f $(EXE) $(BINDIR)
	/bin/cp -f system/DMRIDUpdate.sh $(BINDIR)/update-dmrid
	$(BINDIR)/update-dmrid
	/bin/ln -f -s $(shell pwd)/dmr.cfg $(CFGDIR)
	sed -e "s/XXX/$(EXE)/" -e "s/YYY/dmr/" system/$(EXE).service > $(SYSDIR)/$(EXE).service
	/bin/cp -f system/$(EXE).timer $(SYSDIR)
	systemctl enable $(EXE).timer
	systemctl daemon-reload
	systemctl start $(EXE).service

installgateway : $(GATDIR)/DMRGateway $(GATDIR)/DMRGateway.cfg
	/bin/cp -f $(GATDIR)/DMRGateway $(BINDIR)
	/bin/ln -f -s $(shell pwd)/$(GATDIR)/DMRGateway.cfg $(CFGDIR)
	sed -e "s/XXX/DMRGateway/" -e "s/YYY/DMRGateway/" system/$(EXE).service > $(SYSDIR)/dmrgateway.service
	/bin/cp -f system/gateway.timer $(SYSDIR)/dmrgateway.timer
	systemctl enable dmrgateway.timer
	systemctl daemon-reload
	systemctl start dmrgateway.service

installdash : index.php
	/usr/bin/apt update
	/usr/bin/apt install -y php-common php-fpm sqlite3 php-sqlite3 dnsutils
	mkdir -p $(WWWDIR)
	/bin/ln -f -s $(shell pwd)/index.php $(WWWDIR)
	/bin/cp -f system/qndash.service $(SYSDIR)
	systemctl enable qndash.service
	systemctl daemon-reload
	systemctl start qndash.service

uninstall :
	systemctl stop $(EXE).service
	systemctl disable $(EXE).timer
	/bin/rm -f $(SYSDIR)/$(EXE).service
	/bin/rm -f $(SYSDIR)/$(EXE).timer
	/bin/rm -f $(BINDIR)/$(EXE) $(BINDIR)/update-dmrid
	/bin/rm -f $(CFGDIR)/$(EXE).cfg
	/bin/rm -f $(CFGDIR)/$(exe).db
	sudo systemctl daemon-reload

uninstallgateway :
	systemctl stop dmrgateway.service
	systemctl disable dmrgateway.timer
	/bin/rm -f $(SYSDIR)/dmrgateway.service
	/bin/rm -f $(SYSDIR)/dmrgateway.timer
	/bin/rm -f $(BINDIR)/DMRGateway
	/bin/rm -f $(CFGDIR)/DMRGateway.cfg
	sudo systemctl daemon-reload

uninstalldash :
	systemctl stop qndash.service
	systemctl disable qndash.service
	/bin/rm -f $(SYSDIR)/qndash.service
	systemctl daemon-reload
	/bin/rm -f $(WWWDIR)/index.php
	/bin/rm -f $(CFGDIR)/qn.db
