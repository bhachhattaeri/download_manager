#! bin/sh
# /etc/init.d/DownloadManager
#

case "$1" in
start)
echo -n "Start service Daemon"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
  $DIR/.daemon
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
