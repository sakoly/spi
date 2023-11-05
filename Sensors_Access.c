/*
 * Sensors_Access.c
 */
#include <stdio.h>
#include <Sensors_Access.h>
#include "main.h"
#include "stm32f7xx_hal.h"
#include "math.h"

SPI_HandleTypeDef hspi1;//mpu9250
SPI_HandleTypeDef hspi2;//LPS25
SPI_HandleTypeDef hspi3;//dps310

//ICM20602 Define
#define ICM_SPI hspi2 //ICM SPI
#define ICM_Data_Adress 0x3B //�Z���f�[�^��ۊǂ��郌�W�X�^�̃A�h���X
#define ICM_WakeUp_Adress 0x6B //ICM20602�X���[�v�����A�h���X
#define ICM_Accelerometer_Setting_Adress 0x1B //ICM20602�����x�����W�ݒ�A�h���X
#define ICM_Gyro_Setting_Adress 0x1C //ICM20602�p���x�����W�ݒ�A�h���X
#define ICM_WhoAmI_Adress 0x75//WhoAmI�̃A�h���X
#define ICM_16G 0x18
#define ICM_2500deg 0x18
//LPS25HB�̏��萔
#define LPS_SPI hspi2 //LPS SPI
#define LPS_Data1_Adress 0x28 //�C���f�[�^��ۊǂ��郌�W�X�^�̃A�h���X
#define LPS_Data2_Adress 0x2B
#define LPS_WakeUp_Adress 0x20 //LPS22HB�X���[�v�����A�h���X
#define LPS_WakeUp_Value 0x90 //LPS22HB�X���[�v������������
#define LPS_WhoAmI_Adress 0x0F//WhoAmI�̃A�h���X
//DPS310
#define DPS_SPI hspi3 //DPS SPI
//MPU9250 Define
#define MPU_SPI hspi1//MPU SPI
#define MPU_Data_Adress 0x3B
#define MPU_WakeUp_Adress 0x6B
#define MPU_Accelerometer_Setting_Adress 0x1C
#define MPU_Gyro_Setting_Adress 0x1B
#define MPU_WhoAmI_Adress 0x75//return 0x71
#define rate 0.001//sampling_rate
#define G 16383//Propulsive force balanced by gravity
//CS�s���͂����Őݒ�

void ICM_CS_Low(){
#ifdef ICMCS_GPIO_Port
	HAL_GPIO_WritePin(ICMCS_GPIO_Port,ICMCS_Pin,RESET);
#endif
}
void ICM_CS_High(){
#ifdef ICMCS_GPIO_Port
	HAL_GPIO_WritePin(ICMCS_GPIO_Port,ICMCS_Pin,SET);
#endif
}


void LPS_CS_Low(){
#ifdef LPSCS_GPIO_Port
    HAL_GPIO_WritePin(LPSCS_GPIO_Port,LPSCS_Pin,RESET);
#endif
}
void LPS_CS_High(){
#ifdef LPSCS_GPIO_Port
	HAL_GPIO_WritePin(LPSCS_GPIO_Port,LPSCS_Pin,SET);
#endif
}


void DPS_CS_Low(){
#ifdef DPSCS_GPIO_Port
    HAL_GPIO_WritePin(DPSCS_GPIO_Port,DPSCS_Pin,RESET);
#endif
}
void DPS_CS_High(){
#ifdef DPSCS_GPIO_Port
	HAL_GPIO_WritePin(DPSCS_GPIO_Port,DPSCS_Pin,SET);
#endif
}

void MPU_CS_Low(){
#ifdef MPUCS_GPIO_Port
	HAL_GPIO_WritePin(MPUCS_GPIO_Port,MPUCS_Pin,RESET);
#endif
}
void MPU_CS_High(){
#ifdef MPUCS_GPIO_Port
	HAL_GPIO_WritePin(MPUCS_GPIO_Port,MPUCS_Pin,SET);
#endif
}

//�ȉ��}�C�R���̐ݒ�Ɉˑ����Ȃ��֐�
//------------------------------------------------------------------------------------------
void SPI_Transmit(uint8_t value, SPI_HandleTypeDef spi){
	HAL_SPI_Transmit(&spi,(uint8_t*)&value,1,100);
}

void SPI_TransmitRecieve(uint8_t RecieveData[],int Size, SPI_HandleTypeDef spi){
	uint8_t DummyData = 0x00;
	HAL_SPI_TransmitReceive(&spi, &DummyData, RecieveData,Size,200);
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
void ICM_Write_Byte(uint8_t Adress,uint8_t Value){
	ICM_CS_Low();
	SPI_Transmit(Adress,ICM_SPI);
	SPI_Transmit(Value,ICM_SPI);
	ICM_CS_High();
}

void ICM_Read_Bytes(uint8_t Adress,uint8_t RecieveData[], int Size){
	Adress = Adress | 0x80;

	ICM_CS_Low();
	SPI_Transmit(Adress,ICM_SPI);
	SPI_TransmitRecieve(RecieveData,Size,ICM_SPI);
	ICM_CS_High();
}

void LPS_Write_Byte(uint8_t Adress,uint8_t Value){
	LPS_CS_Low();
	SPI_Transmit(Adress,LPS_SPI);
	SPI_Transmit(Value,LPS_SPI);
	LPS_CS_High();
}
void LPS_Read_Bytes(uint8_t Adress,uint8_t RecieveData[], int Size){
	Adress = Adress | 0x80;
	LPS_CS_Low();
	SPI_Transmit(Adress,LPS_SPI);
	SPI_TransmitRecieve(RecieveData,Size,LPS_SPI);
	LPS_CS_High();
}
void LPS_sequent_Read_Bytes(uint8_t Adress,uint8_t RecieveData[], int Size){
	Adress = Adress | 0x80;
	Adress = Adress | 0x40;
	LPS_CS_Low();
	SPI_Transmit(Adress,LPS_SPI);
	SPI_TransmitRecieve(RecieveData,Size,LPS_SPI);
	LPS_CS_High();
}

void DPS_Write_Byte(uint8_t Adress,uint8_t Value){
	DPS_CS_Low();
	SPI_Transmit(Adress,DPS_SPI);
	SPI_Transmit(Value,DPS_SPI);
	DPS_CS_High();
}
void DPS_Read_Bytes(uint8_t Adress,uint8_t RecieveData[], int Size){
	Adress = Adress | 0x80;
	DPS_CS_Low();
	SPI_Transmit(Adress,DPS_SPI);
	SPI_TransmitRecieve(RecieveData,Size,DPS_SPI);
	DPS_CS_High();
}

void MPU_Write_Byte(uint8_t Adress,uint8_t Value){
	MPU_CS_Low();
	SPI_Transmit(Adress,MPU_SPI);
	SPI_Transmit(Value,MPU_SPI);
	MPU_CS_High();
}

void MPU_Read_Bytes(uint8_t Adress,uint8_t RecieveData[],int Size){
	Adress=Adress|0x80;
	MPU_CS_Low();
	SPI_Transmit(Adress,MPU_SPI);
	SPI_TransmitRecieve(RecieveData,Size,MPU_SPI);
	MPU_CS_High();
}
//------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------
uint8_t ICM_WhoAmI(){
	uint8_t RecieveData[1];
	ICM_Read_Bytes(ICM_WhoAmI_Adress,RecieveData,1);
	return RecieveData[0];
}

uint8_t LPS_WhoAmI(){
	uint8_t RecieveData[1];
	LPS_Read_Bytes(LPS_WhoAmI_Adress,RecieveData,1);
	return RecieveData[0];
}

uint8_t MPU_WhoAmI(){
	uint8_t RecieveData[1];
	MPU_Read_Bytes(MPU_WhoAmI_Adress,RecieveData,1);
	return RecieveData[0];
}
//------------------------------------------------------------------------------------------
//ICM20602�����ݒ�
void ICM_Initialization() {
	ICM_Write_Byte(ICM_WakeUp_Adress, 0x00); //�X���[�v����
	ICM_Write_Byte(ICM_Accelerometer_Setting_Adress, ICM_16G); //�����x�����W�ő�ɐݒ�
	ICM_Write_Byte(ICM_Gyro_Setting_Adress, ICM_2500deg); //�p���x�����W�ő�ɐݒ�
}


void ICM_6axis_Raw(int16_t ICM_Raw_Data[6]) {
	uint8_t RecieveData[14];
	ICM_Read_Bytes(ICM_Data_Adress,RecieveData,14);

	ICM_Raw_Data[0] = (RecieveData[0] << 8 | RecieveData[1] );
	ICM_Raw_Data[1] = (RecieveData[2] << 8 | RecieveData[3] );
	ICM_Raw_Data[2] = (RecieveData[4] << 8 | RecieveData[5] );
	ICM_Raw_Data[3] = (RecieveData[8] << 8 | RecieveData[9] );
	ICM_Raw_Data[4] = (RecieveData[10] << 8 | RecieveData[11] );
	ICM_Raw_Data[5] = (RecieveData[12] << 8 | RecieveData[13] );
}
void ICM_14Byte(uint8_t RecieveData[14]){
	ICM_Read_Bytes(ICM_Data_Adress,RecieveData,14);
	return;
}

void ICM_Calibration(int16_t Gyro_Calibrate_Data[3]) {
	int16_t ICM_Raw_Data[6];
	int CalibrateCount = 0;
	Gyro_Calibrate_Data[0] = 0;
	Gyro_Calibrate_Data[1] = 0;
	Gyro_Calibrate_Data[2] = 0;

	for(int i = 0;i<8000; i++) {
		ICM_6axis_Raw(ICM_Raw_Data);
		if(i>10){
			Gyro_Calibrate_Data[0] += ICM_Raw_Data[3];
			Gyro_Calibrate_Data[1] += ICM_Raw_Data[4];
			Gyro_Calibrate_Data[2] += ICM_Raw_Data[5];
			CalibrateCount++;
		}
		HAL_Delay(1);
	}
	Gyro_Calibrate_Data[0] = Gyro_Calibrate_Data[0]/CalibrateCount;
	Gyro_Calibrate_Data[1] = Gyro_Calibrate_Data[1]/CalibrateCount;
	Gyro_Calibrate_Data[2] = Gyro_Calibrate_Data[2]/CalibrateCount;
}

//LPS22HB�����ݒ�
void LPS_WakeUp() {
	LPS_Write_Byte(LPS_WakeUp_Adress,LPS_WakeUp_Value);//�X���[�v�����A�o��50Hz
}

uint32_t LPS_RawandPressure(uint8_t RecieveData[3]) {
	uint32_t PressureRaw;
	LPS_sequent_Read_Bytes(LPS_Data1_Adress,RecieveData,3);
	PressureRaw = (uint32_t)RecieveData[2] << 16 | (uint16_t)RecieveData[1] << 8 | RecieveData[0];
	return PressureRaw;
}

uint32_t LPS_Pressure_Raw() {
	uint32_t PressureRaw;
	uint8_t RecieveData[3];
	LPS_sequent_Read_Bytes(LPS_Data1_Adress,RecieveData,3);
	PressureRaw = (uint32_t)RecieveData[2] << 16 | (uint16_t)RecieveData[1] << 8 | RecieveData[0];
	return PressureRaw;
}

int LPS_Tempreture_Raw() {
	int TempretureRaw;
	uint8_t RecieveData[2];
	LPS_sequent_Read_Bytes(LPS_Data2_Adress,RecieveData,2);
	TempretureRaw = (uint16_t)RecieveData[1] << 8 | RecieveData[0];
	if (TempretureRaw >= 32768){
		TempretureRaw -= 65536;
	}
	return TempretureRaw;
}

//DPS310
void DPS_Initialization(int data[9]) {
	DPS_Write_Byte(0x06,0x36);
	HAL_Delay(1);
	DPS_Write_Byte(0x07,0xA6);
	HAL_Delay(1);
	DPS_Write_Byte(0x08,0x07);
	HAL_Delay(1);
	DPS_Write_Byte(0x09,0x0C);
	HAL_Delay(1);
	uint8_t RecieveData[15];
	DPS_Read_Bytes(0x10,RecieveData,15);
	int c0=RecieveData[0]<<4 | RecieveData[1]>>4 ;
	if (c0 & (1<<11)){
			c0 = c0 - (1<<12);
	}
	int c1=(0x0F&RecieveData[1])<<8 | RecieveData[2];
	if (c1 & (1<<11)){
			c1 = c1 - (1<<12);
	}
	int c00=RecieveData[3]<<12 | RecieveData[4]<<4 | RecieveData[5]>>4;
	if (c00 & (1<<19)){
			c00 = c00 - (1<<20);
	}
	int c10=(0x0F&RecieveData[5])<<16 | RecieveData[6]<<8 | RecieveData[7];
	if (c10 & (1<<19)){
			c10 = c10 - (1<<20);
	}
	int c01=RecieveData[8]<<8 | RecieveData[9];
	if (c01 & (1<<15)){
			c01 = c01 - (1<<16);
	}
	int c11=RecieveData[10]<<8 | RecieveData[11];
	if (c11 & (1<<15)){
			c11 = c11 - (1<<16);
	}
	int c20=RecieveData[12]<<8 | RecieveData[13];
	if (c20 & (1<<15)){
			c20 = c20 - (1<<16);
	}
	int c21=RecieveData[14]<<8 | RecieveData[15];
	if (c21 & (1<<15)){
			c21 = c21 - (1<<16);
	}
	int c30=RecieveData[16]<<8 | RecieveData[17];
	if (c30 & (1<<15)){
			c30 = c30 - (1<<16);
	}
	data[0]=c0;
	data[1]=c1;
	data[2]=c00;
	data[3]=c01;
	data[4]=c10;
	data[5]=c11;
	data[6]=c20;
	data[7]=c21;
	data[8]=c30;
}


int DPS_Pressure_Raw(){
	int PressureRaw;
	uint8_t RecieveData[3];
	DPS_Read_Bytes(0x00,RecieveData,3);
	PressureRaw = (uint32_t)RecieveData[2]  | (uint16_t)RecieveData[1] << 8 | RecieveData[0]<< 16;
	if(PressureRaw&(1<<23)){
		PressureRaw=PressureRaw-(1<<24);
	}
	return PressureRaw;
}

int DPS_Tempreture_Raw(){
	int TempretureRaw;
	uint8_t RecieveData[3];
	DPS_Read_Bytes(0x03,RecieveData,3);
	TempretureRaw = (uint32_t)RecieveData[2] | (uint16_t)RecieveData[1] << 8 | RecieveData[0]<< 16 ;
	if(TempretureRaw&(1<<23)){
		TempretureRaw=TempretureRaw-(1<<24);
	}
	return TempretureRaw;
}

double DPS_Altitude(int data[9]){
	double p=(double)DPS_Pressure_Raw()/1040384;
	double t=(double)DPS_Tempreture_Raw()/1040384;
	double truepressure=data[2] + p * (data[4] + p * (data[6] + p * data[8])) + t * data[3] + t * p * (data[5] + p * data[7]);
	//double truetempreture=data[0]*0.5+data[1]*t;
	return truepressure;
}

double DPS_Truetemp(int data[9]){
	double t=(double)DPS_Tempreture_Raw()/1040384;
	double truetempreture=data[0]*0.5+data[1]*t;
	return truetempreture;
}


//MPU9250 Functions
void MPU_Initialization(){
	MPU_Write_Byte(MPU_WakeUp_Adress,0x00);
	MPU_Write_Byte(0x19, 1);
	MPU_Write_Byte(0x1A, 1);
	MPU_Write_Byte(0x23,0x00);//mpuの設定　FIFOにしない
	MPU_Write_Byte(0x6A,0x30);//mpuの内部I2C有効化
	MPU_Write_Byte(0x24,0x5D);//mpuがI2Cでデータ読み込み中に割り込みさせない＋センサとのやり取りでSCL400KHzにする（地磁気センサの最高速mode2の最大SCL） ＋ストップビットを入れる（地磁気センサ曰く「命令終了時にはストップコンディションを入力してください。」とのこと）
	MPU_Write_Byte(0x25,0x0C);//I2Cスレーブ0のアドレスと地磁気センサのスレーブアドレスの対応
	MPU_Write_Byte(0x26,0x0A);//mpuがI2Cで書き込む対象を決めるアドレスと地磁気センサ制御アドレスの対応
	MPU_Write_Byte(0x63,0x16);//mpuがI2Cで書き込む内容を決めるアドレスと地磁気センサ制御内容（16ビットでやり取り＋地磁気センサ最高速mode2データ100Hz）
	MPU_Write_Byte(0x27,0x97);//mpuがI2Cでの初期設定書き込みやデータのやり取りをmpu内のスレーブ0で行うこととする＋データは偶数ビットあるとする＋データ量7バイトでやり取り
	MPU_Write_Byte(MPU_Accelerometer_Setting_Adress,0x00);//-16383
	MPU_Write_Byte(MPU_Gyro_Setting_Adress,0x00);//250/s
}

void MPU_6axis_Raw(uint8_t Recieve_Data[14],int16_t MPU_Raw_Data[6]){//data0_accel,data3_gyro
	MPU_Read_Bytes(0x80|MPU_Data_Adress,Recieve_Data,14);
	MPU_Raw_Data[0]=(Recieve_Data[0]<<8|Recieve_Data[1]);
	MPU_Raw_Data[1]=-(Recieve_Data[2]<<8|Recieve_Data[3]);
	MPU_Raw_Data[2]=(Recieve_Data[4]<<8|Recieve_Data[5]);//	軸を向けたときに符号あり最大となるように設定。
	MPU_Raw_Data[3]=-(Recieve_Data[8]<<8|Recieve_Data[9]);
	MPU_Raw_Data[4]=(Recieve_Data[10]<<8|Recieve_Data[11]);
	MPU_Raw_Data[5]=-(Recieve_Data[12]<<8|Recieve_Data[13]);
}

int MPU_Compass_Raw(uint8_t Receive_Data[7],int16_t MPU_Raw_Data[3]){
    MPU_Write_Byte(0x25,0x0c|0x80);//スレーブアドレスを読み取り用へ
    MPU_Write_Byte(0x26,0x03);//I2Cを用いて地磁気センサとmpuのやり取りするアドレスの対応
    MPU_Write_Byte(0x27,0x97);//地磁気センサからmpuへのデータの読み出し有効化
    uint8_t s[1];
    MPU_Read_Bytes(0x3A,s,1);//データが準備できているか
    if(s[0]&0x01){
    	MPU_Read_Bytes(0x49,Receive_Data,7);//EXT_SENS_DATA_00からI2Cのデータが入る　最後のデータはST2（データ状態）で読み込まないと更新されない
    	MPU_Raw_Data[0]=-(Receive_Data[3]<<8|Receive_Data[2]);//	半径500
    	MPU_Raw_Data[1]=(Receive_Data[1]<<8|Receive_Data[0]);
    	MPU_Raw_Data[2]=(Receive_Data[5]<<8|Receive_Data[4]);
    	MPU_Raw_Data[0]=MPU_Raw_Data[0]*40+3010;
    	MPU_Raw_Data[1]=MPU_Raw_Data[1]*24-200;
    	MPU_Raw_Data[2]=MPU_Raw_Data[2]*16-900;
		return 1;
    }else{
    	return 0;
    }
}


void MPU_Compass_correct2(int16_t MPU_Raw_Data[3]){
	int q2=(int)(sqrt(MPU_Raw_Data[0]*MPU_Raw_Data[0]+MPU_Raw_Data[1]*MPU_Raw_Data[1]));
	MPU_Raw_Data[0]=MPU_Raw_Data[0]*1000/q2;
	MPU_Raw_Data[1]=MPU_Raw_Data[1]*1000/q2;
	MPU_Raw_Data[2]=0;
}

void MPU_6axis(uint16_t MPU_Data[6]){
	uint8_t Recieve_Data[14];
	int16_t MPU_Raw_Data[6];
	MPU_6axis_Raw(Recieve_Data,MPU_Raw_Data);
	for(int8_t i=0;i<6;i++){
		MPU_Data[i]=MPU_Raw_Data[i] *16.0*10/32764.0;
	}
}

void MPU_atitude(double* a0,double* a1,double* a2,double* a3,double* W1,double* W2,double* W3,int acgap_x,int acgap_y,int gygap_x,int gygap_y,int gygap_z,double B,double C){
	int16_t MPU_Raw_Datasub[3];
	int16_t MPU_Raw_Data[6];
	uint8_t MPU_Data[14];
	double w1,w2,w3;
	double q0=*a0,q1=*a1,q2=*a2,q3=*a3,Q;//formal_qtanion
	double mag1,mag2,mag3;//X0.5
	double dgf0,dgf1,dgf2,dgf3,dgfsqrt;//X0.25/G
	double dmf0=0,dmf1=0,dmf2=0,dmf3=0,dmfsqrt=1;//X0.25
	double dqw0,dqw1,dqw2,dqw3;
	double q01,q02,q03,q11,q12,q13,q22,q23,q33;
	int k;//for_calculate

	q01=q0*q1;
	q02=q0*q2;
	q03=q0*q3;
	q11=q1*q1;
	q12=q1*q2;
	q13=q1*q3;
	q22=q2*q2;
	q23=q2*q3;
	q33=q3*q3;
	//--------------------------
	k=MPU_Compass_Raw(MPU_Data,MPU_Raw_Datasub);
	if(k==1){
	//--------------------------
	  mag1=(0.5-q22-q33)*MPU_Raw_Datasub[0] + (q12-q03)*MPU_Raw_Datasub[1] + (q02+q13)*MPU_Raw_Datasub[2];
	  mag2=(q03+q12)*MPU_Raw_Datasub[0] + (0.5-q11-q33)*MPU_Raw_Datasub[1] + (q23-q01)*MPU_Raw_Datasub[2];
	  mag3=(q13-q02)*MPU_Raw_Datasub[0] + (q01+q23)*MPU_Raw_Datasub[1] + (0.5-q11-q22)*MPU_Raw_Datasub[2];
	  mag1=sqrt(mag1*mag1+mag2*mag2);
	//--------------------------
	  dmf0=-(mag1*(0.5-q22-q33)+mag3*(q13-q02)-MPU_Raw_Datasub[0]/2)*q2*mag3+
			  (mag1*(q12-q03)+mag3*(q01+q23)-MPU_Raw_Datasub[1]/2)*(-q3*mag1+q1*mag3)+
			  (mag1*(q02+q13)+mag3*(0.5-q11-q22)-MPU_Raw_Datasub[2]/2)*mag1*q2;
	  dmf1=(mag1*(0.5-q22-q33)+mag3*(q13-q02)-MPU_Raw_Datasub[0]/2)*mag3*q3+
			  (mag1*(q12-q03)+mag3*(q01+q23)-MPU_Raw_Datasub[1]/2)*(q2*mag1+q0*mag3)+
			  (mag1*(q02+q13)+mag3*(0.5-q11-q22)-MPU_Raw_Datasub[2]/2)*(mag1*q3-2*mag3*q1);
	  dmf2=-(mag1*(0.5-q22-q33)+mag3*(q13-q02)-MPU_Raw_Datasub[0]/2)*(2*mag1*q2+mag3*q0)+
			  (mag1*(q12-q03)+mag3*(q01+q23)-MPU_Raw_Datasub[1]/2)*(q1*mag1+q3*mag3)+
			  (mag1*(q02+q13)+mag3*(0.5-q11-q22)-MPU_Raw_Datasub[2]/2)*(mag1*q0-2*mag3*q2);
	  dmf3=(mag1*(0.5-q22-q33)+mag3*(q13-q02)-MPU_Raw_Datasub[0]/2)*(-2*mag1*q3-mag3*q1)+
			  (mag1*(q12-q03)+mag3*(q01+q23)-MPU_Raw_Datasub[1]/2)*(-q0*mag1+q2*mag3)+
			  (mag1*(q02+q13)+mag3*(0.5-q11-q22)-MPU_Raw_Datasub[2]/2)*mag1*q1;
	  dmfsqrt=sqrt(dmf0*dmf0+dmf1*dmf1+dmf2*dmf2+dmf3*dmf3);
	}
	//-------------------------
	MPU_6axis_Raw(MPU_Data,MPU_Raw_Data);
	MPU_Raw_Data[0]-=acgap_x;
	MPU_Raw_Data[1]-=acgap_y;

	dgf0=-(G*(q13-q02)-MPU_Raw_Data[0]/2)*q2+(G*(q01+q23)-MPU_Raw_Data[1]/2)*q1;
	dgf1=(G*(q13-q02)-MPU_Raw_Data[0]/2)*q3+(G*(q01+q23)-MPU_Raw_Data[1]/2)*q0-2*(G*(0.5-q11-q22)-MPU_Raw_Data[2]/2)*q1;
	dgf2=-(G*(q13-q02)-MPU_Raw_Data[0]/2)*q0+(G*(q01+q23)-MPU_Raw_Data[1]/2)*q3-2*(G*(0.5-q11-q22)-MPU_Raw_Data[2]/2)*q2;
	dgf3=(G*(q13-q02)-MPU_Raw_Data[0]/2)*q1+(G*(q01+q23)-MPU_Raw_Data[1]/2)*q2;
	dgfsqrt=sqrt(dgf0*dgf0+dgf1*dgf1+dgf2*dgf2+dgf3*dgf3);

	w1=(double)(MPU_Raw_Data[3]-gygap_x)*0.00013309461;//
	w2=(double)(MPU_Raw_Data[4]-gygap_y)*0.00013309461;
	w3=(double)(MPU_Raw_Data[5]-gygap_z)*0.00013309461;

	*W1=w1;
	*W2=w2;
	*W3=w3;

	dqw0=(-q1*w1-q2*w2-q3*w3)/2;
	dqw1=(q0*w1+q2*w3-q3*w2)/2;
	dqw2=(q0*w2-q1*w3+q3*w1)/2;
	dqw3=(q0*w3+q1*w2-q2*w1)/2;

	q0=q0+rate*(dqw0-(C*dmf0/dmfsqrt+B*dgf0/dgfsqrt));
	q1=q1+rate*(dqw1-(C*dmf1/dmfsqrt+B*dgf1/dgfsqrt));
	q2=q2+rate*(dqw2-(C*dmf2/dmfsqrt+B*dgf2/dgfsqrt));
	q3=q3+rate*(dqw3-(C*dmf3/dmfsqrt+B*dgf3/dgfsqrt));

	Q=sqrt(q0*q0+q1*q1+q2*q2+q3*q3);

	*a0=q0/Q;
	*a1=q1/Q;
	*a2=q2/Q;
	*a3=q3/Q;
}
