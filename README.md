# MCU_mylibs

#### 介绍
一个MCU C++跨平台驱动库，有一些片上资源和外部设备的驱动
项目即将改用陈述性名称, 跨平台嵌入式集成库CEILib(Cross-platform Embedded Integration Library)

### 支持列表
目前支持: STM32F103C8T6, STM32F407ZET6, ESP8266
计划支持: W80x, CH57/8x(蓝牙), CH54/5x(USB), CH32V103, STC8G1K08A(8脚), STM8S003F3P6, Arduino, ESP32, 树莓派


#### 软件架构
软件架构说明
大部分使用C++编写，STM32版调用了HAL库，使用了FreeRTOS的CMSIS OS2接口
ESP8266使用了ESP-IDF框架

#### 硬件驱动库
##### 用户输出设备
- SSD1306 128x64液晶
- 数码管
##### 用户输入设备
- 按钮
##### 接口/通信
- I2C
- GPIO
##### 传感器
- 片上ADC
- MCU内置温度传感器
- DHT11
- HTU2x
- AHT2x
- BMP280
- C8S二氧化碳传感器
- 超声波模块
##### 执行器
- 28BYJ-48步进电机
- 直流电机
- 舵机
##### 控制器/数据处理
- 抽象数据Buffer
##### 其他
- RTC时钟
##### 计划
- 电源(电池)管理
- 步进电机PWM细分
- PID控制
- 加热器温度控制
- 4Pin风扇
- W25Qxx SPI Flash
- SPI/SDIO SD卡
- LED点阵
- 4x4键盘
- BMP280
- MPU6050六轴/MPU9250九轴
- QMI8610六轴/QMC7983磁力计
- 38kHz红外
- SX1276/1278 LoRa
- 声波通信, FSO光通信, OLED传输数据
- XGZP68xx{A,D}气压传感器
- ST7735彩屏
- OV7670摄像头

#### 软件库
##### 已实现
- printf_xxx库
- 3x5, 5x7 ASCII字体
##### 计划
- 操作系统: FreeRTOS, RT-Thread, eCos
- 文件系统: FatFs
- GUI界面: LGVL
- TCP/IP协议: LwIP
- 免仿真器调试: gdbserver
- 脚本语言解释器: MicroPython, PikaScript
- 音频/语音编解码: libMAD, CODEC2, Speex
- 图片编解码: LibJPEG
- 神经网络: 未定


#### 使用说明
###### 准备
1.  克隆此仓库到本地，不要放在某个项目目录内，以后用软连接引用

###### STM32配置方法
1.  打开STM32CubeIDE
2.  创建项目，适当配置外设，生成代码
3.  右键项目，点击Convert to C++
4.  创建所需的目录(common, interface)软链接到项目目录 例子:
    ```shell
        resp_dir='~/elec/mcu_mylibs'                   #请替换本仓库克隆的目录
        cd ~/STM32CubeIDE/workspace_1.6.1/MyF1Board    #请替换成你的项目目录
        mkdir mcu_mylibs
        cd mcu_mylibs
        ln -s $resp_dir$/common
        ln -s $resp_dir$/stm32
        ln -s $resp_dir$/stm32f1xx                     #请替换成芯片型号对应的库
    ```
5.  右键项目 -> Properties -> C/C++ General -> Paths and Symbols -> Includes -> 添加所需的头文件目录
    mcu_mylibs/commom/Inc
    mcu_mylibs/stm32/Inc
    mcu_mylibs/stm32f1/Inc (f4项目添加对应目录)
6.  编辑配置文件 mcu_mylibs/common/Inc/mylibs_config.hpp


#### 命名规则
| 类名  | 全称      |  说明           |
| :---- | :----     | :-----------    |
| C_xxx | chip      | 片上设备        |
| E_xxx | extern    | 片外设备        |
| S_xxx | software  | 软件模拟的设备   |
| I_xxx | interface | 跨平台接口，不可直接使用 |
| 无前缀 |           | 该名称已经足够描述      |

#### 目录规范(暂未执行)
```
MCU_mylibs
├──example
│   ├──样例代码
│   └──...
├──interface(跨平台接口)
│   ├──GPIO
│   ├──ADC
│   └──...
├──mcus(MCU片上设备驱动)
│   ├──stm32f1
│   ├──ESP8266
│   └──...
├──exts(外设驱动)
│   ├──StepMotor
│   ├──DHT11
│   └──...
├──dsp(数据处理库)
│   ├──fft
│   ├──pid
│   └──...
├──others(其他)
│   ├──fonts 字体
│   ├──images 样例图片
│   └──...
└──docs(文档)  #TODO: README.md过长, 需要拆分放入这里
    ├──about 关于项目
    ├──tutorial 教程
    ├──three-parts 第三方库
    └──...
```

#### 非MCU代码
PC端工具，大多由Python编写，在ubuntu下测试过:  
    RTC时间同步，点阵/数码管字体生成，二值图动画生成
Android端工具（计划）  
自动配置工具（计划）  

#### 目的
创建一个跨平台的, 综合性的, 容易使用的嵌入式库, 目标是取代Arduino,
尽可能的使用自由软件, 允许适当使用免费的非自由开源软件, 不使用收费的专有软件.

#### 版权说明
本软件库遵循LGPLv3或更新的协议, 见`LICENSE`文件,
包含的第三方库按照第三方库的许可证授权,
本软件库不含商业的第三方库, 如有侵权, 请及时通知删除.


#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
