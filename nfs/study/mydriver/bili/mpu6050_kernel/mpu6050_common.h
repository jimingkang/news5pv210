#ifndef _MPU6050_COMMON_H_
#define _MPU6050_COMMON_H_

#define MPU6050_MAGIC 'K'

//mpu6050数据结构
union mpu6050_data
{
    struct {
        short x;
        short y;
        short z;
    }accel;
    struct {
        short x;
        short y;
        short z;
    }gyro;
    unsigned short temp;
};

//mpu6050的ioctl的命令定义
#define GET_ACCEL _IOR(MPU6050_MAGIC, 0, union mpu6050_data)//读取加速度计的数据
#define GET_GYRO  _IOR(MPU6050_MAGIC, 1, union mpu6050_data)//读取陀螺仪的数据 
#define GET_TEMP  _IOR(MPU6050_MAGIC, 2, union mpu6050_data)//读取温度的数据

#endif

