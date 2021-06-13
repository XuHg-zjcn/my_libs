# MCU_mylibs

#### 介绍
一个MCU C++跨平台驱动库，有一些片上资源和外部设备的驱动

### 支持列表
目前支持: STM32F103C8T6, STM32F407ZET6,  
计划支持: ESP8266, ESP32, CC2640, CC2652P, STM8S103F3P6, Arduino, 树莓派


#### 软件架构
软件架构说明
大部分使用C++编写，STM32版调用了HAL库，使用了FreeRTOS的CMSIS OS2接口

#### 功能
##### 用户输出设备/显示
- 3x5, 5x7 ASCII字体
- SSD1306 128x64液晶
- 数码管
##### 用户输入设备

##### 接口/通信
- I2C
- GPIO
##### 传感器
- 片上ADC
- MCU内置温度传感器
- DHT11
- 超声波模块
##### 执行器
- 28BYJ-48步进电机
- 直流电机
##### 控制器/数据处理
- 抽象数据Buffer
##### 其他
- RTC时钟


#### 功能-计划
- 电源(电池)管理
- 步进电机PWM细分
- 舵机
- PID控制
- 加热器温度控制
- 4Pin风扇
- W25Qxx SPI Flash
- SPI/SDIO SD卡
- LED点阵
- 4x4键盘
- BMP280
- MPU6050六轴/MPU9250九轴
- 38kHz红外
- SX1276/1278 LoRa
- 声波通信, FSO光通信, OLED传输数据


#### 使用说明

1.  打开STM32CubeIDE
2.  创建项目，适当配置外设，生成代码
3.  右键项目，点击Convert to C++
4.  复制本仓库到项目目录
5.  右键项目 -> Properties -> C/C++ General -> Paths and Symbols -> Includes -> 添加所需的头文件目录
    mcu_mylibs/commom/Inc
    mcu_mylibs/stm32/Inc
    mcu_mylibs/stm32f1/Inc (f4项目添加对应目录)
6.  右键项目 -> Properties -> C/C++ General -> Paths and Symbols -> Source Location -> 展开当前项目 -> Filter -> Edit Filter
    添加example, F1项目需添加*f4*, F4项目需添加*f1*
7.  编辑配置文件 mcu_mylibs/common/Inc/mylibs_config.hpp


#### 命名规则
| 缩写 | 全称       |  说明        |
|:----  |:----      |:---------     |
| C_xxx | class     | C++对象        |
| I_xxx | interface | 跨平台接口，不可直接使用 |

#### 非MCU代码
PC端工具，大多由Python编写，在ubuntu下测试过:  
    RTC时间同步，点阵/数码管字体生成，二值图动画生成
Android端工具（计划）  
自动配置工具（计划）  


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
