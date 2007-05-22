//
// C++ Implementation: teste_udf
//
// Description: 
//
//
// Author: Gianni Rossi <gianni.rossi@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
#include <ibase.h>

#include <ptypes.h>
USING_PTYPES;

#ifdef MT
#define STATIC_MT 
#define RWLOCK_READ scoperead( m_rwlock )
#define RWLOCK_WRITE scopewrite( m_rwlock )
#else
#define STATIC_MT static
#define RWLOCK_READ 
#define RWLOCK_WRITE 
#endif

/*
extern "C" {
extern char * icalendar( char * uid, char * from, char * to, char * subject, BLOBCALLBACK description, 
						 char * location, ISC_TIMESTAMP * tsStart, ISC_TIMESTAMP * tsEnd, 
						 int * opr );
}
*/

string intToString( int value, int digits = 0 )
{
	static const int len = 48;
	STATIC_MT char buffer[len];
	
	STATIC_MT char * p = buffer + len - 1;
	*p-- = 0;
	
	if ( digits > ( len - 1))
		digits = len - 1;
	
	if ( value )
	{
		while ( value && ( p != ( buffer - 1 )))
		{
			*p-- = '0' + ( value - (( value / 10 ) * 10));
			value /= 10;
			digits--;
		}
	}
	else
	{
		*p-- = '0';
		digits--;
	}
	
	while ( digits-- )
		*p-- = '0';
	
	string result = ++p;
	
	return result;
}


int main( )
{
	/* char uid[] = "9999",
		from[] = "from@domain.com",
		to[] = "to@domain.com",
		subject[] = "subject -- sub",
		location[] = "the location";
		 
	BLOBCALLBACK description = 0;
		
	ISC_TIMESTAMP tsStart, tsEnd;
	
	int opr = 1;
	
	icalendar( uid, from, to, subject, description, location, &tsStart, &tsEnd, &opr ); */
	
	string s = intToString( 12345, 8 );
	
	const char * p = s;
	std::cout << p << std::endl;
}
