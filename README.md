asus_fanmode
============

**asus_fanmode** is a daemon that controls Fan Boost Mode on ASUS TUF series laptops. Based on CPU temperature readings this tool selects suitable fan mode using values from configuration file.
  
Installation
------------
```
make
sudo make install
sudo systemctl enable asus_fanmode
sudo systemctl start asus_fanmode
```
  
Configuring
-----------
After installation configuration file is in `/etc/asus_fanmode.conf`. It have comments with option descriptions.
  
Laptop Models
-------------
Tested laptop models:  
* FX705DY
* FX505DY
