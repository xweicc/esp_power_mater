# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

#set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/managed_components/lvgl__lvgl)
#set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
#include(${LVGL_DIR}/cmake/lv_conf.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(xt60_power_meter)
