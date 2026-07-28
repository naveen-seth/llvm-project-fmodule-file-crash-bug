// Adapted from tsan/mutex_cycle_long.c
// RUN: %clangxx_tsan_dd %s -o %t
// RUN: %env_tsan_dd_opts=halt_on_error=1 not %run %t 5  2>&1 | FileCheck %s
// RUN: %env_tsan_dd_opts=halt_on_error=1 not %run %t 10 2>&1 | FileCheck %s
// RUN: %env_tsan_dd_opts=halt_on_error=1 not %run %t 15 2>&1 | FileCheck %s
// RUN: %env_tsan_dd_opts=halt_on_error=1 not %run %t 20 2>&1 | FileCheck %s
// RUN:                                       %run %t 30 2>&1 | FileCheck %s --check-prefix=CHECK-TOO-LONG-CYCLE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int num_mutexes = 5;
  if (argc > 1) num_mutexes = atoi(argv[1]);

  pthread_mutex_t *m =
      (pthread_mutex_t *)malloc(num_mutexes * sizeof(pthread_mutex_t));
  for (int i = 0; i < num_mutexes; ++i)
    pthread_mutex_init(&m[i], NULL);

  for (int i = 0; i < num_mutexes - 1; ++i) {
    pthread_mutex_lock(&m[i]);
    pthread_mutex_lock(&m[i + 1]);

    pthread_mutex_unlock(&m[i]);
    pthread_mutex_unlock(&m[i + 1]);
  }

  pthread_mutex_lock(&m[num_mutexes - 1]);
  pthread_mutex_lock(&m[0]);

  pthread_mutex_unlock(&m[num_mutexes - 1]);
  pthread_mutex_unlock(&m[0]);

  for (int i = 0; i < num_mutexes; ++i)
    pthread_mutex_destroy(&m[i]);
  free(m);

  fprintf(stderr, "PASS\n");
}

// CHECK: ThreadSanitizer: lock-order-inversion (potential deadlock)
// CHECK-TOO-LONG-CYCLE: WARNING: too long mutex cycle found
// CHECK-TOO-LONG-CYCLE: PASS
