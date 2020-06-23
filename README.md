# ftn
a wrapper for tonos-cli

## Installation
Please make sure that your user is sudoers and can use sudo.
To install this wrapper run the following command
```sh
cd ~
git clone https://github.com/user529/ftn
cd ftn
chmod 755 ftnautoconf
./ftnautoconf
```
Now you can use it! Try
```sh
ftn help
```
If you want to tweak the ftn configuration, use /etc/ftn.conf

## Update
If you already have ftn installed and want to keep it up to date, just follow these a few steps
```sh
cd ~/ftn
git pull master
```