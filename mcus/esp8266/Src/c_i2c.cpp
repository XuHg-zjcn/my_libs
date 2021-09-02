#include "c_i2c.hpp"
#include "driver/i2c.h"

//参考i2c_examle_main.c
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_VAL (i2c_ack_type_t)0x0             /*!< I2C ack value */
#define NACK_VAL (i2c_ack_type_t)0x1            /*!< I2C nack value */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */

C_I2C::C_I2C()
{
    cmd = i2c_cmd_link_create();
}

#ifdef CI2C_ENABLE_NOSS
//返回值bool 是否有ACK
bool C_I2C::send_byte_noss(u8 data)
{
    return i2c_master_write_byte(cmd, data, true)==ESP_OK;
}

u8 C_I2C::recv_byte_noss(bool ack_en)
{
    u8 ret;
    i2c_master_read_byte(cmd, &ret, ack_en);
    return ret;
}

void C_I2C::send_noss(u8* p, u32 n)
{
    for(int i=0;i<n;i++){
        send_byte(*p++);
    }
}

void C_I2C::recv_noss(u8* p, u32 n)
{
    n--;
    for(int i=0;i<n;i++){
        *p++ = recv_byte(true);
    }
    *p = recv_byte(false);
}
#endif


C_I2C_Dev::C_I2C_Dev(C_I2C *ci2c, u8 dev_addr)
{
    this->ci2c = ci2c;
    this->dev_addr = dev_addr;
}

void C_I2C_Dev::Mem_write(u8 mem_addr, u8* p, u32 n)
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, p, n, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_cmd_link_delete(cmd);
}

void C_I2C_Dev::Mem_read(u8 mem_addr, u8* p, u32 n)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(ci2c->cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | READ_BIT, ACK_CHECK_EN);
    if (n > 1) {
        i2c_master_read(cmd, p, n - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, p + n - 1, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_cmd_link_delete(cmd);
}
