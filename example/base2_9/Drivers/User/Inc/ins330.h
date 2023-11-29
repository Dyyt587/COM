#ifndef INS_330_H
#define INS_330_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
	
	
	/**
Byte
Offset
Name Format Notes Scaling unit Description
0
Counter U4 MSB first 1 ms Unsigned int, 4 bytes
4 Roll I2 MSB first 360°/2^16 °
6 Pitch I2 MSB first 360°/2^16 °
8 Yaw I2 MSB first 360°/2^16 °
10
Steering angle of front
wheel
I2 MSB first 360°/2^16 °
12 Accel_X_Master I2 MSB first 20/2^16 g
14 Accel_Y_Master I2 MSB first 20/2^16 g
16 Accel_Z_Master I2 MSB first 20/2^16 g
18 Gyro_X_Master I2 MSB first 1260°/2^16 °/sec
20 Gyro_Y_Master I2 MSB first 1260°/2^16 °/sec
22 Gyro_Z_Master I2 MSB first 1260°/2^16 °/sec
24 Steering angle rate
I2 MSB first 1260°/2^16 °/sec Steering angle rate of
front wheel
26 Vehicle speed
I2 MSB first 0.001 m/s Positive and negative
value to show speed of
advancing or retreating
will be better
28 Master algorithm states
2 bytes MSB first
30 Slave algorithm states
2 bytes MSB first
*/

#define STATE_IDEL 250
#define STATE_CPLT 251
	typedef struct
	{
		uint8_t databuf[256];
		uint8_t data_index;
		uint8_t data_length;
		uint8_t is_updata;
		
		
		uint32_t counter;
		float roll;
		float pitch;
		float yaw;
		float steering_angle;
		float accel_x_master;
		float accel_y_master;
		float accel_z_master;
		float gyro_x_master;
		float gyro_y_master;
		float gyro_z_master;
		float steering_angle_rate;
		float vehicle_speed;
		uint16_t master_algorithm_states;
		uint16_t slave_algorithm_states;
		uint16_t crc;
	}ins330_t;
	void ins330_init(ins330_t* imu330);
	//void ins330_read(char* buf, int toread_cnt, int* read_cnt);
	void ins330_handle(ins330_t* imu330,char data);
void ins330_show_test(ins330_t* imu330);





#ifdef __cplusplus
}
#endif
#endif // __cplusepluse
