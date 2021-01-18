/* Thanks to BOOSTER for his CSO converter sample */

#ifndef CSO_H
#define CSO_H

#include "iso.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <pspdebug.h>
#include <string.h>

#define CSO_MAGIC 0x4F534943

typedef struct 
{
	unsigned magic;
	unsigned header_size;
	unsigned long long file_size;
	unsigned block_size;
	unsigned char version;
	unsigned char align;
	char reserved[2];
} cso_header;

class Cso : public Entry{

	private:
		
		Image* loadIcon();

		unsigned identifyEntry(const char * name, unsigned block, unsigned * fileSize);
	
		FILE * file;
		unsigned * indices;
		unsigned char * data, * read_buffer;
		unsigned total_blocks, indices_len, current_index, current_index2;
		cso_header file_header, head;
		primaryVolumeDescriptor pvd;
		z_stream dec;

		bool open(const char * path);
		int getPrimaryVolumeDescriptor();
		unsigned findFile(const char * file, unsigned * fileSize);
		unsigned char * getDataBlock(unsigned block);
		void extractFile(const char * name, unsigned block, unsigned size);
		void clear();

	public:
		Cso(string path);
		~Cso();
		
		static bool isPatched(string path);
		static bool isCSO(const char* filepath);
		
		void execute();
		
		Image* getPic0();
		Image* getPic1();

};

#endif
