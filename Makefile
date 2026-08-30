# Omni10-3DS Makefile
# Requires: devkitARM + firmtool + Python 3

ifneq ($(strip $(DEVKITARM)),)
include $(DEVKITARM)/3ds_rules
endif

TARGET   := Omni10
BUILD    := build
SOURCES  := source
INCLUDES := include
DATA     := data
ASSETS   := assets

# Source files (will grow)
CFILES   := $(wildcard $(SOURCES)/*.c)

.PHONY: all firm clean release info

all: firm

firm:
	@echo "========================================"
	@echo "  Building Omni10-3DS"
	@echo "========================================"
	@mkdir -p $(BUILD)
	@echo "Sources found:"
	@echo $(CFILES)
	@echo ""
	@echo "Note: Full FIRM linking requires the complete"
	@echo "ARM9/ARM11 toolchain setup (devkitARM + firmtool)."
	@echo "This is currently a development skeleton."
	@echo ""
	@echo "Placeholder build finished."
	@touch $(BUILD)/Omni10.firm
	@echo "Created $(BUILD)/Omni10.firm (placeholder)"

release: firm
	@echo "Creating release package..."
	@mkdir -p release
	@cp $(BUILD)/Omni10.firm release/ 2>/dev/null || true
	@cp -r scripts release/ 2>/dev/null || true
	@echo "Release folder ready."

info:
	@echo "Omni10-3DS Build Info"
	@echo "Target:   $(TARGET)"
	@echo "Sources:  $(SOURCES)"
	@echo "Includes: $(INCLUDES)"
	@echo "C files:  $(CFILES)"

clean:
	@rm -rf $(BUILD) release
	@echo "Clean done."
