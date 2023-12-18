#!/bin/sh
# Copyright (C) 2006-2013 OpenWrt.org
# Copyright (C) 2010 Vertical Communications

pivot() { # <new_root> <old_root>
	/bin/mount -o noatime,move /proc $1/proc && \
	pivot_root $1 $1$2 && {
		/bin/mount -o noatime,move $2/dev /dev
		/bin/mount -o noatime,move $2/tmp /tmp
		/bin/mount -o noatime,move $2/var /var
		/bin/mount -o noatime,move $2/sys /sys 2>&-
		/bin/mount -o noatime,move $2/overlay /overlay 2>&-
		return 0
	}
}

fopivot() { # <rw_root> <ro_root> <dupe?>
	/bin/mount -o noatime,lowerdir=/,upperdir=$1 -t overlayfs "overlayfs:$1" /mnt
	pivot /mnt $2
}

ramoverlay() {
	mkdir -p /tmp/root
	/bin/mount -t tmpfs -o noatime,mode=0755 root /tmp/root
	fopivot /tmp/root /rom 1
}
ramoverlay
