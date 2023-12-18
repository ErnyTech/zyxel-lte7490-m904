#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bluez-tools
#PKG_VERSION:=0.1.38-662e
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME).tar.xz

PKG_LICENSE:=GPL-2.0+
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk



define Package/bluez-tools
  TITLE:=Bluetooth
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= tools
  DEPENDS:= bluez-daemon +libreadline
endef

CONFIGURE_ARGS += \
	--disable-dependency-tracking


CONFIGURE_VARS += \
	LIBS="-lncurses "

TARGET_CPPFLAGS += \
	-std=gnu99


define Package/bluez-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/bt-agent $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/bt-adapter $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/bt-device $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/bt-network $(1)/usr/bin/
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/bt-obex $(1)/usr/bin/
endef

$(eval $(call BuildPackage,bluez-tools))
