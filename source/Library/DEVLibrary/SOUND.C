#include <sound.h>
#include <io.h>


void sound(uint frequency)
{
	byte status;
	frequency = 1193182 / frequency;
	
	outp8( SPEAKER_CTRL , 0xB6 );
	outp8( SPEAKER_DATA , (byte) frequency );
	outp8( SPEAKER_DATA , (byte) (frequency >> 0x08) );

	status = inp8(0x61);
	if(status != (status|0x03))
		outp8( 0x61 , status|3 ) ;
}

void nosound()
{
	outp8( 0x61 , inp8(0x61) & 0xFC );
}

