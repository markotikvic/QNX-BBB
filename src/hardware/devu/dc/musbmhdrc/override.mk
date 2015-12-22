TYPE = $(firstword $(filter a o dll, $(VARIANTS)) o)
BOARD = $(firstword $(filter omap4430 omapL1xx omap4460_garmin am335x dm816x, $(VARIANTS)))
USEFILE_dll = $(SECTION_ROOT)/$(BOARD)/$(PROTOCOL)/$(NAME)-$(PROTOCOL)-$(BOARD).use
USEFILE   = $(USEFILE_$(TYPE))
