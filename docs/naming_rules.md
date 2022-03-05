# 命名规范
## 类名命名规则
| 类名  | 全称      |  说明           |
| :---- | :----     | :-----------    |
| C_xxx | chip      | 片上设备        |
| E_xxx | extern    | 片外设备        |
| S_xxx | software  | 软件模拟的设备   |
| I_xxx | interface | 跨平台接口，不可直接使用 |
| 无前缀 |           | 该名称已经足够描述      |

## 目录规范
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
