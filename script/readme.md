How to install
=====

1. Run script `install.sh`
1. Edit `/etc/redis/redis.conf`:
    1. open using vim: `sudo vim /etc/redis/redis.conf`
    1. add `bind 0.0.0.0 ::1` to the first line to allow access from the internet. Can modify to accept from lan only
    1. Restart redis service: `sudo systemctl restart redis`
