#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "File.h"

int get_file_size(FILE * fp)
{
    struct stat st;
    fstat(fileno(fp), &st);
    return st.st_size;
}

void file_read(char* buf, FILE* fin, const int fileSize)
{

    if(fread(buf, sizeof(char), fileSize, fin) != fileSize)
    {
        if(feof(fin))
            printf("Premature end of file\n");

        else
            printf("File read error\n");
    }
}

char* get_file_content(const char* nameFile)
{
    FILE* fp = fopen(nameFile, "rb");
    assert(fp != nullptr);

    int fileSize = get_file_size(fp);

    char* buf = (char *)calloc(fileSize, sizeof(char));
    assert(buf != NULL);

    file_read(buf, fp, fileSize);
    
    fclose(fp);
    return buf;
}

int getCountLine(char* buf, const int fileSize)
{
    int nLine = 1;

    for(int i = 0; i < fileSize; i++)
    {
        if(buf[i] == '\n')
            nLine++;
    }

    return nLine;
}


char** write_array_pointers(char* buf, const int fileSize, const int nLine)
{
    char** text = (char **)calloc(nLine, sizeof(char*));
    assert(text != NULL);

    int line = 1;

    text[0] = buf;

    for(int i = 0; i < fileSize; i++)
    {
        if(buf[i] == '\n')
        {
            buf[i] = '\0';
            text[line++] = &buf[i+1];
        }
    }

    return text;
}

void outputFile(char* ofileName, char** text, char* buf, const int nLine)
{
    FILE * fileOutput = fopen(ofileName, "wb");
    assert(fileOutput  != NULL);

    for(int i = 0; i < nLine; i++)
    {
        if((int)text[i][0] > '\r')
            fputs(text[i], fileOutput);
    }

    fclose(fileOutput);

    free(buf);
    free(text);
}
