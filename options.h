#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

class Options
{
public:
    explicit Options();
    ~Options() = default;

    bool parse(int argc, char *argv[]);

    const std::string &inputFname() const;
    const std::string &outputFname() const;
    bool assemble() const;
    bool link() const;
    int memSize() const;

    const std::string &err() const;

private:
    std::string inputFname_;
    std::string outputFname_;
    bool assemble_;
    bool link_;
    int memSize_;

    std::string err_;

    void defaults();
};

#endif // OPTIONS_H
