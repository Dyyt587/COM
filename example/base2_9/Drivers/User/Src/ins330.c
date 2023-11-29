#include "ins330.h"
#include "stdint.h"
#include "elog.h"

///*******************************************************************************
//* FUNCTION: calcCRC calculates a 2-byte CRC on serial data using
//* CRC-CCITT 16-bit standard maintained by the ITU
//* (International Telecommunications Union).
//* ARGUMENTS: queue_ptr is pointer to queue holding area to be CRCed
//* startIndex is offset into buffer where to begin CRC calculation
//* num is offset into buffer where to stop CRC calculation
//* RETURNS: 2-byte CRC
//*******************************************************************************/
//unsigned short calcCRC(QUEUE_TYPE *queue_ptr, unsigned int startIndex, unsigned int num) {
//unsigned int i=0, j=0;
//unsigned short crc=0x1D0F; //non-augmented initial value equivalent to augmented initial value 0xFFFF
//for (i=0; i<num; i+=1) {
//crc ^= peekByte(queue_ptr, startIndex+i) << 8;
//for(j=0;j<8;j+=1) {
//if(crc & 0x8000) crc = (crc << 1) ^ 0x1021;
//else crc = crc << 1;
//}
//}
//return crc;
//}

void ins330_init(ins330_t* imu330)
{
	imu330->data_index = STATE_IDEL;
	imu330->data_length = 38;
}
void ins330_show_test(ins330_t* imu330)
{
	if(imu330->is_updata){
//	log_d("\nins330 counter: %d ms \n", imu330->counter);
//	log_d("ins330 roll: %f ° \n", imu330->roll);
//	log_d("ins330 pitch: %f ° \n", imu330->pitch);
//	log_d("ins330 yaw: %f ° \n", imu330->yaw);
//	log_d("ins330 steering_angle: %f ° \n", imu330->steering_angle);
//	log_d("ins330 accel_x_master: %f g \n", imu330->accel_x_master);
//	log_d("ins330 accel_y_master: %f g \n", imu330->accel_y_master);
//	log_d("ins330 accel_z_master: %f g \n", imu330->accel_z_master);
//	log_d("ins330 gyro_x_master: %f °/sec \n", imu330->gyro_x_master);
//	log_d("ins330 gyro_y_master: %f °/sec \n", imu330->gyro_y_master);
//	log_d("ins330 gyro_z_master: %f °/sec \n", imu330->gyro_z_master);
//	log_d("ins330 steering_angle_rate: %f °/sec \n", imu330->steering_angle_rate);
//	log_d("ins330 vehicle_speed: %f m/s \n", imu330->vehicle_speed);
//	log_d("ins330 master_algorithm_states: %#x \n", imu330->master_algorithm_states);
//	log_d("ins330 slave_algorithm_states: %#x \n", imu330->slave_algorithm_states);
//	log_d("ins330 crc: %#x \n", imu330->crc);
		
		#define INFO "\nins330 counter: %d ms \nins330 roll: %f ° \nins330 pitch: %f ° \nins330 yaw: %f ° \nins330 steering_angle: %f ° \nins330 accel_x_master: %f g \nins330 accel_y_master: %f g \nins330 accel_z_master: %f g \nins330 gyro_x_master: %04.7f °/sec \nins330 gyro_y_master: %04.7f °/sec \nins330 gyro_z_master: %04.7f °/sec \nins330 steering_angle_rate: %04.7f °/sec \nins330 vehicle_speed: %f m/s \nins330 master_algorithm_states: %#x \nins330 slave_algorithm_states: %#x \nins330 crc: %#x \n\
		\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A\033[A"
//		log_d( INFO
//			, imu330->counter, imu330->roll, imu330->pitch, imu330->yaw, imu330->steering_angle, imu330->accel_x_master, imu330->accel_y_master
//			, imu330->accel_z_master, imu330->gyro_x_master, imu330->gyro_y_master, imu330->gyro_z_master, imu330->steering_angle_rate, imu330->vehicle_speed
//			, imu330->master_algorithm_states, imu330->slave_algorithm_states, imu330->crc);
//		

			imu330->is_updata=0;
	}

}
static void ins330_analysis_packet(ins330_t* imu330)
{
	uint16_t type = imu330->databuf[2] << 8 | imu330->databuf[3];

	switch (type)
	{
	case 0x4533:
		imu330->counter = (imu330->databuf[5] << 24 | imu330->databuf[6] << 16 | imu330->databuf[7] << 8 | imu330->databuf[8])*10;
		
		imu330->roll = ((float)((imu330->databuf[9] << 8 | imu330->databuf[10]) * 360)) / (65536.f);
		imu330->pitch = (float)((imu330->databuf[11] << 8 | imu330->databuf[12]) * 360	)	/	(65536.f);
		imu330->yaw   = (float)((imu330->databuf[13] << 8 | imu330->databuf[14]) * 360	)	/	(65536.f);
		imu330->steering_angle = (float)((short)(imu330->databuf[15] << 8 | imu330->databuf[16]) * 0.00549324788281071183337148088808f);
		imu330->accel_x_master = (float)((short)(imu330->databuf[17] << 8 | imu330->databuf[18]) * 3.0518043793392843518730449378195e-4f);
		imu330->accel_y_master = (float)((short)(imu330->databuf[19] << 8 | imu330->databuf[20]) * 3.0518043793392843518730449378195e-4f);
		imu330->accel_z_master = (float)((short)(imu330->databuf[21] << 8 | imu330->databuf[22]) * 3.0518043793392843518730449378195e-4f);

		imu330->gyro_x_master = (float)((short)(imu330->databuf[23] << 8 | imu330->databuf[24]) *0.01922636758983749141680018310826f);
		imu330->gyro_y_master = (float)((short)(imu330->databuf[25] << 8 | imu330->databuf[26]) *0.01922636758983749141680018310826f);
		imu330->gyro_z_master = (float)((short)(imu330->databuf[27] << 8 | imu330->databuf[28])*0.01922636758983749141680018310826f );
		//imu330->gyro_z_master = (float)((int)(imu330->databuf[27] << 8 | imu330->databuf[28]) * 1260)/ (65536.f/2);
		imu330->steering_angle_rate = (float)((short)(imu330->databuf[29] << 8 | imu330->databuf[30])*0.01922636758983749141680018310826f);
		imu330->vehicle_speed = ((float)(imu330->databuf[31] << 8 | imu330->databuf[32]) * 0.001f);


		imu330->master_algorithm_states = imu330->databuf[33] << 8 | imu330->databuf[34];
		imu330->slave_algorithm_states = imu330->databuf[35] << 8 | imu330->databuf[36];
		imu330->crc = imu330->databuf[imu330->data_length - 2] << 8 | imu330->databuf[imu330->data_length - 1];
		//ins330_show_test(imu330);
		imu330->is_updata =1;
	///	ins330_show_test(imu330);

		break;
	default:
		//log_e("%d",type);
		break;
	}

}
void ins330_handle(ins330_t* imu330,char data)
{
	static int state=0;//空闲
//	
//	
//	if((data == 0x55)&& state==0)
//	{
//		imu330->data_index=0;
//		imu330->databuf[imu330->data_index++] = data;
//		state=1;
//	}
//	if((data == 0x55)&& state==1)
//	{
//		imu330->databuf[imu330->data_index++] = data;
//		state=2;
//	}
//	if(state==2){
//				imu330->databuf[imu330->data_index++] = data;
//		if(imu330->data_index == 4){
//			imu330->data_length=imu330->databuf[4]+2+2+2;
//		}

//	}
//	if(imu330->data_index>imu330->data_length){
//		imu330->data_index =0;
//		state=0;
//		ins330_analysis_packet(imu330);

//	}
//	
//	
//	if(state>1){
//	
//				imu330->databuf[imu330->data_index++] = data;
//		if(imu330->data_index == 5){
//			imu330->data_length=imu330->databuf[4]+2+2+2;
//		}
//		if(imu330->data_index>imu330->data_length){
//					ins330_analysis_packet(imu330);

//		imu330->data_index =0;
//		state=0;

//	}
//	}else{
//	if((data == 0x55)&& state==0)
//	{
//		imu330->databuf[0] = data;
//		state=1;
//	}
//	if((data == 0x55)&& state==1)
//	{
//				imu330->data_index=1;

//		imu330->databuf[1] = data;
//		imu330->data_length=38;
//		state=2;
//	}
//	}
//	
//	
	
	
	
//		if (imu330->data_index == 5) {//接收到length
//			//imu330->data_length = imu330->databuf[4]+2+2+2;//包头 type crc
//			imu330->data_length = 39;//包头 type crc

//		}
////		if(imu330->data_index>2 && imu330->data_index<20){
////			if(imu330->databuf[1] != imu330->databuf[0]){
////				imu330->data_index = STATE_IDEL;
////			}
////		}

//		if ((data == 0x55) && (imu330->data_index == STATE_IDEL))//接收到包头,并且空闲状态
//		{
//			imu330->data_index = 0;
//			imu330->databuf[imu330->data_index] = data;
//			imu330->data_index++;
//		}
//		else 
//		{
//			imu330->databuf[imu330->data_index++] = data;
////			if(imu330->data_index>60){
////				imu330->data_index = STATE_IDEL;

////			}

//		}
//				if (imu330->data_index == imu330->data_length-1) {
//			//TODO:crc数据
//			imu330->data_index = STATE_CPLT;
//			//数据包解析
//			ins330_analysis_packet(imu330);
//			imu330->data_index = STATE_IDEL;
//		}
//				
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	if ((data == 0x55) && (state == 0))
	{
		imu330->data_index=0;
		imu330->databuf[imu330->data_index++] = data;
		state=1;
		return;
}
else {
	if ((data == 0x55) && (state == 1))
	{
		imu330->databuf[imu330->data_index++] = data;
		state = 2;
		return;
	}
	if (state == 2) {
		imu330->databuf[imu330->data_index++] = data;
		if (imu330->data_index == 4) {
			imu330->data_length = 39;
		}

	}
	if (imu330->data_index > imu330->data_length-1) {
		imu330->data_index = 0;
		state = 0;
		ins330_analysis_packet(imu330);

	}

}

}
