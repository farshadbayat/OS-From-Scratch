
#ifndef __SOUND_H
#define	__SOUND_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#include <ctype.h>

	#define SPEAKER_CTRL	0x43
	#define SPEAKER_DATA	0x42

	void sound(uint frequency);
	void nosound();


#ifdef __cplusplus
}
#endif

#endif
