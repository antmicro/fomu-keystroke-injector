GIT_VERSION := $(shell git describe --tags)

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

TINYUSB_OBJ = $(patsubst %.c,%.o,$(call rwildcard,$(FIRMWARE_DIRECTORY)/third_party/tinyusb/src,*.c))
OBJECTS = main.o mode.o rgb.o touch.o flash.o \
	script.o storage.o script_parse.o \
	usb_descriptors.o usb_callbacks.o \
	usb_acm.o usb_hid.o usb_dfu.o $(TINYUSB_OBJ)

include ../include/generated/variables.mak
include $(SOC_DIRECTORY)/software/common.mak

CFLAGS += -Wall -Werror -I$(FIRMWARE_DIRECTORY)/include -I$(FIRMWARE_DIRECTORY)/third_party/tinyusb/src
INCLUDES += -I$(BUILDINC_DIRECTORY)/generated

NAME = firmware

all: $(NAME).fbi

vpath %.a $(PACKAGES:%=../%)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

%.fbi: %.bin
	$(PYTHON) -m litex.soc.software.mkmscimg $< -f --little -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary -S -R .bss $< $@
	chmod -x $@

$(NAME).elf: $(FIRMWARE_DIRECTORY)/linker.ld $(OBJECTS)

%.elf: crt0.o $(OBJECTS)
	$(CC) $(LDFLAGS) \
		-T $(FIRMWARE_DIRECTORY)/linker.ld \
		-N -o $@ \
		crt0.o \
		$(OBJECTS) \
		$(PACKAGES:%=-L../%) \
		-Wl,--whole-archive \
		-Wl,--gc-sections \
		$(LIBS:lib%=-l%)
	chmod -x $@
	$(PYTHON) -m litex.soc.software.memusage $@ ../include/generated/regions.ld $(TRIPLE)

%.o: $(FIRMWARE_DIRECTORY)/src/%.c
	$(compile) -include $(FIRMWARE_DIRECTORY)/include/fomu.h

%.o: %.c
	$(compile) -include $(FIRMWARE_DIRECTORY)/include/fomu.h

%.o: $(FIRMWARE_DIRECTORY)/src/%.S
	$(assemble)

crt0.o: $(CPU_DIRECTORY)/crt0.S
	$(assemble)

clean:
	$(RM) $(OBJECTS) $(OBJECTS:.o=.d) $(NAME).elf $(NAME).fbi .*~ *~

.PHONY: all clean load
