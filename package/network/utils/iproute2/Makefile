#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=2.6.33
PKG_RELEASE:=100224

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=http://kernel.org/pub/linux/utils/net/iproute2/
#PKG_MD5SUM:=308e7145218dd552c2766fe527e239e1
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/network/utils
DL_DIR:=$(TOPDIR)/dl

ifeq ($(ZCFG_PLATFORM),MTKSOC)
PKG_VERSION:=3.15.0
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
#PKG_SOURCE_URL:=http://kernel.org/pub/linux/utils/net/iproute2/
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/network/utils
PKG_MD5SUM:=5b1711c9d16071959052e369a2682d77
PATCH_DIR:=patches-$(PKG_VERSION)/$(ZCFG_PLATFORM)
else ifeq ($(ZCFG_PLATFORM),QUECTEL)
PKG_VERSION:=3.15.0
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://kernel.org/pub/linux/utils/net/iproute2/
PKG_MD5SUM:=5b1711c9d16071959052e369a2682d77
PATCH_DIR:=patches-$(PKG_VERSION)/$(ZCFG_PLATFORM)
endif

PKG_BUILD_PARALLEL:=1
#PKG_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/iproute2-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/iproute2/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://linux-net.osdl.org/index.php/Iproute2
endef

define Package/ip
$(call Package/iproute2/Default)
  SUBMENU:=Routing and Redirection
  DEPENDS:= +libnl-tiny
  TITLE:=Routing control utility
endef

define Package/ip/conffiles
/etc/iproute2/rt_tables
endef

define Package/tc
$(call Package/iproute2/Default)
  TITLE:=Traffic control utility
  DEPENDS:=+kmod-sched-core
endef

define Package/genl
$(call Package/iproute2/Default)
  TITLE:=General netlink utility frontend
endef

define Package/ss
$(call Package/iproute2/Default)
  TITLE:=Socket statistics utility
endef

#define Build/Configure
#	$(SED) "s,-I/usr/include/db3,," $(PKG_BUILD_DIR)/Makefile
#	$(SED) "s,^KERNEL_INCLUDE.*,KERNEL_INCLUDE=$(LINUX_DIR)/include," \
#		$(PKG_BUILD_DIR)/Makefile
#	$(SED) "s,^LIBC_INCLUDE.*,LIBC_INCLUDE=$(STAGING_DIR)/include," \
#		$(PKG_BUILD_DIR)/Makefile
#	echo "static const char SNAPSHOT[] = \"ss-$(PKG_RELEASE)\";" \
		> $(PKG_BUILD_DIR)/include/SNAPSHOT.h
#endef

ifdef CONFIG_USE_GLIBC
  TARGET_CFLAGS += -DHAVE_SETNS
endif

ifdef CONFIG_TARGET_quectel
TARGET_CFLAGS += -DTARGET_quectel
endif

MAKE_FLAGS += \
	EXTRA_CCOPTS="$(TARGET_CFLAGS) -I../include -I$(STAGING_DIR)/usr/include/libnl-tiny" \
	KERNEL_DIR="$(LINUX_DIR)" \
	ARCH="$(LINUX_KARCH)" \
	FPIC="$(FPIC)"

MAKE_FLAGS += ZCFG_PLATFORM=ECONET
ifeq ("$(strip ${ZCFG_PLATFORM})","ECONET")
ifeq ("$(strip $(CONFIG_$(subst $\",,$(CONFIG_CUSTOM_SDK_PREFIX))_TCSUPPORT_3_18_21_KERNEL))","y")
MAKE_FLAGS += TCSUPPORT_3_18_21_KERNEL=y
endif
endif

define Build/Compile
	+$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/libnetlink.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libnetlink.a $(1)/usr/lib/
endef

define Package/ip/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/iproute2
#	$(INSTALL_DATA) $(PKG_BUILD_DIR)/etc/iproute2/rt_tables $(1)/etc/iproute2
	ln -s /var/iproute2/rt_tables $(1)/etc/iproute2/rt_tables
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ip/ip $(1)/usr/sbin/
endef

define Package/tc/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tc/tc $(1)/usr/sbin/
endef

define Package/genl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/genl/genl $(1)/usr/sbin/
endef

define Package/ss/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/misc/ss $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ip))
$(eval $(call BuildPackage,tc))
$(eval $(call BuildPackage,genl))
$(eval $(call BuildPackage,ss))
