#
# Copyright (c) 2020, Broadband Forum
# Copyright (c) 2020, AT&T Communications
#

include $(TOPDIR)/rules.mk

PKG_NAME:=obudpst
#PKG_VERSION:=7.2.1
#PKG_SOURCE_VERSION:=9c448095c9773bd7e5cea143af07817ad9f3c6ba
PKG_VERSION:=7.5.0
PKG_SOURCE_VERSION:=8cc26ba7d10f061fb744f075e60e842ebda84ea8
#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL:=https://github.com/BroadbandForum/obudpst.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/network/services
PKG_MIRROR_HASH:=skip

#Follwoing for MTKSOC platform initial development only, delete before release.
ifeq ($(ZCFG_PLATFORM),MTKSOC)
PATCHES_FILE_DIR:=mtksoc-patches
else
PATCHES_FILE_DIR:=nothing-to-copy
endif

ifeq ($(CONFIG_PACKAGE_zcmd),y)
TARGET_CFLAGS += -DZCFG_SUPPORT
endif


PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

LIBS_ZCFG+= +libzcfg_msg +libzcmd_tool +libjson +libzcfg_fe_rdm_access +libzcfg_fe_rdm_struct +libzcfg_fe_rdm_string +libzyutil +zcfg_be +quectel-DTool +libzcfg_fe_dal

#Disable Dependencies Check from libzcfg_fe_rdm_access Makefile
define CheckDependencies
endef

define Package/obudpst
	CATEGORY:=Utilities
	DEPENDS+=+libopenssl $(LIBS_ZCFG)
	TITLE:=Open Broadband-UDP Speed Test (OB-UDPST)
endef

define Build/Install
	:
endef

define Package/obudpst/description
	Open Broadband-UDP Speed Test (OB-UDPST) is a client/server software utility to demonstrate
	one approach of doing IP capacity measurements described by Broadband Forum TR-471.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	-$(CP) ./$(PATCHES_FILE_DIR)/* $(PKG_BUILD_DIR)
endef

define Build/Configure
  AR="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)ar" \
  AS="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC) -c $(TARGET_CFLAGS)" \
  LD="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)ld" \
  NM="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)nm" \
  CC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
  GCC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
  CXX="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)g++" \
  RANLIB="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)ranlib" \
  STRIP="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)strip" \
  OBJCOPY="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)objcopy" \
  OBJDUMP="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)objdump" \
  TARGET_CPPFLAGS="$(TARGET_CPPFLAGS)" \
  TARGET_CFLAGS="$(TARGET_CFLAGS)" \
  TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
  cmake -D DISABLE_INT_TIMER=ON $(PKG_BUILD_DIR)/CMakeLists.txt
endef

define Package/obudpst/install
	$(INSTALL_DIR) $(1)/usr/sbin
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/udpst $(1)/usr/sbin/obudpst
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udpst $(1)/usr/sbin/udpst
endef

$(eval $(call BuildPackage,obudpst))
