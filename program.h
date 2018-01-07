#ifndef PROGRAM_H
#define PROGRAM_H

#include "instruction.h"
#include "options.h"
#include <ostream>
#include <string>
#include <vector>
#include <stack>
#include <memory>

class Program
{
public:
    Program(std::string code, std::shared_ptr<Options> opts);
    ~Program() = default;

    bool parse();
    friend std::ostream &operator<< (std::ostream &stream, const Program &prg);

private:
    std::string origCode_;
    std::vector<Instruction> code_;
    std::shared_ptr<Options> opts_;
    std::stack<int> loopStack_;

    void trim();
    bool checkLoops() const;

    std::string genPrologue() const;
    std::string genWrite() const;
    std::string genRead() const;
    std::string genCleanup() const;

    int beginLoop();
    int endLoop();
};

#endif // PROGRAM_H
