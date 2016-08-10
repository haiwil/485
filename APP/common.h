/**************************************************************************************************
  Filename:       common.h
  Revised:        $Date: 2013-07-18 13:54:23  $
  Revision:       $Revision: 00001 $

  Description:    16 Î»CRC Ð£ÑéÂë
**************************************************************************************************/

#ifndef COMMON_H
#define COMMON_H

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEF
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */
extern unsigned int cyg_crc16(unsigned char *puchMsg, unsigned int usDataLen) ;
extern int hex_2_ascii( unsigned char  *indata, unsigned char *outbuffer, int len);
extern unsigned char ASCII_to_16(unsigned char ch);
extern int BCD_to_hex(unsigned char* indata,unsigned char *outdata,int len);
extern unsigned long HEX2BCD(unsigned short hex_data);
extern unsigned char check_frame(unsigned char*buff,unsigned int freamLen);
extern void quicksort(unsigned char *list,unsigned char m);
extern unsigned char BCC_XOR(unsigned char *s,unsigned char n);
extern void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
/*********************************************************************
*********************************************************************/

#endif /* CRC_16_H */
