/* io
  Generic io helper functions for
  saving and loading data.
*/

#pragma once

#include <stdio.h>

/**
 * @brief Read a file into a char array.
 * @param path file path
 * @param mode access mode
 * @param len  output length of the file data
 * @return allocated char array (caller must free)
 */
char *ex_io_read(const char *path, const char *mode, size_t *len);

/**
 * @brief Prefix a string with another.
 * @param dest   destination string
 * @param src    input string
 * @param prefix prefix string
 */
void ex_io_prefix_str(char *dest, const char *src, const char *prefix);

/**
 * @brief Get the file extension from a path.
 * @param dest destination string
 * @param src  source string
 */
void ex_io_get_extension(char *dest, const char *src);

/**
 * @brief Write bytes to a file.
 * @param path   path to file
 * @param data   data to write
 * @param len    length of data
 * @param append if non-zero, append to end of file
 */
void ex_io_write(const char *path, const void *data, size_t len, int append);

/**
 * @brief Check if a file exists.
 * @param path path to file
 * @return true if the file exists
 */
bool ex_io_exist(const char *path);
