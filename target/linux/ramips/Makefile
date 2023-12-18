#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ramips
#BOARDNAME:=Ralink RT288x/RT3xxx
BOARDNAME:=MTK/Ralink APSoC (MIPS)
SUBTARGETS:=rt288x rt305x rt3883 mt7621 lte7240 lte7461 lte7480 lte7481 lte7490 nr7101 lte5388-s905
CFLAGS:=-Os -pipe -mips32r2 -mtune=mips32r2 -fno-caller-saves
FEATURES:=squashfs gpio

#LINUX_VERSION:=3.6.11
LINUX_VERSION:=3.10.14

include $(INCLUDE_DIR)/target.mk
#DEFAULT_PACKAGES+=\
	kmod-leds-gpio kmod-input-core kmod-input-polldev \
	kmod-input-gpio-keys-polled kmod-button-hotplug \
	kmod-rt2800-pci wpad-mini maccalc swconfig

define Target/Description
	Build firmware images for MTK/Ralink APSoC based boards.
endef

$(eval $(call BuildTarget))
