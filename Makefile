CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f

A9_DIR  := firm/arm9
A11_DIR := firm/arm11
A9_START := $(A9_DIR)/start.s
A9_SRC   := $(A9_DIR)/main.c
A9_SD    := $(A9_DIR)/sdmmc.c
A9_FAT   := $(A9_DIR)/fat_min.c
A9_LD    := $(A9_DIR)/link.ld
A11_START := $(A11_DIR)/start.s
A11_SRC   := $(A11_DIR)/main_arm11.c
A11_LD    := $(A11_DIR)/link.ld

A9_ELF  := arm9.elf
A9_BIN  := arm9.bin
A11_ELF := arm11.elf
A11_BIN := arm11.bin
TARGET  := Omni10.firm

A9_ENTRY  := 0x08000040
A11_ENTRY := 0x1FF80000

OMNI_VER := $(shell tr -d ' \t\r\n' < version.dat 2>/dev/null || echo 1.0.0)

A9_CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te \
              -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-strlen \
              -fno-builtin-strncmp -ffreestanding -I$(A9_DIR) \
              -DOMNI_VERSION=\"$(OMNI_VER)\"
A11_CFLAGS := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv6k \
              -mtune=mpcore -mfloat-abi-soft -ffreestanding \
              -fno-builtin-memset -fno-builtin-memcpy \
              -DOMNI_VERSION=\"$(OMNI_VER)\"
ASFLAGS9  := -marm -march=armv5te
ASFLAGS11 := -marm -march=armv6k

.PHONY: all clean firm check ensure-main arm9 arm11

all: firm

ensure-main:
	@if [ ! -f scripts/firm_main_0.zlib.b64 ] || [ ! -f scripts/firm_main_3.zlib.b64 ]; then \
	  echo "ERROR: missing scripts/firm_main_*.zlib.b64"; exit 1; \
	fi
	python3 scripts/decode_firm_main.py
	@grep -q 'screen_scripts_hub' $(A9_SRC)
	@grep -q 'HOME SCRIPTS' $(A9_SRC)
	@echo "FIRM version = $(OMNI_VER)"

firm: ensure-main $(TARGET)

arm9: $(A9_BIN)
arm11: $(A11_BIN)

$(A9_BIN): $(A9_START) $(A9_SRC) $(A9_SD) $(A9_FAT) $(A9_LD) version.dat
	@echo "=== ARM9 v$(OMNI_VER) REAL SDMMC ==="
	@$(RM) $(A9_ELF) $(A9_BIN)
	$(CC) $(A9_CFLAGS) $(ASFLAGS9) -T $(A9_LD) -nostdlib -Wl,--nmagic \
		$(A9_START) $(A9_SRC) $(A9_SD) $(A9_FAT) -o $(A9_ELF)
	$(OBJCOPY) -O binary $(A9_ELF) $(A9_BIN)
	@wc -c $(A9_BIN)

$(A11_BIN): $(A11_START) $(A11_SRC) $(A11_LD) version.dat
	@echo "=== ARM11 v$(OMNI_VER) ==="
	@$(RM) $(A11_ELF) $(A11_BIN)
	$(CC) $(A11_CFLAGS) $(ASFLAGS11) -T $(A11_LD) -nostdlib -Wl,--nmagic \
		$(A11_START) $(A11_SRC) -o $(A11_ELF)
	$(OBJCOPY) -O binary $(A11_ELF) $(A11_BIN)
	@wc -c $(A11_BIN)

$(TARGET): $(A9_BIN) $(A11_BIN)
	@echo "=== pack Omni10.firm v$(OMNI_VER) ==="
	@$(RM) $(TARGET)
	firmtool build $(TARGET) -i \
		-n $(A9_ENTRY) -e $(A11_ENTRY) \
		-D $(A9_BIN) $(A11_BIN) \
		-A $(A9_ENTRY) $(A11_ENTRY) \
		-C NDMA XDMA
	@wc -c $(A9_BIN) $(A11_BIN) $(TARGET)
	@echo "[OK] $(TARGET) v$(OMNI_VER)"

check:
	@command -v $(CC) >/dev/null && echo "[OK] gcc" || echo "[!!] gcc"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] firmtool"

clean:
	@$(RM) $(A9_ELF) $(A9_BIN) $(A11_ELF) $(A11_BIN) $(TARGET)
