include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=logrotate
PKG_VERSION:=3.7.1
PKG_RELEASE:=1

#Follwoing for MTKSOC platform initial development only, delete before release.
ifeq ($(ZCFG_PLATFORM),MTKSOC)
PATCHES_FILE_DIR:=mtksoc-patches
else ifeq ($(ZCFG_PLATFORM),QUECTEL)
ifneq ($(findstring $(PKG_NAME),$(USE_MTKSOC_PATCH)),)
$(warning Using mtksoc-patches on $(PKG_NAME) USE_MTKSOC_PATCH = $(USE_MTKSOC_PATCH))
PATCHES_FILE_DIR:=mtksoc-patches
else
$(warning Using quectel-patches on $(PKG_NAME) USE_MTKSOC_PATCH = $(USE_MTKSOC_PATCH))
PATCHES_FILE_DIR:=quectel-patches
endif
else
PATCHES_FILE_DIR:=nothing-to-copy
endif

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/utils
DL_DIR:=$(TOPDIR)/dl

PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/logrotate
SECTION:=base
CATEGORY:=Base system
TITLE:=Utility which generates a lot of log files
DESCRIPTION:=
DEPENDS:=+libpopt
endef
ifeq ($(strip $(CONFIG_ISKON_CUSTOMIZATION)),y)
ADSLINFOSTR= echo "*/15 * * * * /bin/xdslctl info --ikstats > /var/adslinfo" >> $(1)/etc/crontab; 
else ifeq ($(strip $(CONFIG_TELIA_CUSTOMIZATION)),y)
ADSLINFOSTR= echo "*/5 * * * * /usr/sbin/logrotate /etc/logrotate.d/messages.conf -s /var/log/rotate.status" >> $(1)/etc/crontab;
else
ADSLINFOSTR= echo "";
endif

ifeq ($(strip $(CONFIG_TELIA_CUSTOMIZATION)),y)
CELLULARSTR= echo "0 0 * * * /usr/sbin/logrotate -f /etc/logrotate.d/messages.conf -s /var/log/rotate.status" >> $(1)/etc/crontab;
else
CELLULARSTR=
endif

ifeq ($(CONFIG_TARGET_quectel),y)
export CONFIG_TARGET_quectel
endif

#define Build/Compile
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		CC="$(TARGET_CROSS)gcc"
#endef

define Package/logrotate/install
#Follwoing for MTKSOC platform initial development only, delete before release.
ifneq ($(ZCFG_PLATFORM),QUECTEL)
	-$(CP) ./$(PATCHES_FILE_DIR)/* $(PKG_BUILD_DIR)/
endif
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/logrotate $(1)/usr/sbin
	if [ ! -d $(1)/etc/logrotate.d ]; then \
		mkdir -m 755 -p $(1)/etc/logrotate.d ; \
	fi
	install -m 755 $(PKG_BUILD_DIR)/zyxel.etc/sys_logrotate.conf $(1)/etc/logrotate.d
	install -m 755 $(PKG_BUILD_DIR)/zyxel.etc/voip_logrotate.conf $(1)/etc/logrotate.d
ifeq ($(strip $(CONFIG_TELIA_CUSTOMIZATION)),y)	
	install -m 755 $(PKG_BUILD_DIR)/zyxel.etc/messages.conf $(1)/etc/logrotate.d
endif
	if [ ! -f $(1)/etc/crontab ]; then \
		touch $(1)/etc/crontab ; \
	fi
ifeq ($(strip $(CONFIG_ZYXEL_DEVICEINFO_VENDOR_LOG_SAVE_TO_FLASH)),y)	
	install -m 755 $(PKG_BUILD_DIR)/zyxel.etc/force_logrotate.conf $(1)/etc/logrotate.d
	if [ -z "`cat $(1)/etc/crontab | grep sys_logrotate.conf`" ]; then \
		$(ADSLINFOSTR) \
	fi
else
	if [ -z "`cat $(1)/etc/crontab | grep sys_logrotate.conf`" ]; then \
		echo "*/5 * * * * /usr/sbin/logrotate /etc/logrotate.d/sys_logrotate.conf " >> $(1)/etc/crontab; \
		$(ADSLINFOSTR) \
		$(CELLULARSTR) \
	fi
	echo "*/1 * * * * sh /usr/sbin/drop_caches.sh" >> $(1)/etc/crontab;
endif	

endef

$(eval $(call BuildPackage,logrotate))

