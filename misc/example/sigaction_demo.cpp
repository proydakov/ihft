//
// https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-sigaction-examine-change-signal-action
// https://habr.com/ru/post/141206/
//

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

constexpr int SLEEP_TIME = 9;

void mysignal(int signal)
{
    printf("In mysignal: %s.\n", strsignal(signal));
    sleep(SLEEP_TIME);
}

void mysa_sigaction(int signal, siginfo_t* info, void* context)
{
    printf("In mysa_sigaction: %s info: %p context: %p.\n", strsignal(signal), (void*)info, (void*)context);
    sleep(SLEEP_TIME);
}

void set_new_signal(int signal, struct sigaction& newhandler, const char * const name)
{
    if (sigaction(signal, &newhandler, nullptr) != -1)
        printf("New handler set for %s.\n", name);
}

int main()
{
    printf("PID: %d\n", getpid());

    printf("SIGINT: %d\n", SIGINT);
    printf("SIGUSR1: %d\n", SIGUSR1);
    printf("SIGUSR2: %d\n", SIGUSR2);

    struct sigaction newhandler;
    memset(&newhandler, 0, sizeof(newhandler));
    newhandler.sa_handler = mysignal;
    newhandler.sa_sigaction = &mysa_sigaction;
    newhandler.sa_flags = 0;

    sigemptyset(&newhandler.sa_mask);
    //sigaddset(&newhandler.sa_mask, SIGINT);
    sigaddset(&newhandler.sa_mask, SIGUSR1);
    sigaddset(&newhandler.sa_mask, SIGUSR2);

    //set_new_signal(SIGINT, newhandler, "SIGINT");
    set_new_signal(SIGUSR1, newhandler, "SIGUSR1");
    set_new_signal(SIGUSR2, newhandler, "SIGUSR2");

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, nullptr) != -1)
        printf("Block several signals {SIGINT}.\n");

    int val;
    if (1 == scanf("%d", &val))
    {
        printf("scaned: %d\n", val);
    }

    return 0;
}
