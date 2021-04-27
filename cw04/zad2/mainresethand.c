#define _XOPEN_SOURCE 700
#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>

static void exit_v(const char *info);
static void handler(int sig);

int
main() {
    struct sigaction action;

    action.sa_handler = handler;
    action.sa_flags = SA_RESETHAND;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        exit_v("sigaction");
    }

    printf("Press CTRL+C\n");

    
    for (;;) {
        pause();
    }

    
    exit(EXIT_SUCCESS);
}

static void
handler(__attribute__((unused)) int signal) {
  printf("handler with SA_RESETHAND. Press CTRL+C\n");
}

static void
exit_v(const char *info) {
  perror(info);
  exit(EXIT_FAILURE);
}