sudo apt update
# install redis
sudo apt install redis-server -y
# install redis for c/c++
sudo apt install libhiredis-dev
# open config redis, and add `bind 0.0.0.0 ::1` to allow access from the internet. Can change to access from lan only
sudo vim /etc/redis/redis.conf