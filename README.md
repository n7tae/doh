# DOH - A DMR-Only Hot-Spot

## Introduction

This will build a DMR-only Hotspot based on Jonathan Naylor's MMDVMHost software. It's quite a bit smaller than MMDVMHost as it **only** supports DMR modes. Also, it does not support *any* display device, but it will eventually support an html dashboard. While *Hot-spot* is in the title, this does support two time slots, duplex operation and any MMDVM-based modem so it would be suitable for control of a DMR-only repeater.

If you want a pre-build, multi-mode hot-spot, then you want Andy Taylor MW0MWZ's excellent Pi-Star. If you want to try your hand at building your own multi-mode hot-spot, then you can take a look at the MMDVM.README file in https::/github.com/n7tae/QnetGateway.

## Building

### Install package

DOH includes a dashboard with a last heard section. The lastheard section uses SQLite3, a light-weight database, so you will need a package:

```bash
sudo apt install libsqlite3-dev
```

### Download the repo

```bash
git clone https://github.com/n7tae/doh.git
```

### Move to the build directory and compile

```bash
cd doh
make
```

### Create and edit your configuration file

```bash
cp dmr.ini dmr.cfg
```

Use your favorite file editor to edit your `dmr.cfg` file. You'll want to add your callsign and DMR ID in the `[General]` section and change the Duplex line if you are using a modem that supports duplex operation. Set your frequency in the `[Info]` section and you can also change other values in this section.

Check other values and change as needed. Make sure the `[Modem]` section is correct for your MMDVM-based modem. The `[DMR Network]` section is defined as a **Direct** connection to the IPSC2-QUADNET DMR+ reflector. Of course you can connect to any system you want. Alternatively, you can use DMRGateway and connect to several systems, make sure you remove the active values in this `[DMR Network]` and uncomment the values for the `Type=Gateway` network.

### Optionally build and configure the DMRGateway

If you want to connect to multiple DMR networks, like BrandMeister and others, even at the same time:

```bash
cd ..
git clone https://github.com/g4klx/DMRGateway.git
cd DMRGateway
make
cp DMRGateway.ini DMRGateway.cfg
```

Then you can use your favorite editor to edit `DMRGateway.cfg`.

When you are done, move back to the `doh` build directory.

```bash
cd ../doh
```

## Installing

Install you program(s):

```bash
sudo make install
sudo make installgateway
```

Of course, you only need to install the gateway if your `doh` is configured to use `Type=Gateway` in the `[DMR Network]` section.

## Maintenance

When you installed `doh`, you downloaded the DMR ID look up table. If you want to freshen this table, do `update-dmrid`. You system will automatically start using the new table within a day. If you want to start using it right away, then do a `sudo systemctl restart doh`.

## Monitoring

Until the dashboard is complete, you can keep an eye on `doh` with `sudo journalctl -u doh -f`.

## Uninstalling

```bash
sudo make uninstall
sudo make uninstallgateway
```

## Copyright

Most of the code is Copyright (C) by Jonathan Naylor G4KLX and his development team.

The some parts have addtional Copyright (C) by Thomas A. Early N7TAE
