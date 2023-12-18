#
# Copyright (C) 2009 OpenWrt.org
#

SUBTARGET:=lte7490
BOARDNAME:=LTE7490 Generic Outdoor LTE CPE
ARCH_PACKAGES:=ramips_24kec
FEATURES+=usb ubifs nand
CPU_TYPE:=24kec
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES +=

define Target/Description
	Build firmware images for LTE7490 Generic Outdoor LTE CPE.
endef

