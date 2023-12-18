include $(TOPDIR)/rules.mk

PKG_NAME:=wakkama_dtls
PKG_VERSION:=1.1.0

#Follwoing for MTKSOC platform initial development only, delete before release.
ifeq ($(ZCFG_PLATFORM),MTKSOC)
PATCHES_FILE_DIR:=mtksoc-patches
else
PATCHES_FILE_DIR:=nothing-to-copy
endif

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/network/services
DL_DIR:=$(TOPDIR)/dl

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

PATCH_DIR=patches_$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk


#PKG_BUILD_DEPENDS:=+libmosquittopp +librt 


LIBS_ZCFG+= +libzcfg_msg +libzcmd_tool +libjson +libzcfg_fe_rdm_access +libzcfg_fe_rdm_struct +libzcfg_fe_rdm_string +libzyutil +libzcfg +zcfg_be +quectel-DTool

define Package/wakkama_dtls
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=Light Weight Machine to Machine
  DEPENDS:=+libz +openssl-util +clinkc +expat +libuuid +libopenssl +libcurl +libcares +libpthread +libdfota $(LIBS_ZCFG)
endef

define Package/wakkama_dtls/description
 wakkama is a system daemon providing a Light Weight Machine to Machine lwm2m protocol.
endef

#Disable Dependencies Check from libzcfg_fe_rdm_access Makefile
define CheckDependencies
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	-$(CP) ./$(PATCHES_FILE_DIR)/* $(PKG_BUILD_DIR)
endef

define Package/wakkama_dtls/install
	$(INSTALL_DIR) $(1)/usr/sbin
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/examples/bootstrap_server/bootstrap_server $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/examples/client/lwm2mclient $(1)/usr/sbin/lwm2mclient_dtls
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/examples/lightclient/lightclient $(1)/usr/sbin/lwm2mclient_dtls
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/examples/server/lwm2mserver $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/lwm2mclient.sh $(1)/usr/sbin/
endef	

$(eval $(call BuildPackage,$(PKG_NAME)))
