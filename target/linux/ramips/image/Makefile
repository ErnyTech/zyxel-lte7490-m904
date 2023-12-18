#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
#include ./ramips-image.mk

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)
endef

define sysupname
$(call imgname,$(1),$(2))-sysupgrade.bin
endef

VMLINUX:=$(IMG_PREFIX)-vmlinux
UIMAGE:=$(IMG_PREFIX)-uImage

#add TRX image temperal name for ZYXEL TRX usage
#TRX_TMP_NAME:=zytrximg
#TRX_IMG_OPT:=-u $(START_ADDR) (reserve)

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE),initramfs)
endef
endif

ifeq ($(CONFIG_SOC_RT288X),y)
define kernel_entry
-a 0x88000000 -e 0x88000000
endef
else
ifeq ($(CONFIG_RALINK_MT7621),y)
define kernel_entry
-a 0x80001000 -e 0x80001000
endef
else
define kernel_entry
-a 0x80000000 -e 0x80000000
endef
endif
endif

define MkCombineduImage
	$(call PatchKernelLzma,$(2),$(3))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt `expr $(4) - 64` ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big" >&2; \
	else if [ `stat -c%s "$(KDIR)/root.$(1)"` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big" >&2; \
	else \
		( dd if=$(KDIR)/vmlinux-$(2).bin.lzma bs=`expr $(4) - 64` conv=sync ; dd if=$(KDIR)/root.$(1) ) > $(KDIR)/vmlinux-$(2).bin.lzma.combined ; \
	fi ; fi
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(2).bin.lzma.combined,$(call sysupname,$(1),$(2)),$(6))
endef


#
# The real magic happens inside these templates
#

define MkImage
	$(eval imagename=$(if $(4),$(4),OpenWrt Linux-$(LINUX_VERSION)))
	-mkimage -A mips -O linux -T kernel -C $(1) $(call kernel_entry)\
		-n "$(imagename)" \
		-d $(2) $(3)
endef

define CompressLzma
	$(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define MkImageSysupgrade/squashfs
	$(eval output_name=$(IMG_PREFIX)-$(2)-$(1)-$(if $(4),$(4),sysupgrade).bin)
	cat $(KDIR)/vmlinux-$(2).uImage $(KDIR)/root.$(1) > $(KDIR)/$(output_name)
	$(call prepare_generic_squashfs,$(KDIR)/$(output_name))
	if [ `stat -c%s "$(KDIR)/$(output_name)"` -gt $(3) ]; then \
		echo "Warning: $(KDIR)/$(output_name) is too big" >&2; \
	else \
		$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name); \
	fi
endef

define PatchKernelLzmaDtb
	cp $(KDIR)/vmlinux$(3) $(KDIR)/vmlinux-$(1)$(3)
	#$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $(KDIR)/$(2).dtb ../dts/$(2).dts
	#$(STAGING_DIR_HOST)/bin/patch-dtb $(KDIR)/vmlinux-$(1)$(3) $(KDIR)/$(2).dtb
	$(call CompressLzma,$(KDIR)/vmlinux-$(1)$(3),$(KDIR)/vmlinux-$(1)$(3).bin.lzma)
endef

define MkImageLzmaDtb
	$(call PatchKernelLzmaDtb,$(1),$(2),$(4))
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(1)$(4).bin.lzma,$(KDIR)/vmlinux-$(1)$(4).uImage,$(3))
endef


ifeq ($(CONFIG_ZYXEL_ENCRYPTED_FIRMWARE_UPGRADE),y)

ENC_OUT_DIR=$(BIN_DIR)/encrypt
EN_PRIVKEY_PA=$(ENC_OUT_DIR)/passphase.key
EN_PRIVKEY=$(ENC_OUT_DIR)/encPrivateKey.pem
SHASUM=`/usr/bin/sha256sum $(EN_PRIVKEY_PA)|cut -d ' ' -f 1`
HASHPASS='QSTFx7GrH4rkdNDpbNaOgz4PeqPYekseZgga7c30HDLIWyfijeaAiQGjvrGWHQCHJzg5GsJOZuXdOkxPIFl7OqybVq4nWiEOPXneelZgMgbIZU2UeHV8pdigMyB4kmWV'
define 	MkEncryptFWimage
#may placed different server
	@echo "load image=$(1)"
	@if [ -d $(ENC_OUT_DIR) ]; then 	\
		rm -f $(EN_PRIVKEY) $(EN_PRIVKEY_PA); \
	else \
		$(INSTALL_DIR) $(ENC_OUT_DIR) ; \
	fi
#remove temp file
	-rm -f $(ENC_OUT_DIR)/SignedRandKey $(ENC_OUT_DIR)/RandKey $(ENC_OUT_DIR)/privatekey.pem $(ENC_OUT_DIR)/Signras_hash $(ENC_OUT_DIR)/Signedpubkey_hash $(ENC_OUT_DIR)/encRAS
#download private key,need modify
	@svn export --force $(CONFIG_ZYXEL_ENCRYPTED_FIRMWARE_KEY_URL) $(ENC_OUT_DIR)/
	@svn export --force $(CONFIG_ZYXEL_ENCRYPTED_FIRMWARE_PASS_URL) $(ENC_OUT_DIR)/
#we use file checksum as passphase to decrypt private key
	openssl enc -d -aes256 -in $(EN_PRIVKEY) -pass pass:$(SHASUM) > $(ENC_OUT_DIR)/privatekey.pem
#generte random string, note!!!! makefile will excute first
	$(eval RADKEY=$(shell cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 128 | head -n 1))
#Calculate hash of image
	openssl dgst -binary -sha512 < $(1) > $(ENC_OUT_DIR)/ras_hash
#sign the hash of image
	openssl rsautl -sign -inkey $(ENC_OUT_DIR)/privatekey.pem -keyform PEM -pkcs -in $(ENC_OUT_DIR)/ras_hash -out $(ENC_OUT_DIR)/Signras_hash	
	$(CP) $(1) $(ENC_OUT_DIR)/encRAS
	cat $(ENC_OUT_DIR)/Signras_hash >> $(ENC_OUT_DIR)/encRAS
#openssl enc -aes-256-cbc -md md5 -in $(1) -out $(ENC_OUT_DIR)/encRAS -pass pass:$(RADKEY)
	@echo $(RADKEY) > $(ENC_OUT_DIR)/RandKey
#encypt public key hash
	openssl rsa -in $(ENC_OUT_DIR)/privatekey.pem -pubout -out $(ENC_OUT_DIR)/publickey.pem
	openssl sha -sha512 < $(ENC_OUT_DIR)/publickey.pem > $(ENC_OUT_DIR)/pubkey_hash
	openssl enc -base64 -md md5 -in $(ENC_OUT_DIR)/pubkey_hash -out $(ENC_OUT_DIR)/Signedpubkey_hash 
	-rm -f $(ENC_OUT_DIR)/publickey.pem $(ENC_OUT_DIR)/pubkey_hash
#sign random key
	openssl rsautl -sign -in $(ENC_OUT_DIR)/RandKey -inkey $(ENC_OUT_DIR)/privatekey.pem | openssl base64 -e -out $(ENC_OUT_DIR)/SignedRandKey
	-rm  -f $(EN_PRIVKEY_PA) $(EN_PRIVKEY) $(ENC_OUT_DIR)/privatekey.pem
endef
	
endif

#add TRX image for ZYXEL TRX usage
define BuildFirmware/OF
	$(call MkImageLzmaDtb,$(2),$(3),$(5))
	$(call MkImageSysupgrade/$(1),$(1),$(2),$(4),$(6))
	$(call Image/Build/zy_trx_image_wo_rootfs_defconfig,$(KDIR)/$(output_name))
endef

#add TRX image for ZYXEL TRX usage
define BuildFirmware/OF/initramfs
	$(call MkImageLzmaDtb,$(2),$(3),$(4),-initramfs)
	$(CP) $(KDIR)/vmlinux-$(2)-initramfs.uImage $(call imgname,$(1),$(2))-uImage.bin
ifeq ($(CONFIG_USE_ZYXEL_FW_SIGNATURE),y)
	$(call Image/Build/Signature,$(KDIR)/vmlinux-$(2)-initramfs.uImage)
	$(call Image/Build/zy_trx_image_wo_rootfs_defconfig,$(KDIR)/vmlinux-$(2)-initramfs.uImage.rsa)
else
	$(call Image/Build/zy_trx_image_wo_rootfs_defconfig,$(KDIR)/vmlinux-$(2)-initramfs.uImage)
endif

ifeq ($(CONFIG_ZYXEL_ENCRYPTED_FIRMWARE_UPGRADE),y)
	$(eval output_name=$(KDIR)/vmlinux-$(2)-initramfs.uImage)
	$(eval enc_root_name=$(ENC_OUT_DIR)/encRAS)
	$(eval signKey_name=$(ENC_OUT_DIR)/Signras_hash)
	$(call MkEncryptFWimage,$(output_name))
	$(call Image/Build/zy_trx_image_wo_rootfs_defconfig_encrypted,$(output_name),$(enc_root_name),$(signKey_name))
endif
endef


# Build images for default ralink layout for 4MB flash
# kernel + roots = 0x3b0000
# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
ralink_default_fw_size_4M=3866624
BuildFirmware/Default4M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_4M),$(4))
BuildFirmware/Default4M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))

# Build images for default ralink layout for 8MB flash
# kernel + roots = 0x7b0000
# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
ralink_default_fw_size_8M=8060928
BuildFirmware/Default8M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_8M),$(4))
BuildFirmware/Default8M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))

ralink_default_fw_size_16M=16121856
BuildFirmware/Default16M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_16M),$(4))
BuildFirmware/Default16M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))

ralink_default_fw_size_32M=33554432
BuildFirmware/Default32M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_32M),$(4))
BuildFirmware/Default32M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))

# Build images for a custom sized flash layout
# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
# $(4) = kernel + rootfs size
BuildFirmware/CustomFlash/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(4),$(5),$(6))
BuildFirmware/CustomFlash/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# wrappers for boards that have 4MB and 8MB versions
define BuildFirmware/DefaultDualSize/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2)-4M,$(3)-4M)
	$(call BuildFirmware/Default8M/$(1),$(1),$(2)-8M,$(3)-8M)
endef
define BuildFirmware/DefaultDualSize/initramfs
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-4M,$(3)-4M)
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-8M,$(3)-8M)
endef

# Some boards need a special header inside the uImage to make them bootable
define BuildFirmware/CustomFlashFactory/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(4))
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(4),$(5),$(6))
endef
BuildFirmware/CustomFlashFactory/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

#
# RT3662/RT3883 Profiles
#

# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
# $(4) = kernel size
# $(5) = rootfs size
# $(6) = signature
# $(7) = model
# $(8) = flash address
# $(9) = start address

#
# MT7620 Profiles
#


Image/Build/Profile/MT7620=$(call BuildFirmware/Default8M/$(1),$(1),mt7620,MT7620)

ifeq ($(SUBTARGET),mt7620)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/MT7620,$(1))
endef
endif


#
# MT7621 Profiles
#

Image/Build/Profile/MT7621=$(call BuildFirmware/Default16M/$(1),$(1),mt7621,MT7621)

ifeq ($(SUBTARGET),mt7621)
CHIP_ID:=MT7621A
define Image/Build/Profile/Default
	$(call Image/Build/Profile/MT7621,$(1))
endef
endif

#
# LTE7240 Profiles
#

Image/Build/Profile/LTE7240=$(call BuildFirmware/Default16M/$(1),$(1),lte7240,MT7621)

ifeq ($(SUBTARGET),lte7240)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
        $(call Image/Build/Profile/LTE7240,$(1))
endef
endif

#
# LTE7461 Profiles
#

Image/Build/Profile/LTE7461=$(call BuildFirmware/Default16M/$(1),$(1),lte7461,MT7621)

ifeq ($(SUBTARGET),lte7461)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
        $(call Image/Build/Profile/LTE7461,$(1))
endef
endif

#
# LTE7480 Profiles
#

Image/Build/Profile/LTE7480=$(call BuildFirmware/Default16M/$(1),$(1),lte7480,MT7621)

ifeq ($(SUBTARGET),lte7480)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
        $(call Image/Build/Profile/LTE7480,$(1))
endef
endif

#
# LTE5388-S905 Profiles
#

Image/Build/Profile/LTE5388-S905=$(call BuildFirmware/Default16M/$(1),$(1),lte5388-s905,MT7621)

ifeq ($(SUBTARGET),lte5388-s905)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
        $(call Image/Build/Profile/LTE5388-S905,$(1))
endef
endif

#
# LTE7481 Profiles
#

Image/Build/Profile/LTE7481=$(call BuildFirmware/Default16M/$(1),$(1),lte7481,MT7621)

ifeq ($(SUBTARGET),lte7481)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
        $(call Image/Build/Profile/LTE7481,$(1))
endef
endif

#
# LTE7490 Profiles
#

Image/Build/Profile/LTE7490=$(call BuildFirmware/Default16M/$(1),$(1),lte7490,MT7621)

ifeq ($(SUBTARGET),lte7490)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
	$(call Image/Build/Profile/LTE7490,$(1))
endef
endif

#
# NR7101 Profiles
#

Image/Build/Profile/NR7101=$(call BuildFirmware/Default16M/$(1),$(1),nr7101,MT7621)

ifeq ($(SUBTARGET),nr7101)
CHIP_ID:=MT7621A
define Image/Build/Profile/Generic
	$(call Image/Build/Profile/NR7101,$(1))
endef
endif

#
# MT7628 Profiles
#

Image/Build/Profile/MT7628=$(call BuildFirmware/Default8M/$(1),$(1),mt7628,MT7628)

ifeq ($(SUBTARGET),mt7628)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/MT7628,$(1))
endef
endif


#
# MT7688 Profiles
#

Image/Build/Profile/MT7688=$(call BuildFirmware/Default8M/$(1),$(1),mt7688,MT7688)

ifeq ($(SUBTARGET),mt7688)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/MT7688,$(1))
endef
endif


#
# Generic Targets
#

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(VMLINUX).elf
	cp $(KDIR)/vmlinux $(BIN_DIR)/$(VMLINUX).bin
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux.bin.lzma,$(KDIR)/uImage.lzma)
	cp $(KDIR)/uImage.lzma $(BIN_DIR)/$(UIMAGE).bin
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	cp $(KDIR)/vmlinux.elf $(KDIR)/vmlinux-initramfs.elf
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-initramfs
	cp $(KDIR)/vmlinux-initramfs.elf $(BIN_DIR)/$(VMLINUX)-initramfs.elf
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(VMLINUX)-initramfs.elf
	$(call CompressLzma,$(KDIR)/vmlinux-initramfs,$(KDIR)/vmlinux-initramfs.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux-initramfs.bin.lzma,$(KDIR)/uImage-initramfs.lzma)
	cp $(KDIR)/uImage-initramfs.lzma $(BIN_DIR)/$(UIMAGE)-initramfs.bin
endif
	$(call Image/Build/Initramfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

ifeq ($(CONFIG_USE_ZYXEL_FW_SIGNATURE),y)
define Image/Build/Signature
	$(SCRIPT_DIR)/private/fwsignature/mk_t2kimage.sh rsa $(1) $(SCRIPT_DIR)/private/fwsignature/$(CONFIG_MRD_PRODUCT_NAME).pem 
	@echo "mkdir $(BUILD_DIR_BASE)/fwsignature"
	mkdir -p $(BUILD_DIR_BASE)/fwsignature
	@echo "mv Public key and hash"
	mv $(SCRIPT_DIR)/private/fwsignature/$(CONFIG_MRD_PRODUCT_NAME).pem.modulus $(BUILD_DIR_BASE)/fwsignature/
	mv $(SCRIPT_DIR)/private/fwsignature/$(CONFIG_MRD_PRODUCT_NAME).pem.publickey_hash $(BUILD_DIR_BASE)/fwsignature/
endef
endif

#
# Add Zyxel TRX image Header
#

define Image/Build/zy_trx_image_wo_rootfs_defconfig
	$(STAGING_DIR_HOST)/bin/ramips-trx \
                -T ZYXEL \
                -K $(1) \
                -P $(CHIP_ID) \
                -B $(CONFIG_MRD_PRODUCT_NAME) \
                -I $(CONFIG_MRD_MODEL_ID) \
                -S $(CONFIG_ZYXEL_FIRMWARE_VERSION) \
                -s $(CONFIG_ZYXEL_FIRMWARE_VERSION) \
                $(TRX_IMG_OPT) \
                -f $(1) -o $(BIN_DIR)/ras.bin \
                -c $(STAGING_DIR_HOST)/bin/ramips-trx_config | tee $(BIN_DIR)/ras.bin.trxinfo
	$(INSTALL_DIR) $(BIN_DIR)/zyxel
	$(CP) $(BIN_DIR)/ras.bin $(BIN_DIR)/zyxel
	$(CP) $(BIN_DIR)/ras.bin.trxinfo $(BIN_DIR)/zyxel
	md5sum $(BIN_DIR)/zyxel/ras.bin --binary > $(BIN_DIR)/zyxel/ras.bin.md5sums
	rm -f $(BIN_DIR)/ras.bin
	rm -f $(BIN_DIR)/ras.bin.trxinfo
endef

define Image/Build/zy_trx_image_wo_rootfs_defconfig_encrypted
	$(STAGING_DIR_HOST)/bin/ramips-trx \
                -T ZYXEL \
                -K $(1) \
                -P $(CHIP_ID) \
                -B $(CONFIG_MRD_PRODUCT_NAME) \
                -I $(CONFIG_MRD_MODEL_ID) \
                -S $(CONFIG_ZYXEL_FIRMWARE_VERSION) \
                -s $(CONFIG_ZYXEL_FIRMWARE_VERSION) \
                $(TRX_IMG_OPT) \
				-f $(2) \
                -e $(3) \
				-o $(BIN_DIR)/encras.bin \
                -c $(STAGING_DIR_HOST)/bin/ramips-trx_config | tee $(BIN_DIR)/encras.bin.trxinfo
	$(INSTALL_DIR) $(BIN_DIR)/zyxel
	$(CP) $(BIN_DIR)/encras.bin $(BIN_DIR)/zyxel
	$(CP) $(BIN_DIR)/encras.bin.trxinfo $(BIN_DIR)/zyxel
	md5sum $(BIN_DIR)/zyxel/encras.bin --binary > $(BIN_DIR)/zyxel/encras.bin.md5sums
	rm -f $(BIN_DIR)/encras.bin
	rm -f $(BIN_DIR)/encras.bin.trxinfo
	rm -f $(2)
endef


define Image/Build/zy_trx_image_uboot_defconfig
	$(STAGING_DIR_HOST)/bin/ramips-trx \
                -U BOOT \
                -G $(1) \
                -B $(CONFIG_MRD_PRODUCT_NAME) \
                -I $(CONFIG_MRD_MODEL_ID) \
                -S $(CONFIG_ZYXEL_FIRMWARE_VERSION) \
                -s $(CONFIG_ZYXEL_FIRMWARE_VERSION) \
                $(TRX_IMG_OPT) \
                -f $(1) -o $(BIN_DIR)/zld_trx.bin \
                -c $(STAGING_DIR_HOST)/bin/ramips-trx_config | tee $(BIN_DIR)/boot.bin.trxinfo
	$(INSTALL_DIR) $(BIN_DIR)/zyxel
	$(CP) $(BIN_DIR)/zld_trx.bin $(BIN_DIR)/zyxel/
	$(CP) $(BIN_DIR)/boot.bin.trxinfo $(BIN_DIR)/zyxel/
	rm -f $(BIN_DIR)/zld_trx.bin
	rm -f $(BIN_DIR)/boot.bin.trxinfo
endef

$(eval $(call BuildImage))
