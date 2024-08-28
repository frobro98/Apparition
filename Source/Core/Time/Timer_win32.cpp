
#include "Timer.h"
#include <windows.h>

const Time Timer::privGetSystemTime()
{
	const static Time oneSecond(Time::ONE_SECOND);
	
	Time systemTime;
	 
	LARGE_INTEGER counterStruct;
	LARGE_INTEGER frequencyStruct;

	if ( 0 != QueryPerformanceCounter( &counterStruct ) && 0 != QueryPerformanceFrequency( &frequencyStruct ) )
	{
		const LONGLONG counter   = counterStruct.QuadPart;
		const LONGLONG frequency = frequencyStruct.QuadPart;

		if ( counter >= 0 && 0 != frequency )
		{
			const int numSeconds = static_cast< int >( counter / frequency );
			const float remainder = static_cast< float >( counter % frequency );

			systemTime  = numSeconds * oneSecond;
			systemTime += ( remainder / frequency ) * oneSecond;
		}
	}

	return( systemTime );
}

