#pragma once

#include <string>
#include <sstream>
#include <ranges>
#include <cstdio>
#include <memory>
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
inline std::string exec(const char* cmd){
    //https://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";

    std::string result;
    char buffer[128];

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }

    return result;
}

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
inline bool isProcessRunning(const std::string& process_name) {
    if (process_name.empty()) {
        return false;
    }

    // Construct shell command
    std::ostringstream command;
    command << "ps cax | grep -w " << '"' << process_name << '"';

    // Execute command and get process list
    const std::string allProcesses = exec(command.str().c_str());

    // Count occurrences of process_name
    const auto occurrences = std::ranges::count(allProcesses | std::views::split('\n'), process_name);

    return occurrences > 1;
}
