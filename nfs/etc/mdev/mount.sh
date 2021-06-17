#!/bin/sh
#
# Called from udev
# Attemp to mount any added block devices 
# and remove any removed devices
#

MOUNT="/bin/mount"
PMOUNT="/usr/bin/pmount"
UMOUNT="/bin/umount"
ACTION=$1
name=$2
DEVNAME="/dev/$name"

automount() {	
	! test -d "/media/$name" && mkdir -p "/media/$name"
	
	if ! $MOUNT -t auto -o sync $DEVNAME "/media/$name"
	then
		rm_dir "/media/$name"
	else
		echo "Auto-mount of [/media/$name] successful" > /dev/console

		touch "/tmp/.automount-$name"
	fi
}
	
rm_dir() {
	# We do not want to rm -r populated directories
	if test "`find "$1" | wc -l | tr -d " "`" -lt 2 -a -d "$1"
	then
		! test -z "$1" && rm -r "$1"
	else
		logger "mount.sh/automount" "Not removing non-empty directory [$1]"
	fi
}

if [ $ACTION == "add" ] && [ -n "$DEVNAME" ]; then
	if [ -x $MOUNT ]; then
    		$MOUNT $DEVNAME 2> /dev/null
	fi

	# If the device isn't mounted at this point, it isn't configured in fstab
	cat /proc/mounts | awk '{print $1}' | grep -q "^$DEVNAME$" || automount 
	
fi

if [ $ACTION == "remove" ] && [ -x "$UMOUNT" ] && [ -n "$DEVNAME" ]; then
	for mnt in `cat /proc/mounts | grep "$DEVNAME" | cut -f 2 -d " " `
	do
		$UMOUNT $mnt
	done
	
	# Remove empty directories from auto-mounter
	test -e "/tmp/.automount-$name" && rm_dir "/media/$name"
