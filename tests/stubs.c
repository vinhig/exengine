/* Stubs for exengine subsystems that tests don't need */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* log stub */
void log_log(int level, const char *file, int line, const char *fmt, ...) {
  (void)level; (void)file; (void)line; (void)fmt;
}

/* io stubs */
char *ex_io_read(const char *path, const char *mode, size_t *len) {
  (void)path; (void)mode;
  if (len) *len = 0;
  return NULL;
}

void ex_io_write(const char *path, const void *data, size_t len, int append) {
  (void)path; (void)data; (void)len; (void)append;
}
