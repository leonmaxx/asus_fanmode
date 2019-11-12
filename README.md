asus_fanmode
============

**asus_fanmode** is a daemon that controls Fan Boost Mode on ASUS TUF series laptops. Based on CPU temperature readings this tool selects suitable fan mode using threshold values from configuration file.
  
Installation
------------
Install using make:
```
make
sudo make install
sudo systemctl enable asus_fanmode
sudo systemctl start asus_fanmode
```
  
Check status
------------
```
systemctl status asus_fanmode
```
  
View log:
```
journalctl -u asus_fanmode
```
  
Configuring
-----------
After installation configuration file is in `/etc/asus_fanmode.conf`. It have comments with option descriptions.  
Most likely on other laptops you'll need to change `cpu_temp` SysFS path. To find needed `hwmonX` use:
```
cat /sys/class/hwmon/hwmon*/name
```
On AMD Ryzen laptops needed one will have name `k10temp`. On Intel most likely `coretemp`.  
After you modify configuration file do not forget to restart service:
```
systemctl restart asus_fanmode
```
  
Laptop Models
-------------
Tested laptop models:  
* FX705DY - out of the box
* FX505DY - ?
