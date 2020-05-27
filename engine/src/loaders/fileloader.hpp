#pragma once
#include <pch.hpp>
#include <fstream>

namespace fs 
{
    static byte* readFile(const char* filename)
    {
        // Open the file
        FILE* fp = fopen(filename, "rb");
        // Move the file pointer to the end of the file and determing the length
        fseek(fp, 0, SEEK_END);
        long file_length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        byte* contents = new byte[file_length + 1]{ 0 };
        // Here's the actual read
        fread(contents, 1, file_length + 1, fp);
        fclose(fp);
        return contents;
    }
}