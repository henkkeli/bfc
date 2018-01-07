#include "program.h"
#include <string>
#include <vector>
#include <ostream>
#include <algorithm>

using std::string;
using std::vector;

Program::Program(string code, std::shared_ptr<Options> opts) :
    origCode_(code),
    opts_(opts)
{
}

string Program::genPrologue() const
{
    return "\t.globl\t_start\n"
           "\t.data\n"
           "buf:\n"
           "\t.zero\t" + std::to_string(opts_->memSize()) + "\n"
           "\t.text\n"
           "_start:\n"
           "\tmovq\t$buf, %rdi\n"; // init pointer to first memory cell
}

string Program::genWrite() const
{
    return "write:\n"
           "\tmovq\t$4, %rax\n"   // syscall write
           "\tmovq\t$1, %rbx\n"   // stdout
           "\tmovq\t%rdi, %rcx\n" // memory pointer
           "\tmovq\t$1, %rdx\n"   // 1 byte
           "\tint\t$0x80\n"
           "\tret\n";
}

string Program::genRead() const
{
    return "read:\n"
           "\tmovq\t$3, %rax\n"   // syscall read
           "\tmovq\t$0, %rbx\n"   // stdin
           "\tmovq\t%rdi, %rcx\n" // memory pointer
           "\tmovq\t$1, %rdx\n"   // 1 byte
           "\tint\t$0x80\n"
           "\tret\n";
}

string Program::genCleanup() const
{
    return "\tmovq\t$1, %rax\n"   // syscall exit
           "\tmovq\t$0, %rbx\n"   // exit code 0
           "\tint\t$0x80\n";
}

int Program::beginLoop()
{
    static int count = 0;
    loopStack_.push(count);
    return count++;
}

int Program::endLoop()
{
    int res = loopStack_.top();
    loopStack_.pop();
    return res;
}

bool Program::parse()
{
    trim();

    if (!checkLoops())
        return false;

    uint pos = 0;
    int count = 0;
    while (pos < origCode_.length())
    {
        char c = origCode_[pos];
        switch (c)
        {
        case '.':
        case ',':
            code_.push_back({c, 1});
            ++pos;
            break;
        case '[':
            code_.push_back({c, beginLoop()});
            ++pos;
            break;
        case ']':
            code_.push_back({c, endLoop()});
            ++pos;
            break;
        case '+':
        case '-':
            count = 0;

            while (pos < origCode_.length())
            {
                c = origCode_[pos++];
                if (c == '+')
                    ++count;
                else if (c == '-')
                    --count;
                else
                {
                    --pos;
                    break;
                }
            }

            if (count == 0)
                break;
            else if (count > 0)
                c = '+';
            else
            {
                c = '-';
                count = -count;
            }

            code_.push_back({c, count});
            break;
        case '>':
        case '<':
            count = 0;

            while (pos < origCode_.length())
            {
                c = origCode_[pos++];
                if (c == '>')
                    ++count;
                else if (c == '<')
                    --count;
                else
                {
                    --pos;
                    break;
                }
            }

            if (count == 0)
                break;
            else if (count > 0)
                c = '>';
            else
            {
                c = '<';
                count = -count;
            }

            code_.push_back({c, count});
            break;
        }
    }

    return true;
}

void Program::trim()
{
    // remove non-command characters
    origCode_.erase(std::remove_if(origCode_.begin(), origCode_.end(), [](char c){ return std::string("><+-.,[]").find(c) == std::string::npos; }), origCode_.end());
}

bool Program::checkLoops() const
{
    int loop = 0;
    for (const char c : origCode_)
    {
        if (c == '[')
            ++loop;
        else if (c == ']')
            --loop;

        if (loop < 0)
            return false;
    }

    return loop == 0;
}

std::ostream &operator<< (std::ostream &stream, const Program &prg)
{
    stream << prg.genPrologue();

    for (auto inst : prg.code_)
    {
        switch (inst.cmd)
        {
        case '<':
            stream << "\tsubq\t$" << inst.param << ", %rdi\n";
            break;
        case '>':
            stream << "\taddq\t$" << inst.param << ", %rdi\n";
            break;
        case '+':
            stream << "\taddb\t$" << inst.param << ", (%rdi)\n";
            break;
        case '-':
            stream << "\tsubb\t$" << inst.param << ", (%rdi)\n";
            break;
        case '[':
            stream << "LB" << inst.param << ":\n"
                      "\tcmpb\t$0, (%rdi)\n"
                      "\tje\tLE" << inst.param << "\n";
            break;
        case ']':
            stream << "\tjmp\tLB" << inst.param << "\n"
                      "LE" << inst.param << ":\n";
            break;
        case '.':
            stream << "\tcall\twrite\n";
            break;
        case ',':
            stream << "\tcall\tread\n";
            break;
        }
    }

    stream << prg.genCleanup();
    stream << prg.genWrite();
    stream << prg.genRead();

    return stream;
}
