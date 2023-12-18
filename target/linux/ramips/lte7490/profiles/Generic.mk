#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Generic
	NAME:=Default Generic Profile
	PACKAGES:=\
		-swconfig -rt2x00 \
		ated hwnat reg gpio btnd switch ethstt uci2dat mii_mgr watchdog 8021xd eth_mac\
		wireless-tools block-mount fstools kmod-scsi-generic \
		kmod-usb-core kmod-usb3 kmod-usb-storage \
		kmod-fs-vfat kmod-fs-ntfs kmod-nls-base kmod-nls-utf8 kmod-nls-cp936 \
		kmod-nls-cp437 kmod-nls-cp850 kmod-nls-iso8859-1 kmod-nls-iso8859-15 kmod-nls-cp950
endef

define Profile/Generic/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,Generic))
