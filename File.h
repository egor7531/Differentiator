#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

char* get_file_content(const char* nameFile);
int get_count_line(char * buf, const size_t fileSize);
char** write_array_pointers(char * buf, const size_t filesize, const size_t nLine);
void outputFile(char* ofileName, char** text, char* buf, const int nLine);

#endif // FILE_H_INCLUDED
