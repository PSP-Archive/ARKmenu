#include "iso.h"

Iso::Iso(string path){
	this->file = NULL;
	this->path = path;
	size_t lastSlash = path.rfind("/", string::npos);
	this->name = path.substr(lastSlash+1, string::npos);
	this->icon0 = this->loadIcon();
}

Iso::~Iso(){
	if (this->icon0 != common::getNoIcon())
		delete this->icon0;
}

Image* Iso::loadIcon(){
	this->open(this->path.c_str());
	int icon_offset = this->findFile("PSP_GAME/ICON0.PNG");
	this->close();
	Image* icon = loadImage(this->path.c_str(), icon_offset);;
	return (icon == NULL)? common::getNoIcon() : icon;
}

Image* Iso::getPic0(){
	return NULL;
}

Image* Iso::getPic1(){
	return NULL;
}

int Iso :: open(const char * path)
{
	file = fopen(path, "rb");
	if(!file) return 0;
	unsigned magic = 0;
	fseek(file, 0x8000, SEEK_SET);
	fread(&magic, sizeof(unsigned), 1, file);
	if(magic!=ISO_MAGIC)
	{
		fclose(file);
		return 0;
	};
	fseek(file, 0, SEEK_SET);
	return 1;
};

void Iso :: close()
{
	if(file) fclose(file);
};

unsigned Iso :: findFile(const char * name)
{
	unsigned ret = 0, end = 0;
	char * dir_name = NULL;
	volumeDescriptor vd;
	primaryVolumeDescriptor pvd;
	pathTable pt;

    fseek(file, 0x8000, SEEK_SET);
    while(!end && !feof(file))
    {
        fread(&vd, sizeof(volumeDescriptor), 1, file);
        if(vd.type!=PRIMARY_VD)
        {
            fseek(file, SECTOR_SIZE - sizeof(volumeDescriptor), 1);
            continue;
        };

        fseek(file, 0x7D, SEEK_CUR); //Only load needed data from pvd
        fread(&pvd, sizeof(primaryVolumeDescriptor), 1, file);
        fseek(file, SECTOR_SIZE * pvd.pathTableOffset, SEEK_SET);

        while(!end && (ftell(file)-SECTOR_SIZE * pvd.pathTableOffset != pvd.pathTableSize))
        {
            fread(&pt, sizeof(short), 1, file);
            fread(&pt.location, sizeof(unsigned), 1, file);
            fseek(file, sizeof(short), SEEK_CUR);

            dir_name = (char *)malloc(pt.length+1);
            fread(dir_name, pt.length, 1, file);
            *(dir_name+pt.length) = '\0';

            if(pt.length % 2) fseek(file, 1, SEEK_CUR); //Alignment
            if(!strncmp(name, dir_name, (int)(strrchr(name, '/')-name)))
            {
                fseek(file, pt.location * SECTOR_SIZE, SEEK_SET);
                ret = this->identifyEntry(strrchr(name, '/')+1);
                end = 1;
            };
            free(dir_name);
        };
        end = 1;
    };
	return ret;
};

unsigned Iso :: identifyEntry(const char * entry)
{
    unsigned ret = 0;
    char * entry_name = NULL;
    dirRecord dr;

    fread(&dr.length, 1, 1, file);
    while(!ret && dr.length)
    {
        if(dr.length==0x30) fseek(file, 0x2F, SEEK_CUR);
        else
        {
            fseek(file, 0x01, SEEK_CUR);
            fread(&dr.location, sizeof(unsigned), 1, file);
            fseek(file, 0x1A, SEEK_CUR); //Tricky way to load needed data again
            fread(&dr.nameLen, sizeof(char), 1, file);

            entry_name = (char *)malloc(dr.nameLen+1);
            fread(entry_name, dr.nameLen, 1, file);
            *(entry_name+dr.nameLen)='\0';

            if(!strcmp(entry_name, entry)) //File was found!
                ret = dr.location * SECTOR_SIZE;

            free(entry_name);
            fseek(file, dr.length-dr.nameLen-0x21, SEEK_CUR);
        };
        fread(&dr.length, 1, 1, file);
    };
    return ret;
};

void Iso::execute(){
	this->executeISO();
}

bool Iso::isPatched(string path){
	Iso* iso = new Iso(path);
	iso->open(path.c_str());
	int offset = iso->findFile("SYSDIR/EBOOT.OLD");
	iso->close();
	return offset;
}

bool Iso::isISO(const char* filename){
	return (common::getMagic(filename, 0x8000) == ISO_MAGIC);
}
