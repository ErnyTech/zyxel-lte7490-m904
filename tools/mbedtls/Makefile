#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mbedtls
PKG_VERSION:=2.5.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:=git://android.git.kernel.org/platform/external/yaffs2.git
#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_SUBDIR:=$(PKG_NAME)
#PKG_SOURCE_VERSION:=d333fc232d7e5ae3370080d5d6f7d88ea9c6b3a1
HOST_BUILD_DIR=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/tools

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/ \
		CC="$(HOSTCC) $(HOST_STATIC_LINKING)" \
		CFLAGS="$(HOST_CFLAGS)"
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/programs/pkey/rsa_sign $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/rsa_sign
endef

$(eval $(call HostBuild))
