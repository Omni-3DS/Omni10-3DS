# Omni10-3DS – bootable FIRM for Luma chainload
# Uses firmtool -i (suggest-screen-init) so Luma passes framebuffers

CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f

FIRM_DIR := firm/arm9
START_S  := $(FIRM_DIR)/start.s
SOURCE   := $(FIRM_DIR)/main.c
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te -fno-builtin-memset -fno-builtin-memcpy
ASFLAGS := -marm -march=armv5te
LDFLAGS := -T $(LINKER) -nostdlib -Wl,--nmagic

.PHONY: all clean firm check

all: firm

firm: $(TARGET)

$(TARGET): $(START_S) $(SOURCE) $(LINKER)
	@echo "=== Omni10 FIRM build ==="
	@$(RM) $(ELF) $(BIN) $(TARGET)
	@echo "[1] compile"
	$(CC) $(CFLAGS) $(ASFLAGS) $(LDFLAGS) $(START_S) $(SOURCE) -o $(ELF)
	@echo "[2] binary"
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	@echo "[3] firmtool (-i = Luma screen init + argv framebuffers)"
	firmtool build $(TARGET) -i -D $(BIN) -n 0x08000000 -A 0x08000000 -C NDMA
	@echo "[4] sizes:"
	@wc -c $(BIN) $(TARGET)
	@echo "[OK] $(TARGET)"

check:
	@command -v $(CC) >/dev/null && echo "[OK] $(CC)" || echo "[!!] missing gcc"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] missing firmtool"

clean:
	@$(RM) $(ELF) $(BIN) $(TARGET)
