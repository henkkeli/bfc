#include "options.h"
#include <boost/program_options.hpp>
#include <string>

using std::string;
namespace po = boost::program_options;

const int DEFAULT_MEM_SIZE = 30000;

Options::Options()
{
    defaults();
}

bool Options::parse(int argc, char *argv[])
{
    defaults();

    try
    {
        po::options_description generic("Generic options");
        generic.add_options()
            ("output-file,o", po::value<string>(), "output file")
            ("mem-size", po::value<int>(), "memory size")
            ("no-assemble,S", "do not assemble")
            ("no-link,c", "do not link")
        ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-file", po::value<string>(), "input file")
        ;

        po::options_description cmdline_options;
        cmdline_options.add(generic).add(hidden);

        po::options_description visible("Allowed options");
        visible.add(generic);

        po::positional_options_description p;
        p.add("input-file", 1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
                  options(cmdline_options).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("input-file"))
        {
            inputFname_ = vm["input-file"].as<string>();
        }
        if (vm.count("output-file"))
        {
            outputFname_ = vm["output-file"].as<string>();
        }
        if (vm.count("mem-size"))
        {
            memSize_ = vm["mem-size"].as<int>();
        }
        if (vm.count("no-assemble"))
        {
            assemble_ = false;
            link_ = false;
        }
        if (vm.count("no-link"))
        {
            link_ = false;
        }
    }
    catch (const po::error &ex)
    {
        err_ = ex.what();
        return false;
    }

    if (inputFname_.empty())
    {
        err_ = "no input file";
        return false;
    }

    return true;
}

const std::string &Options::inputFname() const
{
    return inputFname_;
}

const std::string &Options::outputFname() const
{
    return outputFname_;
}

bool Options::assemble() const
{
    return assemble_;
}

bool Options::link() const
{
    return link_;
}

int Options::memSize() const
{
    return memSize_;
}

const std::string &Options::err() const
{
    return err_;
}

void Options::defaults()
{
    inputFname_.clear();
    outputFname_.clear();
    memSize_ = DEFAULT_MEM_SIZE;
    assemble_ = true;
    link_ = true;
}
