// Adapted from tsan/mutex_bad_unlock.cpp
// RUN: %clangxx_tsan_dd -O1 %s -o %t && %env_tsan_dd_opts=halt_on_error=1 not %run %t 2>&1 | FileCheck %s

#include <pthread.h>

static pthread_mutex_t mu;

static void *unlocker(void *) {
  pthread_mutex_unlock(&mu);
  return 0;
}

int main() {
  pthread_mutex_init(&mu, 0);
  pthread_mutex_lock(&mu);
  pthread_t t;
  pthread_create(&t, 0, unlocker, 0);
  pthread_join(t, 0);
  return 0;
}

// CHECK: WARNING: ThreadSanitizer: unlock of an unlocked mutex (or by a wrong thread)
// CHECK:     {{.*}} in pthread_mutex_unlock
// CHECK:   Mutex M0 (0x{{.*}}) created at:
// CHECK:     {{.*}} in pthread_mutex_init
// CHECK: SUMMARY: ThreadSanitizer: unlock of an unlocked mutex (or by a wrong thread)
