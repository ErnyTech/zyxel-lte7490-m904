#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=udhcp
PKG_VERSION:=0.9.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/network/services
DL_DIR:=$(TOPDIR)/dl

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
#Follwoing for MTKSOC platform initial development only, delete before release.
ifeq ($(ZCFG_PLATFORM),MTKSOC)
PATCHES_FILE_DIR:=mtksoc-patches
else ifeq ($(ZCFG_PLATFORM),QUECTEL)
ifneq ($(findstring $(PKG_NAME),$(USE_MTKSOC_PATCH)),)
$(warning Using mtksoc-patches on $(PKG_NAME) USE_MTKSOC_PATCH = $(USE_MTKSOC_PATCH))
USE_MTK=y
endif
PATCHES_FILE_DIR:=quectel-patches
else
PATCHES_FILE_DIR:=nothing-to-copy
endif


#LIBS_ZCFG = +libzcfg_msg +libzcmd_tool +libjson +libzcfg_fe_rdm_access +libzcfg_fe_rdm_struct

include $(INCLUDE_DIR)/package.mk

#Sunrise
ifeq ($(CONFIG_SUNRISE_RELEASE_DHCP_PPP_BEFORE_IFDOWN),y)
   SUNRISE_RELEASE_DHCP_PPP_BEFORE_IFDOWN = y
   export SUNRISE_RELEASE_DHCP_PPP_BEFORE_IFDOWN
endif
ifeq ($(CONFIG_SUNRISE_DHCP_FORCE_RENEW),y)
   SUNRISE_DHCP_FORCE_RENEW = y
   export SUNRISE_DHCP_FORCE_RENEW
endif
ifeq ($(CONFIG_SUNRISE_DHCP_OPTION58_OPTION59),y)
   SUNRISE_DHCP_OPTION58_OPTION59 = y
   export SUNRISE_DHCP_OPTION58_OPTION59
endif
ifeq ($(CONFIG_SUNRISE_VLANHUNT_CUSTOMIZATION),y)
   SUNRISE_VLANHUNT_CUSTOMIZATION = y
   export SUNRISE_VLANHUNT_CUSTOMIZATION
endif
ifeq ($(CONFIG_TARGET_quectel),y)
   export CONFIG_TARGET_quectel
endif
ifeq ($(CONFIG_PACKAGE_rilcmd),y)
   export CONFIG_PACKAGE_rilcmd
endif

define Package/udhcp
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=An embedded DHCP client and server
  DEPENDS:=$(LIBS_ZCFG)
endef

define Package/udhcp/config
	select PACKAGE_libzcfg_msg if PACKAGE_zcfg
	select PACKAGE_libzcmd_tool if PACKAGE_zcfg
	select PACKAGE_libjson if PACKAGE_zcfg
	select PACKAGE_libzcfg_fe_rdm_access if PACKAGE_zcfg
	select PACKAGE_libzcfg_fe_rdm_struct if PACKAGE_zcfg
endef
#Disable Dependencies Check
define CheckDependencies
endef

ifeq ($(CONFIG_TARGET_quectel),y)
define Build/Compile
	echo "apply udhcpc patch"
	rsync -rlt $(PATCHES_FILE_DIR)/ $(PKG_BUILD_DIR);
ifeq ($(USE_MTK), y)
	rsync -rlt mtksoc-patches/ $(PKG_BUILD_DIR);
endif
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CROSS)gcc -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -L$(STAGING_DIR)/usr/lib" \
		STRIP="$(TARGET_CROSS)strip" \
		INSTALL_DIR="$(BCM_INSTALL_DIR)" \
		INC_PATH="-I$(STAGING_DIR)/usr/include"
endef
else
define Build/Compile
	echo "apply udhcpc patch"
	rsync -rlt $(PATCHES_FILE_DIR)/ $(PKG_BUILD_DIR);

	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CROSS)gcc" \
		STRIP="$(TARGET_CROSS)strip" \
		INSTALL_DIR="$(BCM_INSTALL_DIR)"
endef
endif

define Package/udhcp/install
	$(INSTALL_DIR) $(1)/bin
	$(CP) $(PKG_BUILD_DIR)/udhcpc $(1)/bin
	$(TARGET_CROSS)strip $(1)/bin/udhcpc
endef

$(eval $(call BuildPackage,udhcp))
