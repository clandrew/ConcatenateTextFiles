#include "pch.h"

void PrintUsage()
{
    printf("Usage: ConcatenateTextFiles.exe <dir> <destFile>\n");
}

int main(int argc, void** argv)
{
    if (argc != 3)
    {
        PrintUsage();
        return -1;
    }

    char* dirArg = reinterpret_cast<char*>(argv[1]);
    char* destFileArg = reinterpret_cast<char*>(argv[2]);

    std::string dirStringNarrow(dirArg);
    std::string destFileStringNarrow(destFileArg);

    std::wstring dirName(dirStringNarrow.begin(), dirStringNarrow.end());
    std::wstring destFileName(destFileStringNarrow.begin(), destFileStringNarrow.end());

    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    FILE* output;

    _wfopen_s(&output, destFileName.c_str(), L"wb");

    if (!output)
    {
        printf("The output file could not be opened for writing.\n");
        return -1;
    }

    std::wstring searchKey = dirName;
    searchKey.append(L"\\*");

    hFind = FindFirstFile(searchKey.c_str(), &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("The input directory could not be opened.\n");
        return -1;
    }

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Skip nested folders, and links like '.' and '..'
        }
        else
        {
            wprintf(L"Adding %s.\n", ffd.cFileName);

            // Open the file and append it
            std::wstring inputPath = dirName;
            inputPath.append(L"\\");
            inputPath.append(ffd.cFileName);

            FILE* input{};
            _wfopen_s(&input, inputPath.c_str(), L"rb");

            if (input)
            {
                fseek(input, 0, SEEK_END);
                long fileSize = ftell(input);
                fseek(input, 0, SEEK_SET);

                std::vector<unsigned char> vec;
                vec.resize(fileSize);
                fread(vec.data(), 1, fileSize, input);
                fwrite(vec.data(), 1, fileSize, output);
                fclose(input);
            }
            else
            {
                wprintf(L"The input file %s could not be opened. Skipping.\n", inputPath.c_str());
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
    fclose(output);

    wprintf(L"Finished writing %s.\n", destFileName.c_str());
    return 0;
}