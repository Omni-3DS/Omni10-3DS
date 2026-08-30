# Omni10-3DS Makefile
# Original project – not a GodMode9 fork
#
# Policy: NEVER write a fake Omni10.firm (causes "invalid/corrupted firm" on device).
# Real .firm only with devkitARM + firmtool + real ARM binaries.

HAVE_DEVKIT := 0
ifneq ($(strip $(DEVKITARM)),)
  ifneq ($(wildcard $(DEVKITARM)/3ds_rules),)
    include $(DEVKITARM)/3ds_rules
    HAVE_DEVKIT := 1
  endif
endif

FIRMTOOL ?= $(shell command -v firmtool 2>/dev/null)

TARGET   := Omni10
BUILD    := build
SOURCES  := source
INCLUDES := include
DATA     := data
ASSETS   := assets
LINKER   := linker

COMMON_C := $(wildcard $(SOURCES)/*.c)
ARM9_C   := $(wildcard $(SOURCES)/arm9/*.c)
ARM11_C  := $(wildcard $(SOURCES)/arm11/*.c)

.PHONY: all firm clean release info check help

all: help check

help:
	@echo "========================================"
	@echo "  Omni10-3DS"
	@echo "========================================"
	@echo "  make check    toolchain status"
	@echo "  make info     list sources"
	@echo "  make firm     real FIRM only (needs toolchain)"
	@echo "  make release  source package (no fake firm)"
	@echo "  make clean"
	@echo ""
	@echo "No placeholder .firm is created. Booting a fake file"
	@echo "causes: invalid firm or corrupted firm"
	@echo "========================================"

check:
	@echo "DEVKITARM=$(DEVKITARM)"
	@echo "HAVE_DEVKIT=$(HAVE_DEVKIT)"
	@echo "FIRMTOOL=$(FIRMTOOL)"
	@if [ "$(HAVE_DEVKIT)" = "1" ]; then echo "[OK] devkitARM"; else echo "[!!] devkitARM missing"; fi
	@if [ -n "$(FIRMTOOL)" ]; then echo "[OK] firmtool"; else echo "[!!] firmtool missing (pip install firmtool)"; fi

info:
	@echo "Common: $(COMMON_C)"
	@echo "ARM9:   $(ARM9_C)"
	@echo "ARM11:  $(ARM11_C)"

# Real FIRM only – refuse fake output
firm:
ifeq ($(HAVE_DEVKIT),1)
  ifneq ($(strip $(FIRMTOOL)),)
	@mkdir -p $(BUILD)/arm9 $(BUILD)/arm11
	@echo "Toolchain present."
	@echo "Full compile/link of ARM9+ARM11 is not finished yet."
	@echo "When arm9.bin and arm11.bin exist, pack with firmtool."
	@echo "Refusing incomplete/fake FIRM so the console never sees corrupt data."
	@exit 1
  else
	@echo "ERROR: firmtool not found.  pip install firmtool"
	@exit 1
  endif
else
	@echo "ERROR: No valid Omni10.firm without devkitARM + firmtool."
	@echo "Install: https://devkitpro.org/  then pip install firmtool"
	@echo "A text/placeholder file is NOT a FIRM and will show:"
	@echo "  invalid firm or corrupted firm"
	@exit 1
endif

release:
	@mkdir -p release/Omni10-3DS
	@cp -r $(SOURCES) $(INCLUDES) $(LINKER) $(DATA) $(ASSETS) scripts languages docs release/Omni10-3DS/ 2>/dev/null || true
	@cp Makefile README.md LICENSE requirements.txt release/Omni10-3DS/ 2>/dev/null || true
	@rm -f release/Omni10-3DS/**/*.firm 2>/dev/null || true
	@echo "Source package: release/Omni10-3DS/ (no bootable .firm)"

clean:
	@rm -rf $(BUILD) release
	@echo "Clean done."
