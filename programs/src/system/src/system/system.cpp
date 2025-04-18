#include <system/system.hpp>

#include <string>
#include <sstream>
#include <ranges>
#include <algorithm>
#include <cstdio>
#include <memory>
#include <string>


std::string exec(const char* cmd){
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


bool isProcessRunningMoreThanOnce(const std::string& process_name) {
    if (process_name.empty()) {
        return false;
    }

    // Construct shell command
    std::ostringstream command;
    command << "ps cax | grep -w " << '"' << process_name << '"';

    // Execute command and get process list
    const std::string allProcesses = exec(command.str().c_str());

    // count number of lines
    return std::ranges::count(allProcesses, '\n') > 1;
}
