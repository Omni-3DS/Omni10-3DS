# Omni10-3DS Makefile
# Requires: devkitARM + firmtool + Python 3

ifneq ($(strip $(DEVKITARM)),)
include $(DEVKITARM)/3ds_rules
endif

TARGET   := Omni10
BUILD    := build
SOURCES  := source
INCLUDES := include

# Placeholder – will be expanded once the real source tree exists
.PHONY: all firm clean release

all: firm

firm:
	@echo "Building Omni10.firm ..."
	@echo "(Source not yet implemented – this is the project skeleton)"
	@mkdir -p $(BUILD)
	@echo "Placeholder build successful"

release: firm
	@echo "Creating release package..."

clean:
	@rm -rf $(BUILD)
	@echo "Clean done"
