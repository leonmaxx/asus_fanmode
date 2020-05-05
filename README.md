asus_fanmode
============

**asus_fanmode** is a daemon that controls Fan Boost Mode on ASUS TUF series laptops. Based on CPU temperature readings this tool selects suitable fan mode using threshold values from configuration file.
  
This daemon requires Linux kernel version 5.6 or later. For older kernels you need to apply this two patches:  
1. [platform/x86: asus_wmi: Support throttle thermal policy](https://patchwork.kernel.org/patch/11292813/)
2. [platform/x86: asus_wmi: Set throttle thermal policy to default](https://patchwork.kernel.org/patch/11292815/)
  
For laptops which use `fan_boost_mode` sysfs entry, Linux kernel version 5.7 or later is required. For older kernels you need to apply following patch:  
1. [platform/x86: asus_wmi: Fix return value of fan_boost_mode_store](https://patchwork.kernel.org/patch/11419937/)
  
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
Note: Since version 0.3.0 daemon automatically searches for sysfs entries.  
  
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
* FX505DY - out of the box
