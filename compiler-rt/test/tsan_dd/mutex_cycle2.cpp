// Adapted from tsan/mutex_cycle2.c
// RUN: %clangxx_tsan_dd %s -o %t
// RUN: %env_tsan_dd_opts=halt_on_error=1 not %run %t 2>&1 | FileCheck %s
// RUN: %env_tsan_dd_opts=report_bugs=0   %run %t 2>&1 | FileCheck %s --check-prefix=DISABLED
#include <pthread.h>
#include <stdio.h>

int main() {
  pthread_mutex_t mu1, mu2;
  pthread_mutex_init(&mu1, NULL);
  pthread_mutex_init(&mu2, NULL);

  // mu1 => mu2
  pthread_mutex_lock(&mu1);
  pthread_mutex_lock(&mu2);
  pthread_mutex_unlock(&mu2);
  pthread_mutex_unlock(&mu1);

  // mu2 => mu1 — creates the cycle
  pthread_mutex_lock(&mu2);
  pthread_mutex_lock(&mu1);
  // CHECK: ThreadSanitizer: lock-order-inversion (potential deadlock)
  // DISABLED-NOT: ThreadSanitizer
  pthread_mutex_unlock(&mu1);
  pthread_mutex_unlock(&mu2);

  pthread_mutex_destroy(&mu1);
  pthread_mutex_destroy(&mu2);
  fprintf(stderr, "PASS\n");
  // DISABLED: PASS
}
