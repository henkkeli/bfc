#include "file.h"
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <string>
#include <boost/filesystem.hpp>

using std::string;
namespace fs = boost::filesystem;

string File::tempFile(string suffix)
{
    string pathTemplate = "/tmp/bfcXXXXXX" + suffix;
    char fname[PATH_MAX];

    if (pathTemplate.length() >= PATH_MAX)
    {
        return string();
    }

    strncpy(fname, pathTemplate.c_str(), PATH_MAX);
    int fd = mkstemps(fname, 2);

    if (fd == -1)
    {
        return string();
    }

    close(fd);

    return string(fname);
}

void File::setFileNames(std::string &asmFname, std::string &objFname,
                        std::string &binFname, std::shared_ptr<Options> opts)
{
    string inputFname = fs::basename(opts->inputFname());
    string outputFname = opts->outputFname();
    bool dotbf = inputFname.length() > 3 && inputFname.substr(inputFname.length() - 3) == ".bf";

    if (!opts->assemble())
    {
        if (!outputFname.empty())
        {
            asmFname = outputFname;
        }
        else
        {
            asmFname = inputFname;
            if (dotbf)
                asmFname.replace(asmFname.length() - 2, 2, "s");
            else
                asmFname += ".s";
        }
    }
    else if (!opts->link())
    {
        if (!outputFname.empty())
        {
            objFname = outputFname;
        }
        else
        {
            objFname = inputFname;
            if (dotbf)
                objFname.replace(objFname.length() - 2, 2, "o");
            else
                objFname += ".o";
        }
    }
    else
    {
        if (!outputFname.empty())
        {
            binFname = outputFname;
        }
        else
        {
            binFname = "a.out";
        }
    }
}
