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

#include "vmime/streamContentHandler.hpp"


namespace vmime
{


streamContentHandler::streamContentHandler()
	: m_encoding(NO_ENCODING), m_stream(null)
{
}


streamContentHandler::streamContentHandler(ref <utility::inputStream> is,
	const utility::stream::size_type length, const vmime::encoding& enc)
{
	setData(is, length, enc);
}


streamContentHandler::~streamContentHandler()
{
}


streamContentHandler::streamContentHandler(const streamContentHandler& cts)
	: contentHandler(), m_encoding(cts.m_encoding),
	  m_stream(cts.m_stream), m_length(cts.m_length)
{
}


ref <contentHandler> streamContentHandler::clone() const
{
	return vmime::create <streamContentHandler>(*this);
}


streamContentHandler& streamContentHandler::operator=(const streamContentHandler& cts)
{
	m_encoding = cts.m_encoding;

	m_stream = cts.m_stream;
	m_length = cts.m_length;

	return (*this);
}


void streamContentHandler::setData(ref <utility::inputStream> is,
	const utility::stream::size_type length, const vmime::encoding& enc)
{
	m_encoding = enc;
	m_length = length;
	m_stream = is;
}


void streamContentHandler::generate(utility::outputStream& os, const vmime::encoding& enc,
	const string::size_type maxLineLength) const
{
	if (!m_stream)
		return;

	// Managed data is already encoded
	if (isEncoded())
	{
		// The data is already encoded but the encoding specified for
		// the generation is different from the current one. We need
		// to re-encode data: decode from input buffer to temporary
		// buffer, and then re-encode to output stream...
		if (m_encoding != enc)
		{
			ref <encoder> theDecoder = m_encoding.getEncoder();
			ref <encoder> theEncoder = enc.getEncoder();

			theEncoder->getProperties()["maxlinelength"] = maxLineLength;

			m_stream->reset();  // may not work...

			std::ostringstream oss;
			utility::outputStreamAdapter tempOut(oss);

			theDecoder->decode(*m_stream, tempOut);

			string str = oss.str();
			utility::inputStreamStringAdapter tempIn(str);

			theEncoder->encode(tempIn, os);
		}
		// No encoding to perform
		else
		{
			m_stream->reset();  // may not work...

			utility::bufferedStreamCopy(*m_stream, os);
		}
	}
	// Need to encode data before
	else
	{
		ref <encoder> theEncoder = enc.getEncoder();
		theEncoder->getProperties()["maxlinelength"] = maxLineLength;

		m_stream->reset();  // may not work...

		theEncoder->encode(*m_stream, os);
	}
}


void streamContentHandler::extract(utility::outputStream& os,
	utility::progressListener* progress) const
{
	if (!m_stream)
		return;

	// No decoding to perform
	if (!isEncoded())
	{
		m_stream->reset();  // may not work...

		if (progress)
			utility::bufferedStreamCopy(*m_stream, os, getLength(), progress);
		else
			utility::bufferedStreamCopy(*m_stream, os);
	}
	// Need to decode data
	else
	{
		ref <encoder> theDecoder = m_encoding.getEncoder();

		m_stream->reset();  // may not work...

		utility::progressListenerSizeAdapter plsa(progress, getLength());

		theDecoder->decode(*m_stream, os, &plsa);
	}
}


void streamContentHandler::extractRaw(utility::outputStream& os,
	utility::progressListener* progress) const
{
	if (!m_stream)
		return;

	m_stream->reset();  // may not work...

	if (progress)
		utility::bufferedStreamCopy(*m_stream, os, getLength(), progress);
	else
		utility::bufferedStreamCopy(*m_stream, os);
}


const string::size_type streamContentHandler::getLength() const
{
	return (m_length);
}


const bool streamContentHandler::isEmpty() const
{
	return (m_length == 0 || !m_stream);
}


const bool streamContentHandler::isEncoded() const
{
	return (m_encoding != NO_ENCODING);
}


const vmime::encoding& streamContentHandler::getEncoding() const
{
	return (m_encoding);
}


} // vmime
