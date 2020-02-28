#include <stdio.h>
#include <stdlib.h>
#include "Huffman.h"
#include <stdlib.h>

void TryToCreate(int narg,char ** args, char * FileName);
void TryToList(char ** args, char * FileName);
void TryToExtract(char ** args, char * FileName);
bool CheckParameters(int narg,char ** args);

int main(int narg, char ** args)
{

    if (!CheckParameters(narg,args))
    {
        printf("usage:\nHuffman --file FILENAME --{create|extract|list} [file list]\n");
        printf("examples:\n");
        printf("Huffman --file arc.arc --create \n");
        printf("Huffman --file arc.arc --list\n");
        printf("Huffman --file arc.arc --extract\n");
        getchar();
        return 1;
    }

    TryToCreate(narg,args, args[2]);
    TryToList(args, args[2]);
    TryToExtract(args, args[2]);
    return 0;
}

bool CheckParameters(int narg,char ** args)
{
    if (narg<4) return false;

    if (strcmp(args[1],"--file") != 0) return false;


    if (strcmp(args[3],"--create")!=0
        &&
        strcmp(args[3],"--extract")!=0
        &&
        strcmp(args[3],"--list")!=0
        )
   return false;
   return true;
}


void TryToCreate(int narg,char ** args, char * FileName)
{
    Header H;
    if (strcmp(args[3],"--create")!=0) return;

      FILE * F = fopen(FileName,"wb+");
      printf("Pack to %s\n",FileName);
      for (int k=4; k<narg; k++)
      {
         memset(H.filename,0,sizeof(H.filename));
         strcpy(H.filename,args[k]);
         FILE * G = fopen(H.filename,"rb+");
         if (!G)
         {
             printf("File not found %s\n",H.filename);
             continue;
         }
         fseek(G,0,SEEK_END);
         int size = ftell(G);
         fseek(G,0,SEEK_SET);
         source = malloc(size);
         fread(source,1,size,G);
         fclose(G);

         sourceLen = size;
         Pack();
         if (sourceLen < targetLen+tableLen)
            Pack0();
         H.datasize = targetLen;
         H.tablesize = tableLen;
         fwrite(&H,sizeof(H),1,F);
         if (tableLen) fwrite(table,1,tableLen,F);
         fwrite(target,1,targetLen,F);
      }
      fclose(F);
      printf("Ok\n");
}

void TryToList(char ** args, char * FileName)
{
    Header H;
    if (strcmp(args[3],"--list")!=0) return;
    if (!FileExists(FileName))
    {
        printf("No file %s\n",FileName);
        return;
    }
    printf("List of %s\n",FileName);
    FILE * F = fopen(FileName,"rb");
    int Pos = 0;
    while (true)
    {
        fseek(F,Pos,SEEK_SET);
        fread(&H,sizeof(H),1,F);
        if (feof(F)) break;
        printf("%s\n",H.filename);
        Pos += sizeof(H)+H.tablesize+H.datasize;
    }
    fclose(F);
}

void TryToExtract(char ** args, char * FileName)
{
    Header H;
    if (strcmp(args[3],"--extract")!=0) return;
    if (!FileExists(FileName))
            {
                printf("No file %s\n",FileName);
                return;
            }
    printf("Extract of %s\n",FileName);
    FILE * F = fopen(FileName,"rb");
    int Pos = 0;
    while (true)
    {
        fseek(F,Pos,SEEK_SET);
        fread(&H,sizeof(H),1,F);
        if (feof(F)) break;
        Pos += sizeof(H)+H.tablesize+H.datasize;
        printf("extracting %s\n",H.filename);
        //target, targetLen
        target = malloc(H.datasize);
        targetLen = H.datasize;
        if (H.tablesize)
        {
            tableLen = H.tablesize;
            table = malloc(tableLen);
            fread(table,1,tableLen,F);
            fread(target,1,targetLen,F);
            Unpack();
        }
        else
        {
            fread(target,1,targetLen,F);
            Unpack0();
        }
        FILE * G = fopen(H.filename,"wb+");
        fwrite(source,1,sourceLen,G);
        fclose(G);
    }
    fclose(F);
    printf("Ok\n");
}


