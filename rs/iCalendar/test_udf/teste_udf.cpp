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

#include <ibase.h>
#include <time.h>
#include <iostream>
#include <string>

extern "C" 
{
#ifdef MT
	extern bool getDone();
#endif
	
	extern char * icalendar( char * uid, char * to, char * subject, BLOBCALLBACK description, char * location, ISC_TIMESTAMP * tsStart, ISC_TIMESTAMP * tsEnd, int * opr );
		
	extern int set_smtp_host( char * value );
	extern int set_smtp_from( char * value );
	
	extern int set_smtp_auth( int * value );
	extern int set_smtp_tls( int * value );
	
	extern int set_smtp_user_name( char * value );
	extern int set_smtp_password( char * value );
};

int main()
{
	set_smtp_host( "smtp.mindsearch.com.br" );
	set_smtp_from( "rs@mindsearch.com.br" );
	
	int i = 1;
	
	set_smtp_auth( &i );
	set_smtp_tls( &i );
	
	set_smtp_user_name( "rs@mindsearch.com.br" );	
	set_smtp_password( "rsudf00" );
	
	tm timeStart, timeEnd;
	
	timeStart.tm_year = 107;
	timeStart.tm_mon = 8;
	timeStart.tm_mday = 1;
	
	timeStart.tm_hour = 10;
	timeStart.tm_min = 0;
	timeStart.tm_sec = 0;
	
	timeEnd = timeStart;
	timeEnd.tm_hour++;
	
	ISC_TIMESTAMP tsStart;
	ISC_TIMESTAMP tsEnd;
	
	isc_encode_timestamp( &timeStart, &tsStart );
	isc_encode_timestamp( &timeEnd, &tsEnd );
	
	int opr = 0;
	
	icalendar( "123-teste@xvjau.no-ip.org", "gianni.rossi@gmail.com", "Teste Reserva", 0, "Sala", &tsStart, &tsEnd, &opr );
#ifdef MT
	while ( ! getDone() ) 
		sleep ( 1 );
#endif
	return 0;
}
