include  $(TOPDIR)/rules.mk

PKG_NAME:=syslog-ng
PKG_VERSION:=3.9.1
PKG_RELEASE:=1

PKG_MAINTAINER:=W. Michael Petullo <mike@flyn.org>

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/balabit/syslog-ng/releases/download/$(PKG_NAME)-$(PKG_VERSION)/
PKG_MD5SUM:=1b48da9ef620cf06e55e481b5abb677a
PKG_HASH:=5678856a550ae790618fabde9d1447f932ce7a9080d55dca8fc5df1202c70a17

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/syslog-ng_3.9.1
  SECTION:=admin
  CATEGORY:=Administration
  DEPENDS:=+libpcre +glib2 +libeventlog_0.2.12 +libopenssl +libuuid +libcurl +librt +libjson +libzcfg_fe_rdm_access +libzcfg_fe_rdm_struct +libzcfg_msg +libzcmd_tool
  TITLE:=A powerful syslog daemon
  URL:=http://www.balabit.com/network-security/syslog-ng/opensource-logging-system/
endef

define Package/syslog-ng_3.9.1/config
	select PACKAGE_libzcfg_msg if PACKAGE_zcfg
	select PACKAGE_libzcmd_tool if PACKAGE_zcfg
	select PACKAGE_libjson if PACKAGE_zcfg
	select PACKAGE_libzcfg_fe_rdm_access if PACKAGE_zcfg
	select PACKAGE_libzcfg_fe_rdm_struct if PACKAGE_zcfg
	select PACKAGE_libzcfg_fe_dal if PACKAGE_zcfg
endef

ifeq ("$(strip ${ZCFG_PLATFORM})","BROADCOM")
ifeq ("$(strip ${BRCM_SDK_VERSION})","502L04")
TARGET_CFLAGS += -DSUPPORT_502L04
endif
endif

define Package/syslog-ng_3.9.1/description
  syslog-ng reads and logs messages to the system console, log
  files, other machines and/or users as specified by its
  configuration file.
endef

define Package/syslog-ng_3.9.1/conffiles
  /etc/syslog-ng.conf
endef

define Build/Configure
	$(SED) 's,-I/usr/include,,' $(PKG_BUILD_DIR)/configure
	$(Build/Configure/Default)
endef

CONFIGURE_ARGS += \
  $(call autoconf_bool,CONFIG_IPV6,ipv6) \
         --disable-dependency-tracking \
         --disable-amqp \
         --disable-tcp-wrapper \
         --enable-glibtest=no \
         --disable-mongodb \
         --disable-java \
         --enable-json \
         --enable-ssl \
         --disable-python \
         --disable-spoof-source \
         --disable-sql \
         --disable-linux-caps \
         --disable-smtp \
         --disable-redis \
         --enable-prce \
         --disable-systemd \
		
#		LDFLAGS="-L$(STAGING_DIR)/usr/lib $(TARGET_LDFLAGS)" \
#		LIBS="-ljson -lzcfg_msg -lzcmd_tool -lzcfg_fe_rdm_access -lzcfg_fe_rdm_struct" \

export CONFIG_SYSLOG_NG_3.9.1=y

TARGET_LDFLAGS += \
  -Wl,-rpath-link="$(STAGING_DIR)/usr/lib" \

TARGET_CPPFLAGS += \
  -std=gnu99 \
  -I$(STAGING_DIR)/usr/include/eventlog

CONFIGURE_VARS += \
  LIBDBI_CFLAGS="-I$(STAGING_DIR)/usr/include"

#define Build/Configure
#	$(call Build/Configure/Default, \
#		--disable-dependency-tracking \
#		--sysconfdir=/etc \
#		--disable-spoof-source \
#		--disable-amqp \
#		--disable-tcp-wrapper \
#		--disable-mongodb \
#		--disable-java \
#		--disable-json \
#		--disable-python \
#		--disable-smtp \
#		--disable-redis \
#		--disable-sql \
#		--disable-linux-caps \
#		--enable-ipv6 \
#		--enable-dynamic-linking \
#		--enable-glibtest=no \
#		LIBS="-ljson -lzcfg_msg -lzcmd_tool -lzcfg_fe_rdm_access -lzcfg_fe_rdm_struct" \
#	)
#endef

#		CPPFLAGS="-I$(STAGING_DIR)/usr/include $(TARGET_CPPFLAGS)" \
#		LDFLAGS="-L$(STAGING_DIR)/usr/lib $(TARGET_LDFLAGS)" \
#		GLIB_CFLAGS="-I$(STAGING_DIR)/usr/include $(TARGET_CPPFLAGS)" \
#		GLIB_LIBS="$(STAGING_DIR)/usr/lib/libglib-2.0.so" \
#		EVTLOG_CFLAGS="-I$(STAGING_DIR)/usr/include/eventlog $(TARGET_CPPFLAGS)" \
#		EVTLOG_LIBS="$(STAGING_DIR)/usr/lib/libevtlog.so" 

define Package/syslog-ng_3.9.1/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(MAKE) -C $(PKG_BUILD_DIR) \
		install-sbinPROGRAMS install-libLTLIBRARIES \
		install-moduleLTLIBRARIES DESTDIR="$(1)"
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/syslog-ng $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/syslog-ng
	$(INSTALL_DATA) ./files/syslog-ng.conf $(1)/etc/syslog-ng/
	#$(INSTALL_DIR) $(1)/etc/syslog-ng/ca.d
	#$(INSTALL_DATA) ./files/syslog.crt $(1)/etc/syslog-ng/ca.d/
	#ln -sf /tmp/var/cert/1/cert.crt $(1)/tmp/var/cert/b12867d6.0
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/rc3.d
	$(CP) ./files/syslog-ng.sh $(1)/etc/init.d
	ln -s ../init.d/syslog-ng.sh $(1)/etc/rc3.d/S46syslog-ng
	$(CP) ./files/crond.sh $(1)/etc/init.d
	ln -s ../init.d/crond.sh $(1)/etc/rc3.d/S46crond
	$(call libtool_remove_files,$(1))
endef



#define Package/syslog-ng3/install
#	$(INSTALL_DIR) $(1)/usr/lib
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		install-sbinPROGRAMS install-libLTLIBRARIES \
#		install-moduleLTLIBRARIES DESTDIR="$(1)"
#	$(INSTALL_DIR) $(1)/etc/init.d
#	$(INSTALL_BIN) ./files/syslog-ng.init $(1)/etc/init.d/syslog-ng
#	$(CP) ./files/syslog-ng.sh $(1)/etc/init.d
#	$(INSTALL_DIR) $(1)/etc
#	$(INSTALL_DATA) ./files/syslog-ng.conf $(1)/etc
#	$(call libtool_remove_files,$(1))

#endef

$(eval $(call BuildPackage,syslog-ng_3.9.1))
