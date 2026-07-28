// RUN: %clangxx_tsan_dd -O1 %s -o %t && %env_tsan_dd_opts=halt_on_error=1 not %run %t 2>&1 | FileCheck %s

#include <pthread.h>

static void *thread(void *) {
  pthread_mutex_t m;
  pthread_mutex_init(&m, 0);
  pthread_mutex_lock(&m);
  pthread_mutex_destroy(&m);
  return 0;
}

int main() {
  pthread_t th;
  pthread_create(&th, 0, thread, 0);
  pthread_join(th, 0);
  return 0;
}

// CHECK: WARNING: ThreadSanitizer: destroy of a locked mutex
// CHECK:     {{.*}} in pthread_mutex_destroy
// CHECK:   and:
// CHECK:     {{.*}} in pthread_mutex_lock
// CHECK:   Mutex M0 (0x{{.*}}) created at:
// CHECK:     {{.*}} in pthread_mutex_init
// CHECK: SUMMARY: ThreadSanitizer: destroy of a locked mutex
