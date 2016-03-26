#ifndef UTILS__FILE_STORAGE_READER_H
#define UTILS__FILE_STORAGE_READER_H

#include <memory>
#include <string>
#include <vector>

#include "utils/dataProvider.h"


class FileStorageReader
{
public:
    FileStorageReader(std::string root);

    bool TestIfPathExists(std::string fpath);

    std::vector<std::string> GetFilesList(std::string path);
    DataProvider GetFileDataProvider(std::string fpath);

private:
    std::string _root;
};


#endif // UTILS__FILE_STORAGE_READER_H
