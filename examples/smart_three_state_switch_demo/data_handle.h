/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __DATA_HANDLE_H__
#define __DATA_HANDLE_H__

#define READ_ORDER      0x00
#define READ_ACK        0x01
#define WRITE_ORDER     0x02
#define WRITE_ACK       0x04
#define REPORT_ORDER    0x0A

#define CCW_CLUSTER     0x0100
#define NUMBER_STATES   0x00D0

void data_handle(unsigned short offset);

#endif