#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=popt
#PKG_VERSION:=1.7
#PKG_RELEASE:=5
PKG_VERSION:=1.16
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
#PKG_SOURCE_URL:=http://rpm5.org/files/popt/
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/libs
#PKG_MD5SUM:=5988e7aeb0ae4dac8d83561265984cc9
PKG_MD5SUM:=3743beefa3dd6247a73f8f7a32c14c33

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh aclocal.m4

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/libpopt
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A command line option parsing library
  URL:=http://rpm5.org/files/popt/
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/popt.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpopt.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/popt.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libpopt/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpopt.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libpopt))

