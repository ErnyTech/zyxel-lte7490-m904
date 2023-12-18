include  UNION_EN7565_NPU_demo.profile
include  rule.mak
GLOBAL_INC_DIR=global_inc

ifeq ($(TCSUPPORT_OPENWRT), 1)
HOST_CC=gcc
CFLAGS = -I$(GLOBAL_INC_DIR)/boot
else
CC=gcc
CFLAGS += -I$(GLOBAL_INC_DIR)/boot
endif

OBJS=secure_header.o
EXECS=sheader

ifneq ($(strip $(TCSUPPORT_LITTLE_ENDIAN)),)
CFLAGS += -DTCSUPPORT_LITTLE_ENDIAN
endif

ifeq ($(TCSUPPORT_SECURE_BOOT_FLASH_OTP), 1)
CFLAGS += -DTCSUPPORT_SECURE_BOOT_FLASH_OTP
endif

ifeq ($(TCSUPPORT_SECURE_BOOT_V1), 1)
CFLAGS += -DTCSUPPORT_SECURE_BOOT_V1
endif

ifeq ($(TCSUPPORT_SECURE_BOOT_V2), 1)
CFLAGS += -DTCSUPPORT_SECURE_BOOT_V2
endif

ifeq ($(TCSUPPORT_OPENWRT), 1)
all: $(EXECS)

.c.o:
	$(HOST_CC) -c $(CFLAGS) $<

$(EXECS): $(OBJS)
	$(HOST_CC) -o $@ $(OBJS)
else
all: sheader

.c.o:
	$(CC) -c $(CFLAGS) $<

sheader: $(OBJS) 
	$(CC) -o $@ $(OBJS)
endif

clean:
	rm -f *.o *.a $(EXECS)
