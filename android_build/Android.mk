LOCAL_PATH := $(call my-dir)

################################################################
############# Build ublox shared library #######################
################################################################
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ../inc \
					../SparkFun_u-blox_GNSS_Arduino_Library/src
LOCAL_SRC_FILES  := ../SparkFun_u-blox_GNSS_Arduino_Library/src/SparkFun_u-blox_GNSS_Arduino_Library.cpp \
					../src/Print.cpp \
					../src/Utils.cpp
LOCAL_MODULE     :=  ublox
LOCAL_CFLAGS := -std=gnu++11 -DANDROID
LOCAL_CFLAGS += -fPIE
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true

include $(BUILD_SHARED_LIBRARY)

#################################################################
################ ublox_f9p_test #################################
#################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ublox_f9p_test
LOCAL_C_INCLUDES := ../inc \
					../SparkFun_u-blox_GNSS_Arduino_Library/src
LOCAL_SRC_FILES:= ../linux_examples/ublox_f9p_test.cpp
LOCAL_LDFLAGS := -L./libs/$(TARGET_ARCH_ABI) -fPIE -pie
LOCAL_SHARED_LIBRARIES := ublox
LOCAL_CXXFLAGS += -fPIE
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
include $(BUILD_EXECUTABLE)

#################################################################
################ ublox_f9p i2c test #############################
#################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := ublox_f9p_i2c_test
LOCAL_C_INCLUDES := ../inc \
					../SparkFun_u-blox_GNSS_Arduino_Library/src
LOCAL_SRC_FILES:= ../linux_examples/ublox_f9p_i2c_test.cpp
LOCAL_LDFLAGS := -L./libs/$(TARGET_ARCH_ABI) -fPIE -pie
LOCAL_SHARED_LIBRARIES := ublox
LOCAL_CXXFLAGS += -fPIE
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
include $(BUILD_EXECUTABLE)