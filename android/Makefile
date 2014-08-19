CONFIG ?= Makefile.local
-include $(CONFIG)

HOST_OS     ?= $(shell uname | sed -e s/_.*// | tr '[:upper:]' '[:lower:]')
TARGET_OS   ?= $(HOST_OS)
TARGET_ARCH ?= $(shell uname -m | sed -e s/i.86/i386/)

ifneq ($(findstring $(HOST_OS),sunos darwin),)
  TARGET_ARCH ?= $(shell uname -p | sed -e s/i.86/i386/)
endif

DEBUG ?= 1

ANDROID_PROJECT=android-project
ifeq ($(DEBUG),)
ANT_TARGET:=release
ANT_INSTALL_TARGET:=installr
else
ANT_TARGET:=debug
ANT_INSTALL_TARGET:=installd
endif
ifeq ($(Q),)
NDK_VERBOSE:="V=1"
else
NDK_VERBOSE:=
endif

.PHONY: android
android: android-update-project android-copy-assets android-uninstall
	@echo "===> NDK [native build]"
	$(Q)cd $(ANDROID_PROJECT) && ndk-build $(NDK_VERBOSE)
	@echo "===> ANT [java build]"
	$(Q)cd $(ANDROID_PROJECT) && ant $(ANT_TARGET)
	$(Q)cd $(ANDROID_PROJECT) && ant $(ANT_INSTALL_TARGET)

.PHONY: android-clean
android-clean:
	@echo "===> ANDROID [clean project]"
	$(Q)rm -rf $(ANDROID_PROJECT)/assets
	$(Q)rm -rf $(ANDROID_PROJECT)/bin
	$(Q)rm -rf $(ANDROID_PROJECT)/obj
	$(Q)rm -rf $(ANDROID_PROJECT)/libs
	$(Q)rm -rf $(ANDROID_PROJECT)/gen
	$(Q)rm -rf $(ANDROID_PROJECT)/local.properties

android-update-project: $(ANDROID_PROJECT)/local.properties

$(ANDROID_PROJECT)/local.properties:
	@echo "===> ANDROID [update project]"
	$(Q)cd $(ANDROID_PROJECT); SDK=`android  list sdk | grep "SDK Platform Android 2.3.3" | awk -F'-' ' { print $$1 }'`; [ -n "$$SDK" ] && android update sdk -u -s -t $$SDK || echo
	$(Q)cd $(ANDROID_PROJECT) && android update project -p . -t android-15

.PHONY: android-copy-assets
android-copy-assets:
	@echo "===> CP [copy assets]"
	$(Q)mkdir -p $(ANDROID_PROJECT)/assets
	$(Q)cp -r data $(ANDROID_PROJECT)/assets
	$(Q)for i in hdpi ldpi mdpi xhdpi; do mkdir -p $(ANDROID_PROJECT)/res/drawable-$${i}; cp data/icon.png $(ANDROID_PROJECT)/res/drawable-$${i}/icon.png; done

android-setup:
	$(Q)ARCH=x86_64; \
	NDK_VERSION=r9; \
	SDK_VERSION=20130729; \
	[ $(TARGET_ARCH) = "i386" ] && ARCH=x86; \
	echo "Downloading the ndk..."; \
	wget --quiet --continue http://dl.google.com/android/ndk/android-ndk-$$NDK_VERSION-linux-$$ARCH.tar.bz2; \
	echo "Extracting the ndk..."; \
	tar -xjf android-ndk-$$NDK_VERSION-linux-$$ARCH.tar.bz2 -C ~/; \
	echo "Downloading the sdk..."; \
	wget --quiet --continue http://dl.google.com/android/adt/adt-bundle-linux-$$ARCH-$$SDK_VERSION.zip; \
	echo "Extracting the sdk..."; \
	ARCHIVE=`readlink -f adt-bundle-linux-$$ARCH-$$SDK_VERSION.zip`; \
	cd ~; \
	unzip -o -qq $$ARCHIVE; \
	echo "Configure paths..."; \
	echo "export ANDROID_SDK=~/adt-bundle-linux-$$ARCH-$$SDK_VERSION/sdk" >> ~/.bashrc; \
	echo "export ANDROID_NDK=~/android-ndk-$$NDK_VERSION" >> ~/.bashrc; \
	echo "export NDK_ROOT=\$$ANDROID_NDK" >> ~/.bashrc; \
	echo "export PATH=\$$PATH:\$$ANDROID_NDK:\$$ANDROID_SDK/tools:\$$ANDROID_SDK/platform-tools" >> ~/.bashrc;

android-backtrace:
	adb logcat | ndk-stack -sym $(ANDROID_PROJECT)/obj/local/armeabi

android-uninstall:
	#$(Q)cd $(ANDROID_PROJECT) && ant uninstall
