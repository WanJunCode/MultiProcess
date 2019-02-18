#include "Process.h"

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

Process::Process(std::function<void(int)> function, int index) noexcept
    : pid_(0)
    , func_(function)
    , index_(index) {
    run();
}

Process::~Process() {
    kill(true);
    get_exit_status();
}

void Process::run() {
    pid_t pid =fork();
    if (pid < 0) { //error in fork!
        fprintf(stderr, "fork error!\n");
        printf("process[%d] fork error", getpid());
    } else if (pid == 0) { //child process
        printf("current child process<==%d==>\n", getpid());
        if (func_)
            func_(index_);

        _exit(EXIT_SUCCESS);
    } else { //parent process
        printf("process[%d] fork child process[==%d==]\n", getpid(), pid);
        pid_ = pid;
    }
}

int Process::get_exit_status() noexcept {
    if (pid_ <= 0)
        return -1;

    int exit_status;
    waitpid(pid_, &exit_status, 0);

    if (exit_status >= 256)
        exit_status = exit_status >> 8;
    return exit_status;
}

bool Process::try_get_exit_status(int &exit_status) noexcept {
    if (pid_ <= 0)
        return false;

    if (0 == waitpid(pid_, &exit_status, WNOHANG))
        return false;

    if (exit_status >= 256)
        exit_status = exit_status >> 8;

    return true;
}

void Process::kill() noexcept {
    Process::kill(pid_);
}

void Process::kill(pid_t id) noexcept {
    if (id <= 0)
        return;

    ::kill(id, SIGKILL);
}
