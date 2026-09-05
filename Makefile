CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f
CURL    := curl -fsSL

FIRM_DIR := firm/arm9
START_S  := $(FIRM_DIR)/start.s
SOURCE   := $(FIRM_DIR)/main.c
HOME_A   := $(FIRM_DIR)/home_a.c
HOME_B   := $(FIRM_DIR)/home_b.c
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm
ENTRY    := 0x08000040
GOOD_MAIN := https://raw.githubusercontent.com/Omni-3DS/Omni10-3DS/a199a2616931bd534ecb166eccb717af23f3a569/firm/arm9/main.c

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
	@if [ -f $(HOME_A) ] && [ -f $(HOME_B) ] && grep -q 'int main' $(HOME_B) 2>/dev/null; then \
	  cat $(HOME_A) $(HOME_B) > $(SOURCE); \
	  echo "[OK] main.c from home_a+home_b (v$(OMNI_VER))"; \
	elif [ -f $(SOURCE) ] && grep -q 'int main' $(SOURCE) 2>/dev/null; then \
	  echo "[OK] main.c present"; \
	else \
	  echo "[!] restoring base main.c"; \
	  $(CURL) -o $(SOURCE) $(GOOD_MAIN); \
	  sed -i "s/0\.3\.10/$(OMNI_VER)/g;s/V0\.3\.10/V$(OMNI_VER)/g" $(SOURCE) || true; \
	fi
	@grep -q 'int main' $(SOURCE)
	@echo "FIRM version.dat = $(OMNI_VER)"

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
