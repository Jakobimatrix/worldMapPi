#pragma once

#include <string>


/**
 * @brief Executes a shell command and returns its output as a string.
 *
 * This function uses `popen` to execute a shell command and captures its output.
 * The command is executed in a read-only pipe, and its output is collected
 * into a `std::string`.
 *
 * @param cmd The shell command to execute.
 * @return The output of the command as a `std::string`. If an error occurs, returns `"ERROR"`.
 *
 * @note This function relies on `popen`, which may not be secure if used with untrusted input.
 * @warning Using this function with arbitrary user input can lead to command injection vulnerabilities.
 */
std::string exec(const char* cmd);

/**
 * @brief Checks if a process with the given name is running more than once.
 *
 * This function executes the `ps cax` command to list all processes and filters 
 * them using `grep` to check how many instances of the given process name exist.
 *
 * @param process_name The name of the process to check.
 * @return `true` if more than one instance of the process is running, otherwise `false`.
 *
 * @note This function uses `exec()` to run shell commands, so it is platform-dependent.
 * @warning If the process name is a substring of another process, false positives may occur.
 */
bool isProcessRunning(const std::string& process_name);