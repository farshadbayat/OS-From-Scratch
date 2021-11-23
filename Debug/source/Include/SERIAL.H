
#ifndef __SERIAL_H
#define	__SERIAL_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#define PORT 0x3f8   /* ttyS0 */
	
	void InstallSerialPort();
	void init_serial();
	char read_serial(int port);
	void write_serial(int port, char a);

#ifdef __cplusplus
}
#endif

#endif
