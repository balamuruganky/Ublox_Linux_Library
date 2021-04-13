# Application make file to enable STL 
APP_BUILD_SCRIPT := $(call my-dir)/Android.mk
APP_PROJECT_PATH := $(call my-dir)

# Disable run time type info
#APP_CPPFLAGS += -fno-rtti
APP_CPPFLAGS += -O0
APP_OPTIM := release
NDK_TOOLCHAIN_VERSION=4.9

# Use GNU our STL
APP_STL := gnustl_shared
APP_ABI := armeabi-v7a
APP_PLATFORM := android-123
