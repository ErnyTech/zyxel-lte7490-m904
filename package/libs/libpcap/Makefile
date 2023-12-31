#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpcap
#PKG_VERSION:=1.1.1
PKG_VERSION:=$(call qstrip,$(CONFIG_LIBPCAP_VERSION))
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:=http://www.tcpdump.org/release/
ifeq ($(strip $(CONFIG_LIBPCAP_VERSION)),"1.1.1")
PKG_MD5SUM:=1bca27d206970badae248cfa471bbb47
endif

ifeq ($(strip $(CONFIG_LIBPCAP_VERSION)),"1.9.1")
PKG_MD5SUM:=21af603d9a591c7d96a6457021d84e6c
endif

PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/libs

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

PATCH_DIR:=patches-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/libpcap
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Low-level packet capture library
  URL:=http://www.tcpdump.org/
  MENU:=1
endef

define Package/libpcap/description
This package contains a system-independent library for user-level network packet
capture.
endef

define Package/libpcap/config
	source "$(SOURCE)/Config.in"
endef

TARGET_CFLAGS += \
	-ffunction-sections \
	-fdata-sections

ifeq ($(strip $(CONFIG_LIBPCAP_VERSION)),"1.1.1")
CONFIGURE_VARS += \
	ac_cv_linux_vers=$(LINUX_VERSION)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-yydebug \
	--enable-ipv6 \
	--with-build-cc="$(HOSTCC)" \
	--with-pcap=linux \
	--without-septel \
	--without-dag \
	--without-libnl
endif

ifeq ($(strip $(CONFIG_LIBPCAP_VERSION)),"1.9.1")
CONFIGURE_VARS += \
	ac_cv_linux_vers=$(LINUX_VERSION) \
	ac_cv_header_libusb_1_0_libusb_h=no

ifeq ($(CONFIG_PCAP_HAS_NETFILTER),)
CONFIGURE_VARS += \
	ac_cv_netfilter_can_compile=no
endif

CONFIGURE_ARGS += \
	--enable-shared \
	--disable-yydebug \
	--with-pcap=linux \
	--without-septel \
	--without-dag \
	--without-libnl \
	--without-snf \
	--disable-can \
	--disable-canusb \
	--disable-dbus \
	--disable-bluetooth

ifeq ($(CONFIG_IPV6),y)
CONFIGURE_ARGS += \
	--enable-ipv6
endif
endif

ifeq ($(CONFIG_TARGET_quectel),y)
	TARGET_CFLAGS += -DTARGET_quectel
endif

MAKE_FLAGS += \
	CCOPT="$(TARGET_CFLAGS) -I$(BUILD_DIR)/linux/include"

define Build/Configure
	$(call Build/Configure/Default)
	$(if $(CONFIG_PCAP_HAS_USB),,$(SED) '/^#define PCAP_SUPPORT_USB/D' $(PKG_BUILD_DIR)/config.h)
	$(if $(CONFIG_PCAP_HAS_USB),,$(SED) 's/pcap-usb-linux.c *//' $(PKG_BUILD_DIR)/Makefile)
	$(if $(CONFIG_PCAP_HAS_BT),,$(SED) '/^#define PCAP_SUPPORT_BT/D' $(PKG_BUILD_DIR)/config.h)
	$(if $(CONFIG_PCAP_HAS_BT),,$(SED) 's/pcap-bt-linux.c *//' $(PKG_BUILD_DIR)/Makefile)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/pcap* $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpcap.{a,so*} $(1)/usr/lib/
endef

define Package/libpcap/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpcap.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libpcap))
