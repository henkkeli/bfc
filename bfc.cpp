#include "program.h"
#include "options.h"
#include "file.h"
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/process.hpp>

using std::cerr;
using std::endl;
using std::string;
namespace bp = boost::process;

int main(int argc, char *argv[])
{
    std::shared_ptr<Options> opts = std::make_shared<Options>();

    if (!opts->parse(argc, argv))
    {
        cerr << opts->err() << endl;
        return 1;
    }

    string asmFname;
    string objFname;
    string binFname;

    File::setFileNames(asmFname, objFname, binFname, opts);

    std::ifstream inFile(opts->inputFname());
    if (!inFile.is_open())
    {
        cerr << "can't open input file" << endl;
        return 1;
    }

    std::stringstream buf;
    buf << inFile.rdbuf();
    inFile.close();

    Program prg(buf.str(), opts);
    prg.parse();

    if (asmFname.empty())
    {
        asmFname = File::tempFile(".s");

        if (asmFname.empty())
        {
            cerr << "can't create temporary file" << endl;
        }
    }

    std::ofstream outFile(asmFname);
    if (!outFile.is_open())
    {
        cerr << "can't open output file" << endl;
        return 1;
    }

    outFile << prg;
    outFile.close();

    if (!opts->assemble())
        return 0;

    if (objFname.empty())
    {
        objFname = File::tempFile(".o");

        if (objFname.empty())
        {
            cerr << "can't create temporary file" << endl;
        }
    }

    int res = bp::system(bp::search_path("as"), "-o", objFname, asmFname);
    unlink(asmFname.c_str());

    if (res != 0)
    {
        cerr << "as returned " << res << " exit status" << endl;
        return 1;
    }

    if (!opts->link())
        return 0;

    res = bp::system(bp::search_path("ld"), "-o", binFname, objFname);
    unlink(objFname.c_str());

    if (res != 0)
    {
        cerr << "ld returned " << res << " exit status" << endl;
        return 1;
    }

    return 0;
}
