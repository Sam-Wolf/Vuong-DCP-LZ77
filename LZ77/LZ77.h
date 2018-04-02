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

#include <Vfw.h>
#define DllExport   __declspec( dllexport )  

// Encode Function
extern "C" DllExport  unsigned int Encode(const unsigned char * __restrict in, unsigned char * __restrict out, const unsigned int length);

// Decode Funktion
extern "C" DllExport void Decode(const unsigned char * __restrict in, unsigned char * __restrict out, const unsigned int length);

// Compression Method ID
extern "C" DllExport unsigned long  getCompressID() {
	return MAKEFOURCC('L', 'Z', '7', '7');
};

// Compression Method Name 
extern "C"  DllExport const char * getCompressName() {
	return "LZ77(200/100)";
};

