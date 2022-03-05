//用宏定义`USE_VIRTUAL`选择是否使用抽象类虚函数
#ifdef USE_VIRTUAL
class C_Pin : public I_Pin{
#else
class C_Pin{
#endif

//在类声明和代码中插入(计划改用接口模板类，免去复制)
#ifndef USE_VIRTUAL
//从接口代码中复制
#endif

//使用宏定义选择所用的类（把软件I2C作为TM1650的I2C接口），放在头文件
#define TM1650_I2C S_I2C
