//
// StompSocket.cpp
//
// (c) Elias Karakoulakis: derived from DialogSocket.cpp, 
//  a PoCo socket to be used for STOMP 
// the main difference is the use of LF instead of CR/LF for line termination
// and the use of NULL (\0) to terminate each frame
//
// Library: Net
// Package: Sockets
// Module:  StompSocket
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "StompSocket.h"
#include "Ascii.h"
#include <cstring>


namespace Poco {
namespace Net {


StompSocket::StompSocket():
	_pBuffer(0),
	_pNext(0),
	_pEnd(0)
{
	allocBuffer();
}


StompSocket::StompSocket(const SocketAddress& address): 
	StreamSocket(address),
	_pBuffer(0),
	_pNext(0),
	_pEnd(0)
{
	allocBuffer();
}


StompSocket::StompSocket(const Socket& socket): 
	StreamSocket(socket),
	_pBuffer(0),
	_pNext(0),
	_pEnd(0)
{
	allocBuffer();
}


StompSocket::~StompSocket()
{
	delete [] _pBuffer;
}


StompSocket& StompSocket::operator = (const Socket& socket)
{
	StreamSocket::operator = (socket);
	_pNext   = _pBuffer;
	_pEnd    = _pBuffer;
	return *this;
}


StompSocket& StompSocket::operator = (const StompSocket& socket)
{
	StreamSocket::operator = (socket);
	_pNext   = _pBuffer;
	_pEnd    = _pBuffer;
	return *this;
}


void StompSocket::sendByte(unsigned char ch)
{
	sendBytes(&ch, 1);
}


void StompSocket::sendString(const char* str)
{
	sendBytes(str, (int) std::strlen(str));
}


void StompSocket::sendString(const std::string& str)
{
	sendBytes(str.data(), (int) str.length());
}


void StompSocket::sendMessage(const std::string& message)
{
	std::string line;
	line.reserve(message.length() + 1);
	line.append(message);
	line.append("\n");
	sendString(line);
}


void StompSocket::sendMessage(const std::string& message, const std::string& arg)
{
	std::string line;
	line.reserve(message.length() + arg.length() + 2);
	line.append(message);
	if (!arg.empty())
	{
		line.append(" ");
		line.append(arg);
	}
	line.append("\n");
	sendString(line);
}


void StompSocket::sendMessage(const std::string& message, const std::string& arg1, const std::string& arg2)
{
	std::string line;
	line.reserve(message.length() + arg1.length() +arg2.length() + 3);
	line.append(message);
	line.append(" ");
	line.append(arg1);
	if (!arg2.empty())
	{
		line.append(" ");
		line.append(arg2);
	}
	line.append("\n");
	sendString(line);
}


bool StompSocket::receiveMessage(std::string& message, char terminator)
{
	message.clear();
	return receiveLine(message, terminator);
}


int StompSocket::get()
{
	refill();
    //std::cout << "get()==" << *_pNext  << "(" << ((int) *_pNext) << ")" << std::endl;
	if (_pNext != _pEnd)
		return std::char_traits<char>::to_int_type(*_pNext++);
	else
		return EOF_CHAR;
}


int StompSocket::peek()
{
	refill();
	if (_pNext != _pEnd)
		return std::char_traits<char>::to_int_type(*_pNext);
	else
		return EOF_CHAR;
}


void StompSocket::refill()
{
	if (_pNext == _pEnd)
	{
		int n = receiveBytes(_pBuffer, RECEIVE_BUFFER_SIZE);
		_pNext = _pBuffer;
		_pEnd  = _pBuffer + n;
        //~ std::cout << "refill() read "<< n <<" bytes:'" << std::string(_pBuffer) << "'" << std::endl;
	}
}


void StompSocket::allocBuffer()
{
	_pBuffer = new char [RECEIVE_BUFFER_SIZE];
	_pNext   = _pBuffer;
	_pEnd    = _pBuffer;
}


bool StompSocket::receiveLine(std::string& line, char terminator)
{
	// An old wisdom goes: be strict in what you emit
	// and generous in what you accept.
	int ch = get();
	while (ch != EOF_CHAR && ch != terminator)
	{
		line += (char) ch;
		ch = get();
	}

	if (ch == EOF_CHAR)
		return false;
	return true;
}


int StompSocket::receiveRawBytes(void* buffer, int length)
{
	refill();
	int n = static_cast<int>(_pEnd - _pNext);
	if (n > length) n = length;
	std::memcpy(buffer, _pNext, n);
	_pNext += n;
	return n;
}

bool StompSocket::incoming_data_waiting()
{
    if (_pNext == _pEnd) {
        //_pBuffer doesn't have any new data 
       // return(poll(Poco::Timespan(), Poco::Net::Socket::SELECT_READ));
       return(available()>0);
    } else 
        return(true);	
}
        
} } // namespace Poco::Net