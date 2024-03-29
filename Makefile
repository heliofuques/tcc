IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")

CROSS ?= ARM_CORTEX_A8

run: platform/platform.$(IMPERAS_ARCH).exe application/application.$(CROSS).elf 
	platform/platform.$(IMPERAS_ARCH).exe \
	--program RScpu=application/secureApplication.${CROSS}.elf \
	--program cpuRNS1=application/nonSecureApplication1.${CROSS}.elf \
	--program cpuRNS2=application/nonSecureApplication2.${CROSS}.elf \
    | tee imperas.log

# If iGen definition available generate platform C source
ifneq ($(wildcard platform/platform.tcl),)
platform/platform.c : platform/platform.tcl
	$(MAKE) -C platform    NOVLNV=1
endif

platform/platform.$(IMPERAS_ARCH).exe : platform/platform.c
	$(MAKE) -C platform    NOVLNV=1

application/application.$(CROSS).elf: application/secureApplication.c
	$(MAKE) -C application


clean:
	rm -rf *.log 
	$(MAKE) -C platform    NOVLNV=1 clean
	$(MAKE) -C application          clean

realclean: clean
	$(MAKE) -C platform    NOVLNV=1 realclean
