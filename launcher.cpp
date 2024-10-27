#include "launcher.h"
#include <csignal>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

Process::Process(const string& filename, vector<string> argv,
                 vector<string> env, void (*doBeforeChild)(void* params),
                 void* doBeforeChildParams)
{
    int _tochild[2];
    int _fromchild[2];
    pipe(_tochild);
    pipe(_fromchild);
    child = fork();
    if (!child)
    {
        close(_tochild[1]);
        close(_fromchild[0]);
        dup2(_tochild[0], STDIN_FILENO);
        dup2(_fromchild[1], STDOUT_FILENO);
        doBeforeChild(doBeforeChildParams);
        vector<char*> _argv(argv.size() + 1);
        for (size_t i = 0; i < argv.size(); i++)
            _argv[i] = argv[i].data();
        vector<char*> _env(env.size() + 1);
        for (size_t i = 0; i < env.size(); i++)
            _env[i] = env[i].data();
        execve(filename.c_str(), _argv.data(), _env.data());
    }
    close(_tochild[0]);
    close(_fromchild[1]);
    tochild = _tochild[1];
    fromchild = _fromchild[0];
    childr = fdopen(fromchild, "r");
    childw = fdopen(tochild, "w");
}

Process::Process(const string& filename, const vector<string>& argv,
                 const vector<string>& env)
    : Process(filename, argv, env, [](void*) {}, 0)
{
}

FILE* Process::StdIN() const
{
    return childw;
}

FILE* Process::StdOUT() const
{
    return childr;
}

pid_t Process::PID() const
{
    return child;
}

int Process::Wait()
{
    int stat;
    CloseAll();
    waitpid(child, &stat, 0);
    terminated = true;
    return stat;
}

void Process::Kill()
{
    CloseAll();
    kill(child, SIGKILL);
    terminated = true;
}

Process::~Process()
{
    if (!terminated)
        CloseAll();
}

void Process::CloseAll()
{
    fclose(childw);
    fclose(childr);
    close(tochild);
    close(fromchild);
}
