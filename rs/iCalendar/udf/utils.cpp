/*
	Reserva de Salas
	Copyright 2007 Gianfranco Rossi.

	Este programa é software livre; você pode redistribuí-lo e/ou
	modificá-lo sob os termos da Licença Pública Geral GNU, conforme
	publicada pela Free Software Foundation; tanto a versão 2 da
	Licença.

	Este programa é distribuído na expectativa de ser útil, mas SEM
	QUALQUER GARANTIA; sem mesmo a garantia implícita de
	COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
	PARTICULAR. Consulte a Licença Pública Geral GNU para obter mais
	detalhes.

	Você deve ter recebido uma cópia da Licença Pública Geral GNU
	junto com este programa; se não, escreva para a Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307, USA.
 */
 
#include "utils.h"


tm IsctstoDateTime( const ISC_TIMESTAMP * ts )
{
	STATIC_MT tm time;
	isc_decode_timestamp( ts, &time );
	
	return time;
}

// char * stringToChar( string s )
// {
// 	STATIC_MT char *result, *p;
// 	STATIC_MT const char *q;
// 
// #ifdef MT
// 	result = static_cast<char*>( ib_util_malloc( length( s ) ));
// #else
// 	static const int MAX_LENGTH = 255;
// 	static char buffer[ MAX_LENGTH + 1 ];
// 	result = buffer;
// 	
// 	if ( length( s ) > MAX_LENGTH )
// 		setlength( s, MAX_LENGTH );
// #endif
// 	
// 	p = result;
// 	q = s;
// 	
// 	while ( *q )
// 		*p++ = *q++;
// 
// 	// Copy the terminating 0
// 	*p = *q;
// 	
// 	return result;
// }

std::string BlobToString( BLOBCALLBACK blob )
{
	STATIC_MT std::string result;
#ifndef MT
	result.clear();
#endif
	
	if ( blob && blob->blob_handle )
	{
		int len = blob->blob_max_segment + 1L;
		ISC_UCHAR * buffer = static_cast<ISC_UCHAR*>( malloc( len ) );
		
		ISC_USHORT readLength = 0;

		while ( (*blob->blob_get_segment) ( blob->blob_handle, buffer, len, &readLength ) )
		{
			buffer[ readLength ] = 0;
			
			// I know, the double cast looks ugly; but it works!
			result += static_cast<char*>( static_cast<void*>( buffer ));
		}

		delete buffer;
	}

	return result;
}
