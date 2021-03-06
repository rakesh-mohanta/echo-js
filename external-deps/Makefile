TOP=..

LLVM_CXXFLAGS="`$LLVM_CONFIG --cxxflags` -fno-rtti"
LLVM_LDFLAGS=`$LLVM_CONFIG --ldflags`
LLVM_LIBS=`$LLVM_CONFIG --libs core bitwriter jit x86codegen`
LLVM_LIBS:="$LLVM_LDFLAGS $LLVM_LIBS -lstdc++"
LLVM_CONFIGURE_ARGS=--disable-jit --enable-static --enable-optimized --disable-assertions

PCRE_CONFIGURE_ARGS=--enable-pcre16 --enable-utf --disable-cpp

CFLAGS=-I$(TOP)/runtime

all-local:: build-pcre

clean-local:: clean-pcre

ifneq ($(TRAVIS_BUILD_NUMBER),)
PCRE_TARGETS=osx
else
PCRE_TARGETS=iossim iosdev osx
endif


configure-pcre: $(PCRE_TARGETS:%=configure-pcre-%)

build-pcre: $(PCRE_TARGETS:%=build-pcre-%)

clean-pcre: $(PCRE_TARGETS:%=clean-pcre-%)


configure-pcre-osx:
	@mkdir -p pcre-osx
	(cd pcre-osx && \
	../$(TOP)/pcre/configure $(PCRE_CONFIGURE_ARGS)) || rm -rf pcre-osx

configure-pcre-iossim:
	@mkdir -p pcre-iossim
	(cd pcre-iossim && \
	PATH=$(IOSSIM_ROOT)/usr/bin:$$PATH \
	CC="clang $(IOSSIM_ARCH) $(IOSSIM_ARCH_FLAGS) -miphoneos-version-min=$(MIN_IOS_VERSION) -isysroot $(IOSSIM_SYSROOT)" \
	CXX="clang++ $(IOSSIM_ARCH) $(IOSSIM_ARCH_FLAGS) -miphoneos-version-min=$(MIN_IOS_VERSION) -isysroot $(IOSSIM_SYSROOT)" \
	LD="clang" \
	AS="$(IOSSIM_ROOT)/usr/bin/as" \
	../$(TOP)/pcre/configure --host=$(IOSSIM_TRIPLE) $(PCRE_CONFIGURE_ARGS)) || rm -rf pcre-iossim

configure-pcre-iosdev:
	@mkdir -p pcre-iosdev
	(cd pcre-iosdev && \
	PATH=$(IOSDEV_ROOT)/usr/bin:$$PATH \
	CC="clang $(IOSDEV_ARCH) $(IOSDEV_ARCH_FLAGS) -miphoneos-version-min=$(MIN_IOS_VERSION) -isysroot $(IOSDEV_SYSROOT)" \
	CXX="clang++ $(IOSDEV_ARCH) $(IOSDEV_ARCH_FLAGS) -miphoneos-version-min=$(MIN_IOS_VERSION) -isysroot $(IOSDEV_SYSROOT)" \
	LD="clang" \
	AS="$(IOSDEV_ROOT)/usr/bin/as" \
	../$(TOP)/pcre/configure --host=$(IOSDEV_TRIPLE) $(PCRE_CONFIGURE_ARGS)) || rm -rf pcre-iosdev

build-pcre-osx: configure-pcre-osx
	$(MAKE) -C pcre-osx pcre_chartables.c libpcre16.la

build-pcre-iossim: configure-pcre-iossim
	$(MAKE) -C pcre-iossim pcre_chartables.c libpcre16.la

build-pcre-iosdev: configure-pcre-iosdev
	$(MAKE) -C pcre-iosdev pcre_chartables.c libpcre16.la

clean-pcre-iossim:
	@test -d pcre-iossim && $(MAKE) -C pcre-iossim clean

clean-pcre-iosdev:
	@test -d pcre-iosdev && $(MAKE) -C pcre-iosdev clean

clean-pcre-osx:
	@test -d pcre-osx    && $(MAKE) -C pcre-osx clean

configure-llvm:
	@mkdir -p llvm-osx
	(cd llvm-osx && \
	../$(TOP)/llvm/configure  $(LLVM_CONFIGURE_ARGS)) || rm -rf llvm-osx

build-llvm: configure-llvm
	$(MAKE) -C llvm-osx

install-llvm:

clean-llvm:
	@test -d llvm-osx && $(MAKE) -C llvm-osx clean

include $(TOP)/build/build.mk
