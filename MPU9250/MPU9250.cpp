#include "MPU9250.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

int main() {
    mraa_init();
    MPU9250* sensor = new MPU9250();
    
    printf("WhoAmI: %d",sensor->getWhoAmI());
    
    return 0;
}

MPU9250::MPU9250() {
    i2c = new mraa::I2c(1);
    i2c->frequency(mraa::MRAA_I2C_FAST);
    
    i2c->address(MPU9250_I2C_ADDR);
    i2c->writeReg(MPUREG_PWR_MGMT_1, BIT_H_RESET);
    i2c->writeReg(MPUREG_PWR_MGMT_1, 0x01);
    i2c->writeReg(MPUREG_PWR_MGMT_2, 0x00);
    
    i2c->writeReg(MPUREG_CONFIG, BITS_DLPF_CFG_256HZ_NOLPF2);
    
    i2c->writeReg(MPUREG_INT_PIN_CFG, 0x30);
    
    i2c->writeReg(MPUREG_USER_CTRL, 0x20);
    i2c->writeReg(MPUREG_I2C_MST_CTRL, 0x05);
    
    // configure accel
    _accelScale = SCALE_2G;
    i2c->writeReg(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // +-2G
    i2c->writeReg(MPUREG_ACCEL_CONFIG_2, 0x09); // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
    
    // configure gyro
    _gyroScale = SCALE_250DPS;
    i2c->writeReg(MPUREG_GYRO_CONFIG, BITS_FS_250DPS); // +-250dps
    
    // configure cmps
    // set addr of gyro
    i2c->writeReg(MPUREG_I2C_SLV0_ADDR, AK8963_I2C_ADDR);
    // reset/wake gyro
    i2c->writeReg(MPUREG_I2C_SLV0_REG, AK8963_CNTL2); // I2C slave 0 register address from where to begin data transfer
    i2c->writeReg(MPUREG_I2C_SLV0_DO, 0x01); // Reset AK8963
    i2c->writeReg(MPUREG_I2C_SLV0_CTRL, 0x81); // Enable I2C and set 1 byte
    usleep(1000);
    // set gyro to continuous mode
    i2c->writeReg(MPUREG_I2C_SLV0_REG, AK8963_CNTL1); // I2C slave 0 register address from where to begin data transfer
    i2c->writeReg(MPUREG_I2C_SLV0_DO, 0x12); // Register value to continuous measurement in 16bit
    i2c->writeReg(MPUREG_I2C_SLV0_CTRL, 0x81); // Enable I2C and set 1 byte
    usleep(1000);
}

MPU9250::Vector3* MPU9250::getAccel() {
    Vector3* accel = new Vector3();
    byte data[6];
    i2c->address(MPU9250_I2C_ADDR);
    spi->write(MPUREG_ACCEL_XOUT_H);
    spi->read(data, 6);
    
    short xRaw = ((short) data[0] << 8) | data[1];
    short yRaw = ((short) data[2] << 8) | data[3];
    short zRaw = ((short) data[4] << 8) | data[5];
    
    free(data);
    
    switch(_accelScale) {
        case SCALE_2G:
            accel->x = (float) xRaw * MPU9250A_2g;
            accel->y = (float) yRaw * MPU9250A_2g;
            accel->z = (float) zRaw * MPU9250A_2g;
            break;
        case SCALE_4G:
            accel->x = (float) xRaw * MPU9250A_4g;
            accel->y = (float) yRaw * MPU9250A_4g;
            accel->z = (float) zRaw * MPU9250A_4g;
            break;
        case SCALE_8G:
            accel->x = (float) xRaw * MPU9250A_8g;
            accel->y = (float) yRaw * MPU9250A_8g;
            accel->z = (float) zRaw * MPU9250A_8g;
            break;
        case SCALE_16G:
            accel->x = (float) xRaw * MPU9250A_16g;
            accel->y = (float) yRaw * MPU9250A_16g;
            accel->z = (float) zRaw * MPU9250A_16g;
    }
    
    return accel;
}

MPU9250::Vector3* MPU9250::getGyro() {
    Vector3* rot = new Vector3();
    byte data[6];
    i2c->address(MPU9250_I2C_ADDR);
    spi->write(MPUREG_GYRO_XOUT_H);
    spi->read(data, 6);
    
    short xRaw = ((short) data[0] << 8) | data[1];
    short yRaw = ((short) data[2] << 8) | data[3];
    short zRaw = ((short) data[4] << 8) | data[5];
    
    switch(_accelScale) {
        case SCALE_250DPS:
            rot->x = (float) xRaw * MPU9250G_250dps;
            rot->y = (float) yRaw * MPU9250G_250dps;
            rot->z = (float) zRaw * MPU9250G_250dps;
            break;
        case SCALE_500DPS:
            rot->x = (float) xRaw * MPU9250G_500dps;
            rot->y = (float) yRaw * MPU9250G_500dps;
            rot->z = (float) zRaw * MPU9250G_500dps;
            break;
        case SCALE_1000DPS:
            rot->x = (float) xRaw * MPU9250G_1000dps;
            rot->y = (float) yRaw * MPU9250G_1000dps;
            rot->z = (float) zRaw * MPU9250G_1000dps;
            break;
        case SCALE_2000DPS:
            rot->x = (float) xRaw * MPU9250G_2000dps;
            rot->y = (float) yRaw * MPU9250G_2000dps;
            rot->z = (float) zRaw * MPU9250G_2000dps;
    }
    
    return rot;
}

MPU9250::Vector3* MPU9250::getMagnet() {
    writeByte(MPUREG_I2C_SLV0_ADDR, AK8963_I2C_ADDR | READ_FLAG); // Set the I2C slave addres of AK8963 and set for read.
    writeByte(MPUREG_I2C_SLV0_REG, AK8963_HXL); // I2C slave 0 register address from where to begin data transfer
    writeByte(MPUREG_I2C_SLV0_CTRL, 0x87); // Read 6 bytes from the magnetometer
    usleep(1000);
    
    Vector3* mag = new Vector3();
    byte addr[7] = {MPUREG_EXT_SENS_DATA_00 | READ_FLAG, 0, 0, 0, 0, 0, 0};
    byte* data = (byte*) spi->write((char*) addr, 7);
    
    short xRaw = ((short) data[1] << 8) | data[2];
    short yRaw = ((short) data[3] << 8) | data[4];
    short zRaw = ((short) data[5] << 8) | data[6];
    
    free(data);
    
    mag->x = (float) xRaw * MPU9250M_4800uT;
    mag->y = (float) yRaw * MPU9250M_4800uT;
    mag->z = (float) zRaw * MPU9250M_4800uT;
    
    return mag;
}

void MPU9250::setAccelScale(AccelScale scale) {
    _accelScale = scale;
    writeByte(MPUREG_ACCEL_CONFIG, scale);
}

void MPU9250::setGyroScale(GyroScale scale) {
    _gyroScale = scale;
    writeByte(MPUREG_GYRO_CONFIG, scale);
}

MPU9250::AccelScale MPU9250::getAccelScale() {
    return _accelScale;
}

MPU9250::GyroScale MPU9250::getGyroScale() {
    return _gyroScale;
}

float MPU9250::getTemp() {
    return 0.0f;
}

void MPU9250::calibAccel() {
}

void MPU9250::calibMagnet() {
}

byte MPU9250::getWhoAmI() {
    i2c->address(MPU9250_I2C_ADDR);
    return i2c->readReg(MPUREG_WHOAMI);
}
