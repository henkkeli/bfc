#ifndef FILE_H
#define FILE_H

#include "options.h"
#include <string>
#include <memory>

namespace File
{

std::string tempFile(std::string suffix);
void setFileNames(std::string &asmFname, std::string &objFname,
                  std::string &binFname, std::shared_ptr<Options> opts);
}

#endif // FILE_H
