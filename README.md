# ESP功率计
### 编译说明
- 系统环境：Ubuntu 22.04
- 安装esp-idf：https://docs.espressif.com/projects/esp-idf/zh_CN/release-v5.1/esp32c3/get-started/linux-macos-setup.html
- 执行`idf.py build`编译
- 执行`idf.py flash`烧录
### 功能说明
- 显示电压，电流，功率
- 电流历史曲线显示
- 过流，低压，蜂鸣器报警
- 电压电流校准
### 硬件说明
- 使用ESP32C3主控
- 使用INA226电流采样
- DCDC+LDO供电，带反接保护
- XT60接口
- 1.29寸OLED屏幕
- 输入电压：5~30V
- 电流范围：1mA~60A
### 硬件开源地址
- https://oshwhub.com/qwiaoei/xt60-powermeter
