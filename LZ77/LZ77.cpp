// Vuong-DCP Codec, copyright 2018 by Samuel Wolf
//
//This program is free software : you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "LZ77.h"


// search the lookahed strign in buffer
bool searchInBuffer(const unsigned char * lookahead, unsigned short  lookaheadlength, const unsigned char * buffer,
	unsigned short  bufferlength, unsigned short * offset, unsigned short * length, unsigned short * additional_char) {

	const unsigned char * const bufferEnding = buffer + bufferlength;
	const unsigned char * const lookaheadEnding = lookahead + lookaheadlength -1;
	const unsigned char *  count = buffer;
	*length = 0;
	do {

		if (*buffer == *lookahead)
		{
			*offset = (buffer - count);

			do {
				length[0]++;;
				buffer++;
				lookahead++;

			} while (*buffer == *lookahead && lookahead < lookaheadEnding && buffer < bufferEnding);

			*additional_char = *lookahead;

			return true;

		}

		buffer++;
	} while (buffer < bufferEnding);
	*additional_char = *lookahead;
	return false;

}


// Finds the the longest Matching string from Lookaher in Buffer
bool findBestMatch(const unsigned char * lookahead, unsigned short  lookaheadlength, const unsigned char * buffer,
	unsigned short  bufferlength, unsigned short * offset, unsigned short * length, unsigned short * additional_char) {
	unsigned short best_offset = 0;
	unsigned short best_length = 0;
	unsigned char best_additional = 0;

	const unsigned char * const bufferEnding = buffer + bufferlength;
	const unsigned char * const lookaheadEnding = lookahead + lookaheadlength;
	const unsigned char *  count = buffer;

	do {
		*length = 0;
		bool found = searchInBuffer(lookahead, lookaheadlength, buffer, bufferEnding - buffer, offset, length, additional_char);


		if (found) {
			if (best_length < *length)
			{
				best_length = *length;
				best_additional = *additional_char;
				buffer += *offset;
				best_offset = buffer - count;
			}
			buffer += *offset;

		}
		else
		{
			if (best_length) {
				*offset = best_offset;
				*length = best_length;
				*additional_char = best_additional;
				return true;
			}
			else {
				best_additional = *lookahead;
				return false;
			}

		}

		buffer++;

	} while (buffer < bufferEnding);

	if (best_length) {
		*offset = best_offset;
		*length = best_length;
		*additional_char = best_additional;
		return true;
	}
	else {
		best_additional = *lookahead;
		return false;
	}
}


DllExport unsigned int Encode(const unsigned char * __restrict in, unsigned char * __restrict out, const unsigned int length)
{	// set ending an begining Pointers
	const unsigned char * const ending = in + length;
	unsigned char * const count = out;

	// Set buffer sizes
	unsigned short max_buffer_size = 200;
	unsigned short max_lookahead_size = 100;
	*(unsigned short*)out = max_buffer_size;
	out+=2;

	// init variables
	unsigned short buffer_size = 1;
	const unsigned char *  buffer = in;
	unsigned short lookahead_size = max_lookahead_size;
	const unsigned char *  lookahead = in;

	unsigned short offset = 0;
	unsigned short Llength = 0;
	unsigned short additional = 0;

	// write first output
	out[0] = 0;
	out[1] = *in;
	lookahead++;
	out += 2;

	// Encode 
	do {
		bool found = findBestMatch(lookahead, lookahead_size, buffer, buffer_size, &offset, &Llength, &additional);
		// write Output
		if (found)
		{
			out[0] = Llength;
			out[1] = offset;
			out[2] = additional;
			lookahead += Llength;
			out += 3;
		}
		else
		{
			out[0] = 0;
			out[1] = additional;
			out += 2;
		}

		lookahead++;

		// Adjust buffer and lookahead pionter
		int dif = lookahead - buffer;
		if (dif < max_buffer_size) {
			buffer = in;
			buffer_size = lookahead - buffer;
		}
		else if (dif > max_buffer_size) {
			buffer = lookahead - max_buffer_size;
			buffer_size = max_buffer_size;
		}
		dif = ending - lookahead;
		if (dif < max_lookahead_size) {
			lookahead_size = dif;
		}
		if (dif <= 100)
			dif = dif;
	} while (lookahead < ending);


	return (unsigned int)(out - count);
}

DllExport void Decode(const unsigned char * __restrict in, unsigned char * __restrict out, const unsigned int length)
{
	//Set Begin and End Pionter
	const unsigned char * const ending = out + length;
	const unsigned char * buffer = out;
	const unsigned char * start = out;

	// Get Buffer Size
	unsigned short max_buffer_size = *(unsigned short*)in;
	in+=2;

	// init variables
	unsigned short offset = 0;
	unsigned short Llength = 0;
	unsigned char additional = 0;

	do {
		// read Lenght value
		unsigned char in_val = *in;
		in++;

		
		if (in_val != 0)
		{
			
			Llength = in_val;
			// read Offset Value
			offset = *in;
			in++;

			// Copy From buffer
			for (unsigned int i = 0; i < Llength; i++)
			{
				*out = buffer[offset + i];
				out++;
			}

			*out = *in;
			in++;
			out++;

		}
		// pos and lenght is 0
		else
		{
			*out = *in;
			out++;
			in++;
		}

		// Adjust buffer and lookahead pionter
		int dif = out - buffer;
		if (dif < max_buffer_size) {
			buffer = start;
		}
		else if (dif > max_buffer_size) {
			buffer = out - max_buffer_size;
		}


	} while (out < ending);
}
