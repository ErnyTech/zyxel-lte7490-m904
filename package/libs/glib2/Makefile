#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=glib2
PKG_VERSION:=2.37.7
PKG_RELEASE:=1

PKG_SOURCE:=glib-$(PKG_VERSION).tar.xz
PKG_BUILD_DIR:=$(BUILD_DIR)/glib-$(PKG_VERSION)
PKG_SOURCE_URL:=@ZyXEL_SITE/opensource/libs
#PKG_SOURCE_URL:=@GNOME/glib/2.37
#PKG_MD5SUM:=17a9493e3e2efebc8ab81ec8d4480bfb

ifeq ($(CONFIG_TARGET_quectel), y)
PKG_BUILD_DEPENDS:= libpthread zlib libintl libffi
else
PKG_BUILD_DEPENDS:=glib2/host libpthread zlib libintl libffi
endif
HOST_BUILD_DEPENDS:=libintl/host libiconv/host libffi/host
PKG_INSTALL:=1
PKG_USE_MIPS16:=0

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/glib-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

ifeq ($(CONFIG_TARGET_quectel), y)
TARGET_CFLAGS += -Wno-format-nonliteral -Wno-implicit-function-declaration -DTARGET_quectel
endif

define Package/glib2
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=$(ICONV_DEPENDS) $(INTL_DEPENDS) +zlib +libpthread +libffi
  TITLE:=glib 2.0
  URL:=http://www.gtk.org/
endef

define Package/glib2/description
  The GLib library of C routines
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		--enable-debug=no \
		--disable-selinux \
        --disable-fam \
        $(if $(ICONV_FULL),--with-libiconv=gnu) \
		, \
		glib_cv_stack_grows=no \
		glib_cv_uscore=no \
		ac_cv_path_GLIB_GENMARSHAL=$(STAGING_DIR_HOST)/bin/glib-genmarshal \
		ac_cv_func_mmap_fixed_mapped=yes \
		ac_cv_func_posix_getpwuid_r=yes \
		ac_cv_func_posix_getgrgid_r=yes \
	);
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/glib-2.0 \
		$(1)/usr/include/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/glib-2.0/include/*.h \
		$(1)/usr/include/glib-2.0/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/gio-unix-2.0 \
		$(1)/usr/include/

	$(CP) $(PKG_BUILD_DIR)/glib/glib.h $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/glib/glibconfig.h $(STAGING_DIR)/usr/include
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/glib
	$(CP) $(PKG_BUILD_DIR)/glib/*.h $(STAGING_DIR)/usr/include/glib
	$(CP) $(PKG_BUILD_DIR)/glib/deprecated $(STAGING_DIR)/usr/include/glib/
		
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/glib-2.0 \
		$(1)/usr/lib/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.{so*,a,la} \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig

	$(INSTALL_DIR) $(2)/share/aclocal/
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/share/aclocal/*.m4  \
		$(2)/share/aclocal/
endef

define Package/glib2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,glib2))
