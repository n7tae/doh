# DOH - A DMR-Only Host

## Introduction

This will build a DMR-only Hotspot based on Jonathan Naylor's MMDVMHost software. It's quite a bit smaller than MMDVMHost as it **only** supports DMR modes. Also, it does not support *any* display device, but it does support an html dashboard. This supports two time slots, duplex operation and any MMDVM-based modem so it would be suitable for control of a DMR-only repeater.

If you want a pre-build, multi-mode hot-spot, then you want Andy Taylor MW0MWZ's excellent Pi-Star. If you want to try your hand at building your own multi-mode hot-spot, then you can take a look at the MMDVM.README file in https::/github.com/n7tae/QnetGateway.

## Prerequisites

DOH includes a dashboard with a last heard section. The last heard section uses SQLite3, a light-weight database, so you will need `libsqlite3-dev` to compile DOH, you will also need `curl` to be able to download the DMR ID lookup table:

```bash
sudo apt update
sudo apt install -y libsqlite3-dev curl
```

You will also need a few more things if you are going to *use* the dashboard:

```bash
sudo apt install -y php-common php-fpm sqlite3 php-sqlite3 dnsutils
```

Now you are ready to download DOH:

```bash
git clone https://github.com/n7tae/doh.git
```

## Script control

You can mostly ignore the sections below (it's still recommend that you read this entier README file) and do most everything you need to do regarding configuring and operating DOH *via* two scripts, `./admin` and `./config`. The *admin* script is used to maintain your cloned repository, compile the executable, install, restart and uninstall DOH, and, view the logs. You can also manage the dashboard and control DMRGateway, if you want either one of those features. The *config* script is used to maintain you DOH configuration file, `dmr.cfg`. In fact, you can launch `./config` from within `./admin`!

The *config* script is used to construct your `dmr.cfg` file, the configuration file for DOH. It allows you easily and quickly define connections to different DMR networks. It supports, XLX linking, DMR+, Brandmeister, HB Link, FreeDMR and TGIF. The idea here is that instead of using DMRGateway and connecting to multiple networks, you can quickly switch your direct connection to anywhere you want to go. You might that there is a simplicity to this approach. In fact, it greatly simplify your radio's code plug setup and one of the benefits is that you will be using the native TG groups from each network to which you connect, rather than having to remember to use a remapped TG that might be unique to your DMRGateway.ini file.

You can do everything from within *admin*. If you modify your `dmr.cfg` file from *admin* and DOH is already running, you will be asked if you want to restart DOH, making it incredibly easy to switch networks on the fly.

Of course, if you want to use DMRGateway, you can and the *admin* script supports that too.

## Building

### Move to the build directory and compile

```bash
cd doh
make
```

### Create and edit your configuration file

You can either manually create and edit your `doh.cfg` file, or you can use a menu-based script.

#### Use a script to create your configuration

In your build directory, type `./config`. The main menu reports some of the most important configuration values and also presents you with several sub-menus used to change all configuration values. When your ready, type `w` in the main menu to write you config file and exit the script. As you are changing values most input is examined to be sure your new values are reasonable. If not, the script will let you know.

#### Manually create you configuration file

```bash
cp dmr.ini dmr.cfg
```

Use your favorite file editor to edit your `dmr.cfg` file. You'll want to add your callsign and DMR ID in the `[General]` section and change the Duplex line if you are using a modem that supports duplex operation. Set your frequency in the `[Info]` section and you can also change other values in this section.

Check other values and change as needed. Make sure the `[Modem]` section is correct for your MMDVM-based modem. The `[DMR Network]` section is already defined as a **Direct** connection to the IPSC2-QUADNET DMR+ reflector. Of course you can connect to any system you want, so feel free to modify this section as you want. If you want to be able to access multiple DMR networks, you can use DMRGateway and connect to several systems, make sure you remove the active values in this `[DMR Network]` and uncomment the values for the `Type=Gateway` network.

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

## Optionally, install the dashboard

A simple dashboard can be installed. The php mini-web server is not robust enough for public access, but it's fine for operation behind a firewall. If you need a public webpage, you'll have to install this yourself.

It's easy to install:

```bash
sudo make installdash
```

The dashboard can be found at `<hotspot-hostname>.local`, or `<internal_ip_address>`.

If your system cannot find the dashboard with `<hotspot-hostname>.local`, try installing the avahi-daemon:

```bash
sudo apt install -y avahi-daemon
```

The dashboard uses a few simple configuration variable at the bottom of your dmr.cfg file. You can edit these anytime in your build directory and see the results immediately on your dashboard.

## Maintenance

When you installed `doh`, you downloaded the DMR ID look up table. If you want to freshen this table, do `sudo update-dmrid`. You system will automatically start using the new table within a day. If you want to start using it right away, then do a `sudo systemctl restart doh`.

## Monitoring

Until the dashboard is complete, you can keep an eye on `doh` with `sudo journalctl -u doh -f`.

## Uninstalling

```bash
sudo make uninstall
sudo make uninstallgateway
sudo make uninstalldash
```

## Copyright

Most of the code is Copyright (C) by Jonathan Naylor G4KLX and his development team.

The some parts have addtional Copyright (C) by Thomas A. Early N7TAE
