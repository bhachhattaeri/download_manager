#! /bin/sh
# /etc/init.d/DownloadManager
#

case "$1" in
start)
echo -n "Start service Daemon"
  DownloadManager
;;
stop)
;;
restart)
;;
*)
echo "Usage: $0 {start|stop|restart}"
exit 1
;;
esac
