LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := intl

APP_SUBDIRS := src

# Add more subdirs here, like src/subdir1 src/subdir2

LOCAL_CFLAGS := -Os -I$(LOCAL_PATH)/src -I$(LOCAL_PATH)/include -DBUILDING_LIBINTL -DBUILDING_DLL \
                -DIN_LIBINTL -DENABLE_RELOCATABLE=1 -DIN_LIBRARY \
                -Dset_relocation_prefix=libintl_set_relocation_prefix \
                -Drelocate=libintl_relocate -DDEPENDS_ON_LIBICONV=1 \
                -DNO_XMALLOC -DHAVE_CONFIG_H \
                -DLOCALEDIR=\"\" \
                -DLIBDIR=\"\" \
                -DLOCALE_ALIAS_PATH=\"\" \
                -DINSTALLDIR=\"\"

# -DLOCALE_ALIAS_PATH=\"/usr/local/share/locale\" \
#   -DINSTALLDIR=\"/usr/local/lib\"

LOCAL_SRC_FILES := $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))

include $(BUILD_SHARED_LIBRARY)
