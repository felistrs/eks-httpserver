#ifndef UTILS__FILE_STORAGE_READER_H
#define UTILS__FILE_STORAGE_READER_H

#include <memory>
#include <string>
#include <vector>

#include "utils/dataProvider.h"


namespace srv {


class FileStorageReader
{
public:
    FileStorageReader(std::string root);

    bool Exists(std::string fpath);

    std::vector<std::string> GetFilesList(std::string path);
    std::shared_ptr<DataProvider> GetFile(std::string fpath);

private:
    std::string _root;
};


}


#endif // UTILS__FILE_STORAGE_READER_H
