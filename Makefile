# Omni10-3DS Makefile
# Original project – not a GodMode9 fork
# Real builds need: devkitARM + firmtool + Python 3

ifneq ($(strip $(DEVKITARM)),)
  ifneq ($(wildcard $(DEVKITARM)/3ds_rules),)
    include $(DEVKITARM)/3ds_rules
  endif
endif

TARGET   := Omni10
BUILD    := build
SOURCES  := source
INCLUDES := include
DATA     := data
ASSETS   := assets

CFILES   := $(wildcard $(SOURCES)/*.c) \
            $(wildcard $(SOURCES)/arm9/*.c) \
            $(wildcard $(SOURCES)/arm11/*.c)

.PHONY: all firm clean release info

all: firm

firm:
	@echo "========================================"
	@echo "  Omni10-3DS (from scratch, no GM9)"
	@echo "========================================"
	@mkdir -p $(BUILD)
	@echo "Sources:"
	@echo $(CFILES)
	@echo ""
	@if [ -n "$(DEVKITARM)" ] && [ -f "$(DEVKITARM)/3ds_rules" ]; then \
		echo "devkitARM OK – real compile path not fully wired yet"; \
	else \
		echo "No full toolchain in this environment."; \
		echo "Writing non-empty PLACEHOLDER firm (not bootable)."; \
	fi
	@printf 'Omni10-3DS PLACEHOLDER FIRM - not bootable - build from source with devkitARM\n' > $(BUILD)/Omni10.firm
	@echo "Created $(BUILD)/Omni10.firm (placeholder, not 0 bytes)"

release: firm
	@mkdir -p release
	@cp $(BUILD)/Omni10.firm release/ 2>/dev/null || true
	@cp -r scripts assets data languages release/ 2>/dev/null || true
	@echo "Release folder ready."

info:
	@echo "Omni10-3DS Build Info (original codebase)"
	@echo "Target:   $(TARGET)"
	@echo "Sources:  $(SOURCES)"
	@echo "C files:  $(CFILES)"
	@echo "DEVKITARM=$(DEVKITARM)"

clean:
	@rm -rf $(BUILD) release
	@echo "Clean done."
