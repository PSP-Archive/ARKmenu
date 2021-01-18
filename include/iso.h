#ifndef ISO_H
#define ISO_H

#include "entry.h"
#include "gfx.h"

#define ISO_MAGIC 0x30444301
#define PRIMARY_VD 1
#define SECTOR_SIZE 0x800

typedef struct
{
    char type;
    char id[5];
    char version;
} volumeDescriptor;

typedef struct
{
    unsigned pathTableSize;
    unsigned pathTableSizeLE;
    unsigned pathTableOffset;
    unsigned pathTableOptionalOffset;
} primaryVolumeDescriptor;

typedef struct
{
    char length;
    char extended;
    unsigned location;
    short parent;
} pathTable; //8

typedef struct
{
    char length;
	char extended;
    unsigned location;
	unsigned locationLE;
	unsigned size;
	unsigned sizeLE;
	char trash[7];
	char flags;
	char other_size;
	char intervale;
	unsigned vol_seq;
    char nameLen;
} dirRecord;

using namespace std;

class Iso : public Entry{

	private:
		
		Image* loadIcon();
		FILE * file;
		int open(const char * path);
		unsigned findFile(const char * name);
		unsigned identifyEntry(const char * entry);
		void close();

	public:
		Iso(string path);
		~Iso();
		
		static bool isPatched(string path);
		static bool isISO(const char* filepath);
		
		void execute();
		
		Image* getPic0();
		Image* getPic1();

};

#endif
