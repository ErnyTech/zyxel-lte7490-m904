CURRENT_DIR = $(shell pwd)
COMMON_ZCFG_INCLUDE_DIR = $(CURRENT_DIR)/../../../private/apps/zcfg/include

#PLATFORM=broadcom


LIBS_PATH = -L.
LINK_LIBS = -ljson -lcrypto -lzlog -lzos
LINK_LIBS += -lssl

INC_PATH = -I$(TOOLCHAIN)/include -I. -I$(COMMON_ZCFG_INCLUDE_DIR)
INC_PATH += -I./$(ZCFG_PLATFORM)
INC_PATH += -I./libzyutil_encrypt

ifeq ($(ZCFG_PLATFORM),BROADCOM)
ZCFLAGS += -DBCMTAG_EXE_USE

ALLOWED_INCLUDE_PATHS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)  \
                         -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
                         -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD) \
                         -I$(BUILD_DIR)/shared/broadcom/include/$(BRCM_BOARD) \
                         -I$(BUILD_DIR)/shared/opensource/include/$(BRCM_BOARD)

INC_PATH += $(ALLOWED_INCLUDE_PATHS)
endif

ifeq ($(ZCFG_PLATFORM),ECONET)
INC_PATH += -I$(STAGING_DIR)/usr/include/trx
ifneq ($(strip $(TCSUPPORT_MTD_ENCHANCEMENT)),)
ZCFLAGS+=-DTCSUPPORT_MTD_ENCHANCEMENT
endif
ifneq ($(strip $(TCSUPPORT_RESERVEAREA_BLOCK)),)
ZCFLAGS+=-DTCSUPPORT_RESERVEAREA_BLOCK=$(TCSUPPORT_RESERVEAREA_BLOCK)
endif
ifneq ($(strip $(TCSUPPORT_CT)),)
ZCFLAGS+=-DTCSUPPORT_CT
endif
ifneq ($(strip $(TCSUPPORT_NAND_BMT)),)
ZCFLAGS+=-DTCSUPPORT_NAND_BMT
endif
ifneq ($(strip $(TCSUPPORT_NAND_BADBLOCK_CHECK)),)
ZCFLAGS+=-DTCSUPPORT_NAND_BADBLOCK_CHECK
endif
ifneq ($(strip $(TCSUPPORT_PRODUCTIONLINE)),)
ZCFLAGS+=-DTCSUPPORT_PRODUCTIONLINE
endif
ifneq ($(strip $(TCSUPPORT_DUAL_WLAN_MT7612E)),)
ZCFLAGS+=-DTCSUPPORT_WLAN_AC
endif
ifneq ($(strip $(TCSUPPORT_DUAL_WLAN_MT7615E)),)
ZCFLAGS+=-DMT7615E
endif
ifneq ($(strip $(TCSUPPORT_WLAN_MT7615_11N)),)
ZCFLAGS+=-DMT7615_11N
endif
ifneq ($(strip $(TCSUPPORT_WLAN_MT7615D)),)
ZCFLAGS+=-DMT7615D -DMT7615_11N -DMT7615E -DTCSUPPORT_DUAL_WLAN -DTCSUPPORT_WLAN_AC
endif
ifneq ($(strip $(TCSUPPORT_DUAL_WLAN_MT7570)),)
ZCFLAGS+=-DTCSUPPORT_MT7570
endif
LINK_LIBS+= -lzcfg_msg
endif

ifneq ($(strip $(CONFIG_ZYXEL_RMA_FW_SUPPORT)),)
ZCFLAGS+=-DZYXEL_RMA_FW_SUPPORT
endif

# Support ip6tables NAT table
ifeq ($(CONFIG_MTKSOC_IP6TABLES_NAT_SUPPORT),y)
ZCFLAGS += -DMTKSOC_IP6TABLES_NAT_SUPPORT
endif

# Local Certificate for TR069
ifeq ($(CONFIG_ZYXEL_DEFAULT_LOCAL_CERTIFICATE),y)
ZCFLAGS += -DZYXEL_DEFAULT_LOCAL_CERTIFICATE
endif

#Support access password from MRD
ifeq ($(CONFIG_ZYXEL_RANDOM_PASSWORD_IN_MRD),y)
ZCFLAGS += -DZYXEL_RANDOM_PASSWORD_IN_MRD
endif

ifeq ($(ZCFG_PLATFORM),MTKSOC)
INC_PATH += -I$(STAGING_DIR)/usr/include/trx
endif
ifeq ($(ZCFG_PLATFORM),QUECTEL)
INC_PATH += -I$(STAGING_DIR)/usr/include/trx
endif

LIB_DEBUG_OBJS = debugcfg.o

ifneq ($(strip $(CONFIG_ZYXEL_IPV6_UPLOAD_DIAG_ENABLE)),)
ZCFLAGS+=-DZYXEL_IPV6_UPLOAD_DIAG_ENABLE
endif

ifeq ($(CONFIG_TARGET_econet_nr7102),y)
ZCFLAGS += -DZYXEL_NR7102
endif

ifeq ($(CONFIG_TARGET_quectel),y)
INC_PATH += -I$(STAGING_DIR)/usr/include
INC_PATH += -I$(TOPDIR)/tools/ramips/trx/src
LIBS_PATH += -L$(STAGING_DIR)/usr/lib
endif

ZCFLAGS += -DZYXEL=1 -std=gnu99
LIB_OBJS = common.o flash.o skconn.o zyutil_cofw.o
LIB_OBJS += zyutil_sfp.o

ifneq ($(strip $(ZYXEL_FEATURE_BITS)),)
LIB_OBJS += libzyutil_zyxelfeaturebits.o
endif

PKG_NAME = libzyutil
LIB_OBJS += $(ZCFG_PLATFORM)/$(PKG_NAME)_wrapper.o
ZCFLAGS += -D$(ZCFG_PLATFORM)_PLATFORM

LIB_OBJS += libzyutil_encrypt/libzyutil_encrypt.o
LIB_OBJS += libzyutil_encrypt/libzyutil_encrypt_SHA256.o
LIB_OBJS += libzyutil_encrypt/libzyutil_encrypt_MD5.o
LIB_OBJS += libzyutil_encrypt/aes.o

.PHONY : libzyutil

all: clean libzyutil libdebug.a

### Implicit rules ###
.c.o:
	$(CC) -c -fPIC $(CFLAGS) $(LIBS_PATH) $(INC_PATH) $< -o $@

libzyutil : $(LIB_OBJS)
	@echo $(CFLAGS) 
ifeq ($(CONFIG_TARGET_quectel),y)
	$(CC) -shared -o libzyutil.so $(LIB_OBJS) $(LINK_LIBS) $(LIBS_PATH)
else
	$(CC) -shared -o libzyutil.so $(LIB_OBJS) $(LINK_LIBS)
endif
	$(STRIP) libzyutil.so


libdebug.a : $(LIB_DEBUG_OBJS)
	$(AR) rs $@ $(LIB_DEBUG_OBJS)

install:
	install -m 666 zyutil.h $(COMMON_ZCFG_INCLUDE_DIR)
	install -m 666 zyutil_cofw.h $(COMMON_ZCFG_INCLUDE_DIR)
	install -m 666 zyutil_sfp.h $(COMMON_ZCFG_INCLUDE_DIR)

clean:
	rm -rf *.o
	rm -rf *.so
	rm -rf $(COMMON_ZCFG_INCLUDE_DIR)/zyutil.h
	rm -rf $(COMMON_ZCFG_INCLUDE_DIR)/zyutil_cofw.h
	rm -rf $(COMMON_ZCFG_INCLUDE_DIR)/zyutil_sfp.h

CURR_DIR:=$(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

ifeq ($(ZCFG_PLATFORM),BROADCOM)
include $(BUILD_DIR)/make.common
endif

CFLAGS+= $(ZCFLAGS) $(EXTRA_CFLAGS)
CFLAGS +=  -DZYXEL_PRODUCT_NAME='"$(PRODUCT_NAME)"'

ifeq ($(strip $(CONFIG_SUNRISE_RELEASE_DHCP_PPP_BEFORE_IFDOWN)),y)
	CFLAGS += -DSUNRISE_RELEASE_DHCP_PPP_BEFORE_IFDOWN
endif

ifeq ($(strip $(CONFIG_ELISA_CUSTOMIZATION)), y)
	CFLAGS += -DELISA_CUSTOMIZATION
endif

ifeq ($(strip $(CONFIG_ELISA_FW_NAMING_CUSTOMIZATION)), y)
	CFLAGS += -DELISA_FW_NAMING_CUSTOMIZATION
endif

ifeq ($(strip $(CONFIG_ZYXEL_HASH_ACCOUNT_PASSWORD_BY_SHA512)), y)
	CFLAGS += -DSHA512_PASSWD
endif

ifeq ($(strip $(CONFIG_WIND_ITALY_CUSTOMIZATION)), y)
	CFLAGS += -DWIND_ITALY_CUSTOMIZATION
endif
