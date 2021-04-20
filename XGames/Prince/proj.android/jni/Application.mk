APP_STL := gnustl_static
NDK_TOOLCHAIN_VERSION=clang

APP_CPPFLAGS := -frtti -std=c++11 -fsigned-char
APP_LDFLAGS := -latomic
APP_ABI := armeabi armeabi-v7a
APP_SHORT_COMMANDS := true

ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
