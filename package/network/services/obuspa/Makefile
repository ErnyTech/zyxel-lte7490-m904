include $(TOPDIR)/rules.mk

PKG_NAME:=obuspa
PKG_VERSION:=4.0.0

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
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/network/services
DL_DIR:=$(TOPDIR)/dl

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-master

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
#PKG_BUILD_DEPENDS:=PACKAGE_zcfg:esmd

PATCH_DIR=patches_$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

PKG_FIXUP:=autoreconf
CONFIG_SRC_TREE_OVERRIDE:=y


#PKG_BUILD_DEPENDS:=+libmosquittopp +librt 


LIBS_ZCFG+= +libzcfg_msg +libzcmd_tool +libjson +libzcfg_fe_rdm_access +libzcfg_fe_rdm_struct +libzcfg_fe_rdm_string +libzyutil +zcfg_be +quectel-DTool +libzcfg_fe_dal

define Package/obuspa
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=USP agent
  DEPENDS:=+libz +openssl-util +clinkc +expat +libuuid +libopenssl +libcurl +libsqlite3 +libcares +libmosquitto +libpthread +libdfota $(LIBS_ZCFG)
endef

define Package/obuspa/description
 OB-USP-AGENT is a system daemon providing a User Services Platform (USP) Agent.
endef

#Disable Dependencies Check from libzcfg_fe_rdm_access Makefile
define CheckDependencies
endef

export libmosquitto_CFLAGS=-I$(STAGING_DIR)/usr/include/ -I$(STAGING_DIR)/include
export libmosquitto_LIBS=-L$(STAGING_DIR)/usr/lib -lmosquitto 
export CONFIG_ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
export CONFIG_TELENOR_CUSTOMIZATION
export CONFIG_ZYXEL_SUPPORT_TR181_SHOW_APN_PASSWORD
ifeq ($(CONFIG_TELENOR_CUSTOMIZATION),y)
export Telenor_customization_CFLAG=-DTELENOR_CUSTOMIZATION 
endif

ifeq ($(CONFIG_ZYXEL_SUPPORT_TR181_SHOW_APN_PASSWORD),y)
export TR181_SHOW_APN_PASSWORD_CFLAG=-DZYXEL_SUPPORT_TR181_SHOW_APN_PASSWORD
endif

ifeq ($(CONFIG_ZYXEL_TR69_SUPPORT_RPC_OF_HTTPS),y)
export ZYXEL_TR69_SUPPORT_RPC_OF_HTTPS_CFLAG=-DZYXEL_TR69_SUPPORT_RPC_OF_HTTPS
endif

ifeq ($(CONFIG_ZYXEL_CONTROLLABLE_SEND_EVENT),y)
export ZYXEL_CONTROLLABLE_SEND_EVENT=-DZYXEL_CONTROLLABLE_SEND_EVENT
endif

CONFIGURE_ARGS += \
        --enable-coap \
        --enable-stomp \
        --enable-mqtt \		
        --with-build-cc="$(HOSTCC)"		
	
define Build/Prepare
	$(call Build/Prepare/Default)	
	-$(CP) ./$(PATCHES_FILE_DIR)/* $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR);$(PKG_FIXUP) --force --install
endef		


define Package/obuspa/install
	#$(INSTALL_DIR) $(1)/var/obuspa/
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/obuspa
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/obuspa $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/obuspa.sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
