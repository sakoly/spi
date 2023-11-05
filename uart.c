/*TX��DMA�ɑΉ��������ǔ�*/
#include <stdio.h>
#include <string.h> //strlen�֐��̎g�p�ɕK�v
#include <uart.h>
#include "main.h"

UART_HandleTypeDef huart6;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart1;

void uart_putc(uint8_t c, uint8_t select){
	char buf[1];
	buf[0] = c;
	if(select == 2){
		HAL_UART_Transmit(&huart2, (uint8_t *)buf, sizeof(buf), 100);
	} else if(select == 6) {
		HAL_UART_Transmit(&huart6, (uint8_t *)buf, sizeof(buf), 100);
	}else if(select == 5) {
		HAL_UART_Transmit(&huart5, (uint8_t *)buf, sizeof(buf), 100);
	}else if(select == 4) {
		HAL_UART_Transmit(&huart4, (uint8_t *)buf, sizeof(buf), 100);
	}else if(select == 3) {
		HAL_UART_Transmit(&huart3, (uint8_t *)buf, sizeof(buf), 100);
	}else if(select == 1) {
		HAL_UART_Transmit(&huart1, (uint8_t *)buf, sizeof(buf), 100);
	}
}
void uart_puts(char *str, uint8_t select){
	while(*str){
		uart_putc(*str++, select);
	}
}
void uart_putln(char *str, uint8_t select){
	uart_puts(str, select);
	uart_puts("\r\n", select);
}
void uart_putint(int n, uint8_t select){
	char buf[10];
	itoa(n,buf,10);
	uart_puts(buf, select);
}
void uart_putintln(int n, uint8_t select){
	uart_putint(n, select);
	uart_puts("\r\n", select);
}

void uart_putHEX(int n, uint8_t select){
	char buf[2];
	itoa(n,buf,16);
	uart_puts(buf, select);
}

char uart_receive(uint8_t select){
	char v[1];	
	if(select == 2){
		HAL_UART_Receive( &huart2, (uint8_t*) v, 1, 100);
	}else if(select == 6) {
		HAL_UART_Receive( &huart6, (uint8_t*) v, 1, 100);
	}else if(select == 5) {
		HAL_UART_Receive( &huart5, (uint8_t*) v, 1, 100);
	}else if(select == 4) {
		HAL_UART_Receive( &huart4, (uint8_t*) v, 1, 100);
	}else if(select == 3) {
		HAL_UART_Receive( &huart3, (uint8_t*) v, 1, 100);
	}else if(select == 1) {
		HAL_UART_Receive( &huart1, (uint8_t*) v, 1, 100);
	}
	return v[0];
}

void uart_puts_DMA(char *str, uint8_t select){
	if(select == 2){
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)str, strlen(str));
	}else if(select == 6){
		HAL_UART_Transmit_DMA(&huart6, (uint8_t *)str, strlen(str));
	}else if(select == 5){
		HAL_UART_Transmit_DMA(&huart5, (uint8_t *)str, strlen(str));
	}else if(select == 4){
		HAL_UART_Transmit_DMA(&huart4, (uint8_t *)str, strlen(str));
	}else if(select == 3){
		HAL_UART_Transmit_DMA(&huart3, (uint8_t *)str, strlen(str));
	}else if(select == 1){
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)str, strlen(str));
	}
}

void uart_putln_DMA(char *str, uint8_t select){
	uart_puts_DMA(str, select);
	uart_puts_DMA("\r\n", select);
}
// search one value form buffer, and renew where to read.return num_length
int analybuf(uint8_t* str,uint8_t* start,uint8_t end,double* re){
	uint8_t a;
	int b=0;
	int c=0;
	int d=0;
	int e=1;
	a=*start;
	for(;(str[a]<'0'||str[a]>'9')&&a!=end;a++);
	if(a==end)return 0;
	for(;(str[a]>='0'&&str[a]<='9')&&a!=end;a++){
		b*=10;
		b+=str[a]-'0';
		d++;
	}
	if(a==end)return 0;
	if(str[a]=='.'){
		if(a==end)return 0;
		a++;
		for(;(str[a]>='0'&&str[a]<='9')&&a!=end;a++){
			c*=10;
			c+=str[a]-'0';
			e*=10;
			d++;
		}
	}
	if(a==end)return 0;
	*re=b+(double)c/e;
	*start=a;
	return d;
}

int analytwe(uint8_t* str,uint8_t* start,uint8_t end,int* re){
	uint8_t a;
	int b=0;
	int d=1;
	for(a=*start;str[a]!=','&&a!=end;a++);
	if(a==end)return 0;
	a++;
	for(;(str[a]<'0'||str[a]>'9')&&a!=end;a++);
	if(a==end)return 0;
	b=str[a]-'0';
	a++;
	for(;a!=end&&str[a]!=',';a++){
		if(str[a]>='0'&&str[a]<='9'){
			b*=10;
			b+=str[a]-'0';
			d++;
		}
	}
	if(a==end)return 0;
	for(int c=*start;c<a;c++){
		str[c]='\0';
	}
	*re=b;
	*start=a;
	return d;
}


//renew time, latitude and longitude
int gps_renew(uint8_t* str,uint8_t* start,uint8_t end,double *data,uint8_t start_read){//start_read is next process
	int a,b;
	double re;
	switch(start_read){
	case 3:
		for(a=*start;str[a]=='G';a++){
			if(a==end)return 1;
		}
		*start=a;
	case 0:
		b=analybuf(str,start,end,&re);
		if(b!=0){
			if(b==9){
				data[0]=re;
			}
		}else{
			return 0;//next_renew_datanum=0
		}
	case 1:
		b=analybuf(str,start,end,&re);
		if(b!=0){
			if(b<10&&b>6){
				data[1]=re;
			}
		}else{
			return 1;
		}
	case 2:
		b=analybuf(str,start,end,&re);
		if(b!=0){
			if(b<10&&b>6){
				data[2]=re;
			}
		}else{
			return 2;
		}
	}
	return 3;
}
