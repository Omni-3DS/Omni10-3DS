# Omni10-3DS – real FIRM build (same pattern as SlabyLol/3DSFirm)
# Needs: arm-none-eabi-gcc (devkitARM) + firmtool on PATH

CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f

FIRM_DIR := firm/arm9
SOURCE   := $(FIRM_DIR)/main.c
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te -T $(LINKER)

.PHONY: all clean firm check

all: firm

firm: $(TARGET)

$(TARGET): $(SOURCE) $(LINKER)
	@echo "1/4 clean"
	@$(RM) $(ELF) $(BIN) $(TARGET)
	@echo "2/4 compile ARM9"
	$(CC) $(CFLAGS) $(SOURCE) -o $(ELF)
	@echo "3/4 objcopy binary"
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	@echo "4/4 firmtool pack"
	firmtool build $(TARGET) -D $(BIN) -n 0x08000000 -A 0x08000000 -C NDMA
	@echo "[OK] $(TARGET) built"

check:
	@command -v $(CC) >/dev/null && echo "[OK] $(CC)" || echo "[!!] $(CC) missing"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] firmtool missing"

clean:
	@$(RM) $(ELF) $(BIN) $(TARGET)
	@echo "Clean done."
