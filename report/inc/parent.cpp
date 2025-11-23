#include "../common/comm.h"
#include <fstream>
#include <string>

int main() {
    std::string filename;
    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        LogErr("parent", "Cannot open file: " + filename);
        return 1;
    }
    
    LogMsg("parent", "Reading file '" + filename + "'");
    
    HANDLE pipe_to_child_read = nullptr;
    HANDLE pipe_to_child_write = nullptr;
    HANDLE pipe_from_child_read = nullptr;
    HANDLE pipe_from_child_write = nullptr;
    
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = nullptr;
    
    BOOL pipe1_created = CreatePipe(&pipe_to_child_read, &pipe_to_child_write, &sa, 0);
    ASSERT_MSG(pipe1_created, "Failed to create pipe_to_child");
    if (!pipe1_created) {
        LogErr("parent", "Failed to create pipe_to_child");
        return 1;
    }
    
    BOOL pipe2_created = CreatePipe(&pipe_from_child_read, &pipe_from_child_write, &sa, 0);
    ASSERT_MSG(pipe2_created, "Failed to create pipe_from_child");
    if (!pipe2_created) {
        LogErr("parent", "Failed to create pipe_from_child");
        CloseHandle(pipe_to_child_read);
        CloseHandle(pipe_to_child_write);
        return 1;
    }
    
    BOOL inherit1 = SetHandleInformation(pipe_to_child_read, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    ASSERT_MSG(inherit1, "Failed to set inheritance for pipe_to_child_read");
    
    BOOL inherit2 = SetHandleInformation(pipe_from_child_write, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    ASSERT_MSG(inherit2, "Failed to set inheritance for pipe_from_child_write");
    
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = pipe_to_child_read;
    si.hStdOutput = pipe_from_child_write;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    char cmd[] = "child.exe";
    BOOL process_created = CreateProcess(nullptr, cmd, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);
    ASSERT_MSG(process_created, "Failed to create child process child.exe");
    if (!process_created) {
        std::stringstream error;
        error << "Failed to create child process. Error code: 0x" << std::hex << GetLastError();
        LogErr("parent", error.str());
        CloseHandle(pipe_to_child_read);
        CloseHandle(pipe_to_child_write);
        CloseHandle(pipe_from_child_read);
        CloseHandle(pipe_from_child_write);
        return 1;
    }
    
    CloseHandle(pipe_to_child_read);
    CloseHandle(pipe_from_child_write);
    
    std::string line;
    bool should_terminate = false;
    
    while (std::getline(file, line) && !should_terminate) {
        if (line.empty()) continue;
        
        try {
            int number = std::stoi(line);
            
            std::stringstream msg;
            msg << "Sending number " << number << " to child";
            LogMsg("parent", msg.str());
            
            DWORD bytes_written;
            BOOL write_success = WriteFile(pipe_to_child_write, &number, sizeof(int), &bytes_written, nullptr);
            ASSERT_MSG(write_success && bytes_written == sizeof(int), "Error writing number to pipe");
            if (!write_success || bytes_written != sizeof(int)) {
                LogErr("parent", "Error writing to pipe");
                break;
            }
            
            int response;
            DWORD bytes_read;
            if (!ReadFile(pipe_from_child_read, &response, sizeof(int), &bytes_read, nullptr)) {
                LogErr("parent", "Error reading from pipe");
                break;
            }
            
            if (bytes_read == sizeof(int)) {
                if (response == -1) {
                    LogMsg("parent", "Received termination signal from child");
                    should_terminate = true;
                } else if (response > 0) {
                    std::stringstream msg_resp;
                    msg_resp << "Received composite number " << response << " from child";
                    LogMsg("parent", msg_resp.str());
                }
            } else if (bytes_read == 0) {
                LogMsg("parent", "Child process terminated");
                break;
            }
            
        } catch (const std::exception& e) {
            std::stringstream err;
            err << "Error converting string to number: " << line;
            LogErr("parent", err.str());
        }
    }
    
    file.close();
    CloseHandle(pipe_to_child_write);
    CloseHandle(pipe_from_child_read);
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    std::stringstream exit_msg;
    exit_msg << "Child process exited with code " << exitCode;
    LogMsg("parent", exit_msg.str());
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    LogMsg("parent", "Terminating");
    return 0;
}

