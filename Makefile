# Forgive me for I know not what I do.

# by default output will take the name of the folder we're in
PROJECTNAME := $(notdir $(CURDIR))
TARGETDIR := ./build/
SOURCEDIR := ./src/
ASSETSDIR := ./assets/
ifdef TARGET_GG
	TARGETEXT := gg
	SMSLIB := SMSlib_GG.lib
	DEFINES := -DTARGET_GG
else
	TARGETEXT := sms
	SMSLIB := SMSlib.lib
	DEFINES := 
endif
SOURCEEXT := c
HEADEREXT := h
# the default entrypoint (where main function is defined), which must come first in the linker
ENTRYPOINT := main
SMSLIB_DIR := /usr/local/share/sdcc/lib/sms
SMSINC_DIR := /usr/local/share/sdcc/include/sms

TARGET := $(TARGETDIR)$(PROJECTNAME).$(TARGETEXT)
SOURCES := $(wildcard $(SOURCEDIR)*.$(SOURCEEXT))
OBJECTS := $(patsubst $(SOURCEDIR)%.$(SOURCEEXT),$(TARGETDIR)%.rel,$(SOURCES))
HEADERS := $(SOURCEDIR)*.$(HEADEREXT)

# asset bundling
ASSETS := $(wildcard $(ASSETSDIR)*)
ASSETSPSGTARGET := $(patsubst $(ASSETSDIR)%.psg,$(TARGETDIR)$(ASSETSDIR)%.psg,$(filter %.psg, $(ASSETS)))
ASSETSVGMTARGET := $(patsubst $(ASSETSDIR)%.vgm,$(TARGETDIR)$(ASSETSDIR)%.psg,$(filter %.vgm, $(ASSETS)))
# patterns included in $(BUNDLEDASSETS) will be bundled; others will be ignored
BUNDLEDASSETS = $(ASSETSPSGTARGET) $(ASSETSVGMTARGET)
# the file src/assets.genererated.h will be automatically generated to index all bundled assets in code
ASSETSHEADER := $(SOURCEDIR)assets.generated.$(HEADEREXT)

# customize this list to explicitly specify the order of linking
MAINS := $(TARGETDIR)$(ENTRYPOINT).rel

# main build target
build: $(TARGETDIR) $(TARGET)

# create the build folder if it doesn't exist
$(TARGETDIR):
	mkdir -p $(TARGETDIR)

# link stage - generally runs once to create a single output
$(TARGETDIR)%.ihx: $(OBJECTS)
	sdcc -L$(SMSLIB_DIR) -o$@ -mz80 --no-std-crt0 --data-loc 0xC000 $(SMSLIB_DIR)/crt0_sms.rel $(MAINS) $(filter-out $(MAINS),$(OBJECTS)) $(SMSLIB) $(SMSLIB_DIR)/PSGlib.rel

# compile stage - generally runs once per .c file found in the source folder
$(TARGETDIR)%.rel: $(SOURCEDIR)%.$(SOURCEEXT) $(ASSETSHEADER)
	sdcc -I$(SMSINC_DIR) -c -mz80 -o$(TARGETDIR) --peep-file $(SMSLIB_DIR)/peep-rules.txt $(DEFINES) $<

# packing stage - generally runs once to create a single output
$(TARGET): $(TARGETDIR)$(ENTRYPOINT).ihx
	makesms $< $(TARGET)

# automatically bundle selected assets using assets2banks
$(ASSETSHEADER): $(TARGETDIR)$(ASSETSDIR) $(BUNDLEDASSETS)
	cd $(TARGETDIR) && assets2banks $(ASSETSDIR) --allowsplitting --compile --singleheader=../$(ASSETSHEADER) || touch ../$(ASSETSHEADER)

# include any existing .psg file in assets folder in bundled assets
$(TARGETDIR)$(ASSETSDIR)%.psg: $(ASSETSDIR)%.psg
	cp $< $@

# convert a .vgm file in assets folder to a .psg file in bundled assets
$(TARGETDIR)$(ASSETSDIR)%.psg: $(ASSETSDIR)%.vgm
	cp $< $@

# create bundled assets folder under build as staging area
$(TARGETDIR)$(ASSETSDIR):
	mkdir -p $(TARGETDIR)$(ASSETSDIR)

# make clean to remove the build folder and all generated files
.PHONY: clean
clean:
	rm -rf $(TARGETDIR)
	-rm src/*.generated.*
