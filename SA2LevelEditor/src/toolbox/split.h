#ifndef SPLIT_H
#define SPLIT_H

/* Parse a line and return an array of the
individual tokens. Original line is modified. 
Make sure to call free() on the returned char** */
char** split(char* line, char delim, int* length);

//given a string, split the string into a vector of strings
// based on some delimiter character. the string canont be more 
// than 1023 characters long.
std::vector<std::string> split(std::string line, char delim);

#endif
