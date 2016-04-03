#include <dirent.h> // TODO: WINDOWS ?
#include <sys/stat.h> // TODO: WINDOWS ?
#include <unistd.h>

#include "utils/fileStorageReader.h"

#include "utils/logger.h"


FileStorageReader::FileStorageReader(std::string root) :
    _root(root)
{
    if (!TestIfPathExists("")) {
        warning("(FileStorageReader) : Path does not exists : " + root);
    }
}

bool FileStorageReader::TestIfPathExists(std::string rel_path)
{
    struct stat buffer;
    return stat((_root + rel_path).c_str(), &buffer) == 0;
}

std::vector<std::string> FileStorageReader::GetFilesList(std::string rel_path)
{
    std::vector<std::string> res;

    std::string path = _root + rel_path;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != nullptr)
    {
        // for each element in folder
        while ((ent = readdir (dir)) != nullptr)
        {
            // add to container
            std::string name(ent->d_name);
            if (name != "." && name != "..")  // skip . & ..
            {
                struct stat sb;
                if (ent->d_type == DT_DIR)
                    name += "/";
                res.push_back(name);
            }
        }
        closedir(dir);
    }
    else
    {
        error("(FileStorageReader::GetFileList) : Path does not exists");
    }

    return res;
}

DataProvider FileStorageReader::GetFileDataProvider(std::string fpath)
{
    std::string full_fpath = _root + fpath;

    if (TestIfPathExists(full_fpath.c_str()))
    {
        return DataProvider(full_fpath);
    }
    else
    {
        error("(FileStorageReader::GetFile) : File does not exists.");
        return DataProvider(fpath); // TODO: remove this, throw exception
    }
}

bool FileStorageReader::IsFolder(std::string path) {
    struct stat stat_;
    int status = stat(path.c_str(), &stat_);

    if (status == 0)
    {
        return bool(stat_.st_mode & S_IFDIR);
    }

    return false;
}

bool FileStorageReader::IsFile(std::string path) {
    struct stat stat_;
    int status = stat(path.c_str(), &stat_);

    if (status == 0)
    {
        return bool(stat_.st_mode & S_IFREG);
    }

    return false;
}
