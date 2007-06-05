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

#include "smtp.h"
#include "smtp_config.h"

#include <openssl/ssl.h>
#include <auth-client.h>
#include <libesmtp.h>

int authinteract( auth_client_request_t request, char **result, int fields, void *arg );
void print_recipient_status( smtp_recipient_t recipient, const char *mailbox, void *arg );
int tlsinteract( char *buf, int buflen, int rwflag, void *arg );
void event_cb( smtp_session_t session, int event_no, void *arg,...);
int handle_invalid_peer_certificate( long vfy_result );

string sendMail ( string to, string subject, string content_type, string messageBody )
{
	string result;
	
	SMTP_Config * config = SMTP_Config::config();

	smtp_session_t session;
	smtp_message_t message;
	auth_context_t authctx;
	const smtp_status_t *status;
	const char *c;

	auth_client_init ();
	session = smtp_create_session ();
	message = smtp_add_message ( session );

	if ( config->tls() )
	{
		smtp_starttls_enable ( session, Starttls_ENABLED );
		smtp_starttls_enable ( session, Starttls_REQUIRED );
	}

	smtp_set_server ( session, config->host() );

	/* Do what's needed at application level to use authentication
	*/
	authctx = auth_create_context ();
	auth_set_mechanism_flags ( authctx, AUTH_PLUGIN_PLAIN, 0 );
	auth_set_interact_cb ( authctx, authinteract, NULL );

	/* Use our callback for X.509 certificate passwords.  If STARTTLS is
	not in use or disabled in configure, the following is harmless. */
	smtp_starttls_set_password_cb ( tlsinteract, NULL );
	smtp_set_eventcb ( session, event_cb, NULL );

	/* Now tell libESMTP it can use the SMTP AUTH extension.
	*/
	if ( config->auth() )
		smtp_auth_set_context ( session, authctx );

	/* Set the reverse path for the mail envelope.  (NULL is ok)
	*/
	smtp_set_reverse_path ( message, config->from() );

	/* RFC 2822 doesn't require recipient headers but a To: header would
	be nice to have if not present. */
	c = to;
	smtp_add_recipient( message, c );

	/* Set the Subject: header.  For no reason, we want the supplied subject
	to override any subject line in the message headers. */
	if ( ! isempty( subject ))
	{
		c = subject;
		smtp_set_header ( message, "Subject", c );
		smtp_set_header_option ( message, "Subject", Hdr_OVERRIDE, 1 );
	}
	
	/* Set the special content-type header used for iCalendar messages
	*/
	if ( ! isempty( content_type ))
	{
		c = content_type;
		smtp_set_header ( message, "Content-Type", c );
		smtp_set_header_option ( message, "Content-Type", Hdr_OVERRIDE, 1 );
	}
	
	/* Set the message itself
	*/
	c = messageBody;
	smtp_set_message_str( message, const_cast<char*>( c ) );

	/* Initiate a connection to the SMTP server and transfer the
	message. */
	if ( ! smtp_start_session ( session ) )
	{
		char buf[128];
		
		result += string( "SMTP server problem " + string( smtp_strerror (smtp_errno (), buf, sizeof buf )) );
		result += "\n";
	}
	else
	{
		/* Report on the success or otherwise of the mail transfer.
		*/
		status = smtp_message_transfer_status ( message );
		
		result += intToString( status->code ) + " ";
		result += ( status->text != NULL ) ? status->text : "\n";
		result += smtp_enumerate_recipients ( message, print_recipient_status, NULL );
		result += "\n";
	}

	/* Free resources consumed by the program.
	*/
	smtp_destroy_session ( session );
	auth_destroy_context ( authctx );
	auth_client_exit ();
	
	return result;
}

/* Callback to pass user/password info.  Not thread safe. */
int authinteract( auth_client_request_t request, char **result, int fields, void *arg )
{
	static const string userPrompt = "user name";
	static const string passwordPrompt = "password";
	
	SMTP_Config * config = SMTP_Config::config();
	
	int i;
	string prompt;
	const char * c;
	
	for (i = 0; i < fields; i++)
	{
		prompt = lowercase( request[i].prompt );
		
		if ( prompt == userPrompt )
		{
			c = config->userName();
			result[i] = const_cast<char*>( c );
		}
		else if ( prompt == passwordPrompt )
		{
			c = config->password();
			result[i] = const_cast<char*>( c );
		}
	} 

	return 1;
}

/* Callback to prnt the recipient status */
void print_recipient_status ( smtp_recipient_t recipient, const char *mailbox, void *arg )
{
	const smtp_status_t *status;

	status = smtp_recipient_status ( recipient );
	printf ( "%s: %d %s", mailbox, status->code, status->text );
}

void event_cb ( smtp_session_t session, int event_no, void *arg,... )
{
	va_list alist;
	int *ok;

	va_start ( alist, arg );
	switch ( event_no )
	{
		case SMTP_EV_CONNECT:
		case SMTP_EV_MAILSTATUS:
		case SMTP_EV_RCPTSTATUS:
		case SMTP_EV_MESSAGEDATA:
		case SMTP_EV_MESSAGESENT:
		case SMTP_EV_DISCONNECT: break;
		case SMTP_EV_WEAK_CIPHER:
		{
			int bits;
			bits = va_arg ( alist, long ); ok = va_arg ( alist, int* );
			printf ( "SMTP_EV_WEAK_CIPHER, bits=%d - accepted.\n", bits );
			*ok = 1; 
			break;
		}
		case SMTP_EV_STARTTLS_OK:
			puts ( "SMTP_EV_STARTTLS_OK - TLS started here." ); 
			break;
		case SMTP_EV_INVALID_PEER_CERTIFICATE:
		{
			long vfy_result;
			vfy_result = va_arg ( alist, long ); ok = va_arg ( alist, int* );
			*ok = handle_invalid_peer_certificate ( vfy_result );
			break;
		}
		case SMTP_EV_NO_PEER_CERTIFICATE:
		{
			ok = va_arg ( alist, int* );
			puts ( "SMTP_EV_NO_PEER_CERTIFICATE - accepted." );
			*ok = 1; break;
		}
		case SMTP_EV_WRONG_PEER_CERTIFICATE:
		{
			ok = va_arg ( alist, int* );
			puts ( "SMTP_EV_WRONG_PEER_CERTIFICATE - accepted." );
			*ok = 1; break;
		}
		case SMTP_EV_NO_CLIENT_CERTIFICATE:
		{
			ok = va_arg ( alist, int* );
			puts ( "SMTP_EV_NO_CLIENT_CERTIFICATE - accepted." );
			*ok = 1; break;
		}
		default:
			printf ( "Got event: %d - ignored.\n", event_no );
	}
	va_end ( alist );
}

int handle_invalid_peer_certificate ( long vfy_result )
{
	const char *k ="rare error";
	switch ( vfy_result )
	{
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
			k="X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT"; break;
		case X509_V_ERR_UNABLE_TO_GET_CRL:
			k="X509_V_ERR_UNABLE_TO_GET_CRL"; break;
		case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
			k="X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE"; break;
		case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE:
			k="X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE"; break;
		case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
			k="X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY"; break;
		case X509_V_ERR_CERT_SIGNATURE_FAILURE:
			k="X509_V_ERR_CERT_SIGNATURE_FAILURE"; break;
		case X509_V_ERR_CRL_SIGNATURE_FAILURE:
			k="X509_V_ERR_CRL_SIGNATURE_FAILURE"; break;
		case X509_V_ERR_CERT_NOT_YET_VALID:
			k="X509_V_ERR_CERT_NOT_YET_VALID"; break;
		case X509_V_ERR_CERT_HAS_EXPIRED:
			k="X509_V_ERR_CERT_HAS_EXPIRED"; break;
		case X509_V_ERR_CRL_NOT_YET_VALID:
			k="X509_V_ERR_CRL_NOT_YET_VALID"; break;
		case X509_V_ERR_CRL_HAS_EXPIRED:
			k="X509_V_ERR_CRL_HAS_EXPIRED"; break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
			k="X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD"; break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
			k="X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD"; break;
		case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD:
			k="X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD"; break;
		case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD:
			k="X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD"; break;
		case X509_V_ERR_OUT_OF_MEM:
			k="X509_V_ERR_OUT_OF_MEM"; break;
		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
			k="X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT"; break;
		case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
			k="X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN"; break;
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
			k="X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY"; break;
		case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
			k="X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE"; break;
		case X509_V_ERR_CERT_CHAIN_TOO_LONG:
			k="X509_V_ERR_CERT_CHAIN_TOO_LONG"; break;
		case X509_V_ERR_CERT_REVOKED:
			k="X509_V_ERR_CERT_REVOKED"; break;
		case X509_V_ERR_INVALID_CA:
			k="X509_V_ERR_INVALID_CA"; break;
		case X509_V_ERR_PATH_LENGTH_EXCEEDED:
			k="X509_V_ERR_PATH_LENGTH_EXCEEDED"; break;
		case X509_V_ERR_INVALID_PURPOSE:
			k="X509_V_ERR_INVALID_PURPOSE"; break;
		case X509_V_ERR_CERT_UNTRUSTED:
			k="X509_V_ERR_CERT_UNTRUSTED"; break;
		case X509_V_ERR_CERT_REJECTED:
			k="X509_V_ERR_CERT_REJECTED"; break;
	}
	printf ( "SMTP_EV_INVALID_PEER_CERTIFICATE: %ld: %s\n", vfy_result, k );
	return 1; /* Accept the problem */
}


int tlsinteract ( char *buf, int buflen, int rwflag, void *arg )
{
	const char *pw;
	int len;

	pw = SMTP_Config::config()->certificatePassword();
	
	len = strlen ( pw );
	if ( len + 1 > buflen )
		return 0;
	strcpy ( buf, pw );
	return len;
}
