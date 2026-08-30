# Omni10-3DS Makefile
# Requires (for real builds): devkitARM + firmtool + Python 3

# Only include 3ds_rules when the file actually exists
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
	@if [ -n "$(DEVKITARM)" ] && [ -f "$(DEVKITARM)/3ds_rules" ]; then \
		echo "devkitARM detected – real FIRM build would run here"; \
	else \
		echo "Note: Full FIRM linking requires devkitARM + firmtool."; \
		echo "This is currently a development skeleton (placeholder)."; \
	fi
	@echo ""
	@echo "Placeholder build finished."
	@touch $(BUILD)/Omni10.firm
	@echo "Created $(BUILD)/Omni10.firm (placeholder)"

release: firm
	@echo "Creating release package..."
	@mkdir -p release
	@cp $(BUILD)/Omni10.firm release/ 2>/dev/null || true
	@cp -r scripts release/ 2>/dev/null || true
	@cp -r assets release/ 2>/dev/null || true
	@cp -r data release/ 2>/dev/null || true
	@echo "Release folder ready."

info:
	@echo "Omni10-3DS Build Info"
	@echo "Target:   $(TARGET)"
	@echo "Sources:  $(SOURCES)"
	@echo "Includes: $(INCLUDES)"
	@echo "C files:  $(CFILES)"
	@echo "DEVKITARM=$(DEVKITARM)"

clean:
	@rm -rf $(BUILD) release
	@echo "Clean done."
