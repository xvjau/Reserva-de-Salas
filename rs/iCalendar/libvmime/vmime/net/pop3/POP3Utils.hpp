//
// VMime library (http://www.vmime.org)
// Copyright (C) 2002-2006 Vincent Richard <vincent@vincent-richard.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Linking this library statically or dynamically with other modules is making
// a combined work based on this library.  Thus, the terms and conditions of
// the GNU General Public License cover the whole combination.
//

#ifndef VMIME_NET_POP3_POP3UTILS_HPP_INCLUDED
#define VMIME_NET_POP3_POP3UTILS_HPP_INCLUDED


#include <map>

#include "vmime/config.hpp"
#include "vmime/types.hpp"


namespace vmime {
namespace net {
namespace pop3 {


class POP3Utils
{
public:

	/** Parse a response of type ([integer] [string] \n)*.
	  * This is used in LIST or UIDL commands:
	  *
	  *    C: UIDL
	  *    S: +OK
	  *    S: 1 whqtswO00WBw418f9t5JxYwZ
	  *    S: 2 QhdPYR:00WBw1Ph7x7
	  *    S: .
	  *
	  * @param response raw response string as returned by the server
	  * @return an associative array which map a message number to its
	  * data (either UID or size)
	  */
	static void parseMultiListOrUidlResponse
		(const string& response, std::map <int, string>& result);
};


} // pop3
} // net
} // vmime


#endif // VMIME_NET_POP3_POP3UTILS_HPP_INCLUDED

