
include $(TOPDIR)/rules.mk

PKG_NAME := secure_header
#PKG_VERSION:=$(call qstrip,$(CONFIG_TC_SDK_VERSION))
#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=@ZyXEL_SITE/private/Trendchip/tools
#DL_DIR:=$(TOPDIR)/dl/private
#PATCH_DIR:=patches-$(PKG_VERSION)

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk
define Host/Compile
	-mkdir -p $(HOST_BUILD_DIR)/tools/$(PKG_NAME)
	$(CP) -dR ./src/* $(HOST_BUILD_DIR)/tools/$(PKG_NAME)
	$(MAKE) -C $(HOST_BUILD_DIR)/tools/$(PKG_NAME) \
                CC="$(HOSTCC)" \
                CXX="$(CXX) $(HOST_STATIC_LINKING)" \
		clean
	CFLAGS="-O2 -Wall" $(MAKE) -C $(HOST_BUILD_DIR)/tools/$(PKG_NAME) \
	        CC="$(HOSTCC)" \
                CXX="$(CXX) $(HOST_STATIC_LINKING)"
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/tools/$(PKG_NAME)/sheader $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/sheader
endef

$(eval $(call HostBuild))
