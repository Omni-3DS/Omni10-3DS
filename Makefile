CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
RM      := rm -f

FIRM_DIR := firm/arm9
START_S  := $(FIRM_DIR)/start.s
SOURCE   := $(FIRM_DIR)/main.c
CHUNKS   := $(FIRM_DIR)/c0 $(FIRM_DIR)/c1 $(FIRM_DIR)/c2 $(FIRM_DIR)/c3 $(FIRM_DIR)/c4 $(FIRM_DIR)/c5 $(FIRM_DIR)/c6
LINKER   := $(FIRM_DIR)/link.ld
ELF      := arm9.elf
BIN      := arm9.bin
TARGET   := Omni10.firm
ENTRY    := 0x08000040

CFLAGS  := -Wall -O2 -marm -fomit-frame-pointer -nostdlib -march=armv5te \
           -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-strlen \
           -fno-builtin-strncmp -ffreestanding
ASFLAGS := -marm -march=armv5te
LDFLAGS := -T $(LINKER) -nostdlib -Wl,--nmagic

.PHONY: all clean firm check

all: firm

$(SOURCE): $(CHUNKS)
	@cat $(CHUNKS) > $(SOURCE)
	@grep -q 'int main' $(SOURCE)
	@grep -q 'slr_compile' $(SOURCE)

firm: $(TARGET)

$(TARGET): $(START_S) $(SOURCE) $(LINKER)
	@echo "=== Omni10 FIRM v0.5.0 ==="
	@$(RM) $(ELF) $(BIN) $(TARGET)
	$(CC) $(CFLAGS) $(ASFLAGS) $(LDFLAGS) $(START_S) $(SOURCE) -o $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	firmtool build $(TARGET) -i -n $(ENTRY) -e 0 -D $(BIN) -A $(ENTRY) -C NDMA
	@wc -c $(BIN) $(TARGET)
	@echo "[OK] $(TARGET)"

check:
	@command -v $(CC) >/dev/null && echo "[OK] gcc" || echo "[!!] gcc"
	@command -v firmtool >/dev/null && echo "[OK] firmtool" || echo "[!!] firmtool"

clean:
	@$(RM) $(ELF) $(BIN) $(TARGET)
