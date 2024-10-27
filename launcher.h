#pragma once
#include <csignal>
#include <string>
#include <vector>

class Process
{
    pid_t child;
    int tochild, fromchild;
    FILE *childw, *childr;
    bool terminated = false;
    void CloseAll();

public:
    Process(const std::string& filename, std::vector<std::string> argv,
            std::vector<std::string> env, void (*doBeforeChild)(void* params),
            void* doBeforeChildParams);
    Process(const std::string& filename, const std::vector<std::string>& argv,
            const std::vector<std::string>& env);
    FILE* StdIN() const;
    FILE* StdOUT() const;
    pid_t PID() const;
    int Wait();
    void Kill();
    ~Process();
};
