/*
 *   C3Mapper - create minimaps from Caesar 3 scenarios and saved games
 *   Copyright (C) 2007  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
This class is a C++ port from the Dynamite library, written in C.
http://synce.sourceforge.net/synce/dynamite.php
Dynamite library licence:

Copyright (c) 2003 David Eriksson <twogood@users.sourceforge.net>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pkwareinputstream.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
* Dictionary for keeping track of past read bytes
*/
class PKDictionary {
	public:
		/**
		* Creates a new dictionary of size `size'
		*/
		PKDictionary(int size) {
			dictionary = new unsigned char[size];
			this->size = size;
			this->first = -1;
		}
		
		~PKDictionary() {
			delete[] dictionary;
		}
		
		/**
		* Returns the byte at the specified position.
		* Also does a PUT for this byte since the compression
		* algorithm requires it
		*/
		unsigned char get(int position) {
			int index = (size + first - position) % size;
			put(dictionary[index]);
			return dictionary[index];
		}
		
		/**
		* Adds a byte to the dictionary
		*/
		void put(unsigned char b) {
			first = (first + 1) % size;
			dictionary[first] = b;
		}
	
	private:
		int size;
		int first;
		unsigned char *dictionary;
};

PKWareInputStream::PKWareInputStream(string filename, int file_length) {
	ifstream *i = new ifstream();
	i->open(filename.c_str(), ios::in|ios::binary);
	if (!i->is_open()) {
		input = NULL;
		throw PKException("File not readable");
	}
	input = i;
	this->close_stream = true;
	this->file_length = file_length;
	init();
}

PKWareInputStream::PKWareInputStream(istream *i, bool close_stream, int file_length) {
	input = i;
	this->close_stream = close_stream;
	this->file_length = file_length;
	init();
}

PKWareInputStream::~PKWareInputStream() {
	if (close_stream) {
		delete input;
	}
}

unsigned char PKWareInputStream::read() {
	if (read_copying) {
		//cout << "Copying bytes from offset " << read_offset << "; " << read_length << " bytes left" << endl;
		read_length--;
		if (read_length <= 0) {
			read_copying = false;
		}
		unsigned char b = dictionary->get(read_offset);
		return b;
	} else {
		if (readBit() == 0) {
			// Copy byte verbatim
			int result = readBits(8);
			dictionary->put((unsigned char)result);
			//cout << "Reading verbatim byte: " << result << endl;
			return (unsigned char)result;
		}
		// Needs to copy stuff from the dictionary
		read_length = getCopyLength();
		if (read_length >= 519) {
			//cout << "519 read: end of file" << endl;
			throw PKException("EOF");
		}
		
		read_offset = getCopyOffset(read_length);
		//cout << "Copying " << read_length << " bytes from offset " << read_offset << endl;
		unsigned char b = dictionary->get(read_offset);
		read_length--;
		read_copying = true;
		return b;
	}
}

int PKWareInputStream::read(unsigned char *buf, int length) {
	int current = 0;
	try {
		int max = length;
		while (current < max) {
			if (read_copying) {
				while (current < max && read_copying) {
					read_length--;
					if (read_length <= 0) {
						read_copying = false;
					}
					buf[current++] = dictionary->get(read_offset);
				}
			} else if (readBit() == 0) {
				// Copy byte verbatim
				int result = readBits(8);
				dictionary->put((unsigned char)result);
				buf[current++] = (unsigned char)result;
			} else {
				// Needs to copy stuff from the dictionary
				read_length = getCopyLength();
				if (read_length >= 519) {
					return current;
				}
				
				read_offset = getCopyOffset(read_length);
				buf[current++] = dictionary->get(read_offset);
				read_length--;
				read_copying = true;
			}
		}
		return current;
	} catch (PKException e) {
		if (e.msg == "EOF") {
			//cout << "EOF thrown, current = " << current << endl;
			return current;
		}
		throw;
	}
}

unsigned char PKWareInputStream::readByte() {
	return read();
}

unsigned short PKWareInputStream::readShort() {
	unsigned char data[2];
	read(data, 2);
	return (unsigned short)(data[0] + (data[1] << 8));
}

unsigned int PKWareInputStream::readInt() {
	unsigned char data[4];
	unsigned int number = 0;
	
	read(data, 4);
	for (int i = 0; i < 4; i++) {
		number += (data[i] << (i*8));
	}
	return number;
}

/**
* Skips length bytes from the input
* @todo Make much more efficient
*/
void PKWareInputStream::skip(int length) {
	for (int i = 0; i < length; i++) {
		read();
	}
}

void PKWareInputStream::empty() {
	//int i = 0;
	try {
		bool success = true;
		while (success) {
			read();
			//i++;
		}
	} catch (PKException e) {
		if (e.msg == "EOF") {
			//cout << "Emptied. " << i << " bytes discarded" << endl;
			return;
		}
		throw;
	}
}

///////////////////////////
// Private class methods //
///////////////////////////

/**
* Initialises the stream
*/
void PKWareInputStream::init() {
	// First get the file length if it hasn't been given
	if (file_length == -1) {
		int current = input->tellg();
		input->seekg(0, ios::end);
		file_length = (int)input->tellg() - current;
		input->seekg(current, ios::beg);
		cout << "Discovered file length: " << file_length << endl;
	}
	if (file_length <= 2) {
		throw PKException("File too small");
	}
	eof_reached = false;
	readHeader();
	fillBuffer();
	// Init the remaining variables
	bufBit = 0;
	read_offset = 0;
	read_length = 0;
	read_copying = false;
}

/**
* Reads the 2-byte header and initialises the dictionary
*/
void PKWareInputStream::readHeader() {
	// Read the header to decide on the encoding type
	char c;
	input->read(&c, 1);
	if (c != 0) {
		throw PKException("Static dictionary not supported");
	}
	
	input->read(&c, 1);
	dictionary_bits = (int)c;
	switch (dictionary_bits) {
		case 4: dictSize = 1024; break;
		case 5: dictSize = 2048; break;
		case 6: dictSize = 4096; break;
		default:
			throw PKException("Unknown dictionary size");
	}
  dictionary.reset( new PKDictionary(dictSize) );
  buffer.resize( BUFFER_SIZE );
	//System.out.println("Dictionary size: "+dictSize);
	file_length -= 2; // Subtract two header bytes from total file length
}

/**
* Gets the amount of bytes to copy from the dictionary
*/
int PKWareInputStream::getCopyLength() {
	int bits;
	
	bits = readBits(2);
	if (bits == 3) { // 11
		return 3;
	} else if (bits == 1) { // 10x
		return 4 - 2 * readBit();
	} else if (bits == 2) { // 01
		if (readBit() == 1) { // 011
			return 5;
		} else { // 010x
			return 7 - readBit();
		}
	} else if (bits == 0) { // 00
		bits = readBits(2);
		if (bits == 3) { // 0011
			return 8;
		} else if (bits == 1) { // 0010
			if (readBit() == 1) { // 00101
				return 9;
			} else { // 00100x
				return 10 + readBit();
			}
		} else if (bits == 2) { // 0001
			if (readBit() == 1) { // 00011xx
				return 12 + readBits(2);
			} else { // 00010xxx
				return 16 + readBits(3);
			}
		} else if (bits == 0) { // 0000
			bits = readBits(2);
			if (bits == 3) { // 000011xxxx
				return 24 + readBits(4);
			} else if (bits == 1) { // 000010xxxxx
				return 40 + readBits(5);
			} else if (bits == 2) { // 000001xxxxxx
				return 72 + readBits(6);
			} else if (bits == 0) { // 000000
				if (readBit() == 1) { // 0000001xxxxxxx
					return 136 + readBits(7);
				} else { // 0000000xxxxxxxx
					return 264 + readBits(8);
				}
			}
		}
	}
	throw PKException("Invalid copy length");
}

/**
* Gets the offset at which to start copying bytes from the dictionary
*/
int PKWareInputStream::getCopyOffset(int length) {
	int lower_bits, result;
	if (length == 2) {
		lower_bits = 2;
	} else {
		lower_bits = dictionary_bits;
	}
	
	result = getCopyOffsetHigh() << lower_bits;
	result |= readBits(lower_bits);
	return result;
}

/**
* Gets the "high" value of the copy offset, the lower N bits
* are stored verbatim; N depends on the copy length and the
* dictionary size.
*/
int PKWareInputStream::getCopyOffsetHigh() {
	int bits;
	
	bits = readBits(2);
	if (bits == 3) { // 11
		return 0;
	} else if (bits == 1) { // 10
		bits = readBits(2);
		if (bits == 3) { // 1011
			return 0x1;
		} else if (bits == 1) { // 1010
			return 0x2;
		} else if (bits == 2) { // 1001x
			return 0x4 - readBit();
		} else if (bits == 0) { // 1000x
			return 0x6 - readBit();
		}
	} else if (bits == 2) { // 01
		bits = readBits(4);
		if (bits == 0) {
			return 0x17 - readBit();
		} else {
			bits = reverse(bits, 4);
			return 0x16 - bits;
		}
	} else if (bits == 0) { // 00
		bits = readBits(2);
		if (bits == 3) {
			bits = reverse(readBits(3), 3);
			return 0x1f - bits;
		} else if (bits == 1) {
			bits = reverse(readBits(3), 3);
			return 0x27 - bits;
		} else if (bits == 2) {
			bits = reverse(readBits(3), 3);
			return 0x2f - bits;
		} else {
			bits = reverse(readBits(4), 4);
			return 0x3f - bits;
		}
	}
	
	throw PKException("Invalid copy offset");
}

/**
* Reverse the bits in `number', essentially converting it from little
* endian to big endian or vice versa.
*/
int PKWareInputStream::reverse(int number, int length) {
	int result = 0;
	for (int i = 0; i < length; i++) {
		if (0 != (number & (1 << i))) {
			// Set bit in result
			result |= (1 << (length - 1 - i));
		}
	}
	return result;
}

/**
* Fill the internal buffer
*/
void PKWareInputStream::fillBuffer() {
	bufOffset = 0;
	if (file_length <= BUFFER_SIZE) {
    input->read(buffer.data(), file_length);
		eof_reached = true;
		eof_position = file_length;
	} else {
    input->read(buffer.data(), BUFFER_SIZE);
		file_length -= BUFFER_SIZE;
	}
}

/**
* Advances the data pointer one byte, filling the buffer
* if necessary
*/
void PKWareInputStream::advanceByte() {
	bufOffset++;
	if (eof_reached && bufOffset >= eof_position) {
		throw PKException("EOF (invalid)");
	}
	if (bufOffset >= BUFFER_SIZE) {
		fillBuffer();
	}
	bufBit = 0;
}

/**
* Reads one single bit
*/
unsigned char PKWareInputStream::readBit() {
	if (bufBit == 8) {
		advanceByte();
	}
	unsigned char b = (unsigned char)((buffer[bufOffset] & (1 << bufBit)) >> bufBit);
	bufBit++;
	return b;
}

/**
* Reads bits in little endian order
* @param length Number of bits to read. Should never be more than 8.
* @return int Value of the bits read
*/
int PKWareInputStream::readBits(int length) {
	int result;
	if (bufBit == 8) {
		advanceByte();
	}
	// Check to see if we span multiple bytes
	if (bufBit + length > 8) {
		// First take last remaining bits in this byte & put them in place
		result = ((buffer[bufOffset] & 0xff) >> bufBit);
		int length2 = length + bufBit - 8;
		int length1 = length - length2;
		advanceByte();
		
		// Read length2 bytes from the second byte & add them to the result
		result |= ((buffer[bufOffset]) & ((1 << length2) - 1)) << length1;
		bufBit = length2;
	} else {
		// Same byte, easy!
		result = buffer[bufOffset] >> bufBit;
		result &= (1 << length) - 1;
		bufBit += length;
	}
	return result;
}
