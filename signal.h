#ifndef SIGNAL_H
#define SIGNAL_H

void handle_sigusr1(void);
void handle_signal(int signal);
void handle_sigusr2(void);
void handle_sigint(int sig, siginfo_t* info, void* vp);
#endif