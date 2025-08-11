set(SRC_FILES "main.c" "buzzer.c" "ina226.c" "key.c" "timer.c")

if(CONFIG_ESC)
	list(APPEND SRC_FILES "esc.c")
endif()

if(CONFIG_LCD)
	add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
	list(APPEND SRC_FILES "lcd.c" "lcd_view.c")
	list(APPEND SRC_FILES "lv_font_cn_16.c")
	list(APPEND SRC_FILES "lv_font_cn_24.c")
	list(APPEND SRC_FILES "lv_font_cn_32.c")
	list(APPEND SRC_FILES "lv_img_32.c")
else()
	list(APPEND SRC_FILES  "oled.c" "oled_view.c" "font.c" "ch1115.c" "ssd1312.c")
endif()

idf_component_register(
   SRCS ${SRC_FILES}
   INCLUDE_DIRS "."
)

