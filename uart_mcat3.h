#ifndef UART_MCAT3_H
#define UART_MCAT3_H

void uart_putc(uint8_t c, uint8_t select);

void uart_puts(char *str, uint8_t select);

void uart_putln(char *str, uint8_t select);

void uart_putint(int n, uint8_t select);

void uart_putintln(int n, uint8_t select);

void uart_putHEX(int n, uint8_t select);

char uart_receive(uint8_t select);

void uart_puts(char *str, uint8_t select);

int analybuf(uint8_t* str,uint8_t* start,uint8_t end,double* re);

int analytwe(uint8_t* str,uint8_t* start,uint8_t end,int* re);

int gps_renew(uint8_t* str,uint8_t* start,uint8_t end,double *data,uint8_t start_read);
#endif
