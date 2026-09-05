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
GOOD_MAIN := https://raw.githubusercontent.com/Omni-3DS/Omni10-3DS/a199a2616931bd534ecb166eccb717af23f3a569/firm/arm9/main.c

# Single source of truth for About + boot splash
OMNI_VER := $(shell tr -d ' \t\r\n' < version.dat 2>/dev/null || echo 0.8.2)

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te \
           -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-strlen \
           -fno-builtin-strncmp -ffreestanding \
           -DOMNI_VERSION=\"$(OMNI_VER)\"
ASFLAGS := -marm -march=armv5te
LDFLAGS := -T $(LINKER) -nostdlib -Wl,--nmagic

.PHONY: all clean firm check ensure-main

all: firm

ensure-main:
	@# Prefer full main if it has int main; else restore known-good ARM9 payload
	@if [ ! -f $(SOURCE) ] || ! grep -q 'int main' $(SOURCE) 2>/dev/null; then \
	  echo "[!] restoring firm/arm9/main.c"; \
	  $(CURL) -o $(SOURCE) $(GOOD_MAIN); \
	fi
	@# ALWAYS rewrite legacy 0.3.10 strings to version.dat (About was stuck on 0.3.10)
	@sed -i \
	  -e "s/0\.3\.10/$(OMNI_VER)/g" \
	  -e "s/V0\.3\.10/V$(OMNI_VER)/g" \
	  -e "s/VERSION 0\.3\.10/VERSION $(OMNI_VER)/g" \
	  -e "s/v0\.3\.10/v$(OMNI_VER)/g" \
	  $(SOURCE)
	@grep -q 'int main' $(SOURCE)
	@echo "[OK] FIRM version strings -> $(OMNI_VER) (from version.dat)"

firm: ensure-main $(TARGET)

$(TARGET): $(START_S) $(SOURCE) $(LINKER) version.dat
	@echo "=== Omni10 FIRM v$(OMNI_VER) ==="
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
