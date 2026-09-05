CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f
CURL    := curl -fsSL

FIRM_DIR := firm/arm9
START_S  := $(FIRM_DIR)/start.s
SOURCE   := $(FIRM_DIR)/main.c
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm
ENTRY    := 0x08000040

OMNI_VER := $(shell tr -d ' \t\r\n' < version.dat 2>/dev/null || echo 0.8.4)

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te \
           -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-strlen \
           -fno-builtin-strncmp -ffreestanding \
           -DOMNI_VERSION=\"$(OMNI_VER)\"
ASFLAGS := -marm -march=armv5te
LDFLAGS := -T $(LINKER) -nostdlib -Wl,--nmagic

.PHONY: all clean firm check ensure-main

all: firm

# NEVER use old 0.3.10 fallback when compressed full source exists
ensure-main:
	@if [ ! -f scripts/firm_main_0.zlib.b64 ] || [ ! -f scripts/firm_main_3.zlib.b64 ]; then \
	  echo "ERROR: missing scripts/firm_main_*.zlib.b64 full source"; exit 1; \
	fi
	python3 scripts/decode_firm_main.py
	@grep -q 'screen_scripts_hub' $(SOURCE)
	@grep -q 'FULL FEATURES' $(SOURCE)
	@echo "FIRM version = $(OMNI_VER) (FULL FEATURES)"

firm: ensure-main $(TARGET)

$(TARGET): $(START_S) $(SOURCE) $(LINKER) version.dat
	@echo "=== Omni10 FIRM v$(OMNI_VER) FULL FEATURES ==="
	@$(RM) $(ELF) $(BIN) $(TARGET)
	$(CC) $(CFLAGS) $(ASFLAGS) $(LDFLAGS) $(START_S) $(SOURCE) -o $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	firmtool build $(TARGET) -i -n $(ENTRY) -e 0 -D $(BIN) -A $(ENTRY) -C NDMA
	@wc -c $(BIN) $(TARGET)
	@echo "[OK] $(TARGET) v$(OMNI_VER)"

check:
	@command -v $(CC) >/dev/null && echo "[OK] gcc" || echo "[!!] gcc"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] firmtool"

clean:
	@$(RM) $(ELF) $(BIN) $(TARGET)
