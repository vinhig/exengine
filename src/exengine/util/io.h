/* io
  Generic io helper functions for
  saving and loading data.
*/

#pragma once

#include <stdio.h>

/**
 * [ex_io_read reads a file into a char array]
 * @param  path [file path]
 * @param  mode [access mode]
 * @return      [malloc'd char array, remember to free]
 */
char *ex_io_read(const char *path, const char *mode, size_t *len);

/**
 * [ex_io_prefix_str prefix a string with another]
 * @param  dest   [destination string]
 * @param  src    [input string]
 * @param  prefix [prefix string]
 * @return        [input string with prefix]
 */
void ex_io_prefix_str(char *dest, const char *src, const char *prefix);

/**
 * [ex_io_get_extension get the file extension string]
 * @param dest [destination string]
 * @param src  [source string]
 */
void ex_io_get_extension(char *dest, const char *src);

/**
 * [ex_io_write writes bytes to a file]
 * @param path   [path to file]
 * @param data   [data to write]
 * @param len    [length of data]
 * @param append [if set to 1, append to end of file]
 */
void ex_io_write(const char *path, const void *data, size_t len, int append);

/**
 * [ex_io_exists check if file exists]
 * @param path   [path to file]
 */
bool ex_io_exist(const char *path);
