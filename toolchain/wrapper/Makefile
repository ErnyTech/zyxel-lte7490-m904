#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wrapper
PKG_VERSION:=1

include $(INCLUDE_DIR)/toolchain-build.mk


# 1: args
define toolchain_util
$(strip $(SCRIPT_DIR)/ext-toolchain.sh --toolchain $(CONFIG_TOOLCHAIN_ROOT) \
	--cflags $(CONFIG_TARGET_OPTIMIZATION) \
	--cflags "$(if $(call qstrip,$(CONFIG_TOOLCHAIN_LIBC)),-m$(call qstrip,$(CONFIG_TOOLCHAIN_LIBC))) $(if $(CONFIG_SOFT_FLOAT),-msoft-float)" \
	--cflags "$(patsubst ./%,-I$(TOOLCHAIN_ROOT_DIR)/%,$(call qstrip,$(CONFIG_TOOLCHAIN_INC_PATH)))" \
	--cflags "$(patsubst ./%,-L$(TOOLCHAIN_ROOT_DIR)/%,$(call qstrip,$(CONFIG_TOOLCHAIN_LIB_PATH)))" \
	--topdir "$(TOPDIR)" \
	$(1))
endef

# 1: config symbol
# 2: feature
define toolchain_test
$$(if $$($(1)), \
	@echo -n "Testing external toolchain for $(2) support ... "; \
	if $(call toolchain_util,--test "$(2)"); then \
		echo "ok"; exit 0; \
	else \
		echo "failed"; \
		echo "ERROR: $(1) is enabled but the external toolchain does not support it"; \
		exit 1; \
	fi)
endef

# Workaround for using external toolchain (for BRCM 412L08 toolchain)
ifeq ($(CONFIG_TARGET_NAME),"mips-brcm412l08-linux-uclibc")
define Host/Prepare
endef

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	( \
	cd $(TOOLCHAIN_DIR); rm -fr bin usr lib; \
	ln -sf $(CONFIG_TOOLCHAIN_ROOT)/usr/bin; \
	ln -sf $(CONFIG_TOOLCHAIN_ROOT)/usr; \
	ln -sf $(CONFIG_TOOLCHAIN_ROOT)/lib; \
	)
endef

else
define Host/Prepare
	$(call toolchain_test,CONFIG_SOFT_FLOAT,softfloat)
	$(call toolchain_test,CONFIG_IPV6,ipv6)
	$(call toolchain_test,CONFIG_NLS,wchar)
	$(call toolchain_test,CONFIG_PACKAGE_libpthread,threads)
endef

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	$(call toolchain_util,--wrap "$(TOOLCHAIN_DIR)/bin")
endef
endif

define Host/Clean
	rm -rf $(TOOLCHAIN_DIR)/bin
endef

$(eval $(call HostBuild))
