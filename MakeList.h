#include <stdlib.h>
#include <stdio.h>

#define bool unsigned char
#define false 0
#define true 1


long FileSize(FILE * F)

{
	fseek(F, 0, SEEK_END);
	long result = ftell(F);
	fseek(F, 0, SEEK_SET);
	return result;
}

bool FileExists(char * FileName)
{
        FILE * F = fopen(FileName,"rb");
        if (!F) return false;
        fclose(F);
        return true;
}



typedef struct Header
{
        char filename[32];
        int tablesize;
        int datasize;
} Header;




	unsigned char * source;
	long sourceLen;
	unsigned char * target; //Packed data
	long targetLen;
	unsigned char * table;
	long tableLen;


void freesource()
{
    if (sourceLen) free(source);
    sourceLen = 0;
    source = NULL;
}
void freetarget()
{
    if (tableLen) free(table);
    table = NULL;
    tableLen = 0;
    if (targetLen) free(target);
    target = NULL;
    targetLen = 0;
}


	void Pack0()
	{
		freetarget();
		target = malloc(sourceLen);
		targetLen = sourceLen;
		memcpy(target,source,targetLen);
		tableLen = 0;
	}

    void Unpack0()
    {
		freesource();
		source = malloc(targetLen);
		sourceLen = targetLen;
		memcpy(source,target,sourceLen);
    }

