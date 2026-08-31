# Omni10-3DS – real FIRM (3DSFirm pattern + Luma argv FBs)

CC      := arm-none-eabi-gcc
AS      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f

FIRM_DIR := firm/arm9
START_S  := $(FIRM_DIR)/start.s
SOURCE   := $(FIRM_DIR)/main.c
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te -fno-builtin-memset
ASFLAGS := -marm -march=armv5te
LDFLAGS := -T $(LINKER) -nostdlib

.PHONY: all clean firm check

all: firm

firm: $(TARGET)

$(TARGET): $(START_S) $(SOURCE) $(LINKER)
	@echo "1/5 clean"
	@$(RM) $(ELF) $(BIN) $(TARGET)
	@echo "2/5 compile ARM9 (start.s + main.c)"
	$(CC) $(CFLAGS) $(ASFLAGS) $(LDFLAGS) $(START_S) $(SOURCE) -o $(ELF)
	@echo "3/5 objcopy"
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	@echo "4/5 firmtool"
	firmtool build $(TARGET) -D $(BIN) -n 0x08000000 -A 0x08000000 -C NDMA
	@echo "5/5 Luma screen-init flag (reserved2[0] bit0 at FIRM+0x10)"
	@printf '\x01' | dd of=$(TARGET) bs=1 seek=16 conv=notrunc 2>/dev/null
	@ls -la $(BIN) $(TARGET)
	@echo "[OK] $(TARGET)"

check:
	@command -v $(CC) >/dev/null && echo "[OK] $(CC)" || echo "[!!] missing gcc"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] missing firmtool"

clean:
	@$(RM) $(ELF) $(BIN) $(TARGET)
