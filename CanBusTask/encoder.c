//未注释，未报错
#include "main.h"
#include "encoder.h"
#include "CanBusTask.h"


int dtm123[6]; //2022加 临时储存下枪数据 声明为外部变量


void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg)
{
		v->ecd_bias = (msg->Data[0]<<8)|msg->Data[1];  //保存初始编码器值作为偏差  
		v->raw_value= v->ecd_bias;
		v->ecd_value = v->ecd_bias;
}

//2310 与2006使用同一套解码
void Motor_2310_EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
	int i=0;
	int32_t temp_sum = 0;    
	v->last_raw_value = v->raw_value;
	v->raw_value = (msg->Data[0]<<8)|msg->Data[1];
	v->diff = v->raw_value - v->last_raw_value;
	
	if(v->diff < -4096)    //两次编码器的反馈值差别太大，表示圈数发生了改变
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>4096)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff- 8192;
	}
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//保存diff
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	
	//计算得到连续的编码器输出值
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	
	//计算得到角度值，范围正负无穷大
	v->ecd_angle = (double)(v->raw_value - v->ecd_bias)*(360/36.0)/8192 + v->round_cnt * 360/36.0;//减速比36:1
	
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	//计算速度平均值
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	v->filter_rate = (int32_t)(temp_sum/RATE_BUF_SIZE);			
}

void Motor_6623_EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
	int i=0;
	int32_t temp_sum = 0;    
	v->last_raw_value = v->raw_value;
	v->raw_value = (msg->Data[0]<<8)|msg->Data[1];
	v->diff = v->raw_value - v->last_raw_value;
	
	if(v->diff < -4096)    //两次编码器的反馈值差别太大，表示圈数发生了改变
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>4096)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff- 8192;
	}
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//保存diff
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	
	//计算得到连续的编码器输出值
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	
	//计算得到角度值，范围正负无穷大
	v->ecd_angle = (double)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;   //1:1
	
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	//计算速度平均值
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	v->filter_rate = (int32_t)(temp_sum/RATE_BUF_SIZE);			
}

//采用的是绝对式编码器,码盘式角度-数字编码器,13位码盘；
//假定码盘在读取周期内，旋转角度小于180°；
//偏差值小于-4096，说明顺时针通过零点，圈数+1;偏差值大于4096，说明逆时针通过零点，圈数-1；
//顺时针通过零点，表示编码器开始走下一圈；逆时针通过零点，表示编码器退回上一圈；
//经运算后的数值，大于0为顺时针；小于0为逆时针；
//2^13=8192,角度分辨率为360/(2^13)=0.0439°；
//读取周期T计算：2π*rmp*T/60<π,得：T<30/rmp (s)   ,其中rmp为编码器转速，单位圈/分；
//码盘：顺时针为计数值增大的方向，逆时针为计数值减小的方向。
//
//            0
//
//     
//   6144           2048   
//
//
//          4096

void Motor_6020_EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
	int i=0;
	int32_t temp_sum = 0;
  int16_t torque_current6623 = 0;
  int16_t motor_speed = 0;	
	v->last_raw_value = v->raw_value;
	v->raw_value = (msg->Data[0]<<8)|msg->Data[1];
	v->diff = v->raw_value - v->last_raw_value;
	
	if(v->diff < -4096)    //两次编码器的反馈值差别太大，表示圈数发生了改变
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>4096)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff- 8192;
	}
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//保存diff
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	
	//计算得到连续的编码器输出值
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	
	//计算得到角度值，范围正负无穷大
	v->ecd_angle = (double)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;//+ v->round_cnt * 360;
	v->ecd_xtl_angle = (double)(v->raw_value - v->ecd_bias)*(360)/8192;	
	
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	//计算速度平均值
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	motor_speed = (msg->Data[2]<<8)|msg->Data[3];
	v->filter_rate = motor_speed *360/60;	
	torque_current6623= (msg->Data[4]<<8)|msg->Data[5];
	v->real_torque_current = torque_current6623;
}


void Motor_3508_EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
	int i=0;
	int32_t temp_sum = 0;
  int16_t temp_filter=0;	
	int16_t torque_current = 0;
	v->last_raw_value = v->raw_value;
	v->raw_value = (msg->Data[0]<<8)|msg->Data[1];
	v->diff = v->raw_value - v->last_raw_value;
	
	if(v->diff < -4096)    //两次编码器的反馈值差别太大，表示圈数发生了改变
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>4096)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff- 8192;
	}
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//保存diff
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	
	//计算得到连续的编码器输出值
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	
	//计算得到角度值，范围正负无穷大
	v->ecd_angle = (double)(v->raw_value - v->ecd_bias)*(360/19.0)/8192 + v->round_cnt * 360/19.0;//减速比36:1
	
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	//计算速度平均值
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	//v->filter_rate = (int32_t)(temp_sum/RATE_BUF_SIZE);	
	temp_filter = ((msg->Data[2]<<8)|msg->Data[3]);
	v->filter_rate = temp_filter/19.;
	torque_current=((msg->Data[4]<<8)|msg->Data[5]);
	v->real_torque_current = torque_current;
}

//2022加 板间通信 Down to Up接受
//数据具体意义暂时未定

void Down_to_Up_Flag(CanRxMsg * msg)
{
	dtm123[0]=msg->Data[0];
	dtm123[1]=msg->Data[1];
	dtm123[2]=msg->Data[2];
	dtm123[3]=msg->Data[3];
	dtm123[4]=msg->Data[4];
	dtm123[5]=msg->Data[5];
}

