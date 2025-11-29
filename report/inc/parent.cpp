#include "../common/comm.h"
#include <fstream>
#include <string>

struct SharedData {
    int number;
    int response;
    bool has_request;
    bool has_response;
    bool should_terminate;
};

int main() {
    std::string filename;
    std::cout << "Enter filename: ";
    std::getline(std::cin, filename);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        LogErr("parent", "Failed to open file: " + filename);
        return 1;
    }
    
    LogMsg("parent", "Reading file '" + filename + "'");
    
    const char* mapping_name = "Local\\Lab3SharedMemory";
    HANDLE hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        sizeof(SharedData),
        mapping_name
    );
    
    ASSERT_MSG(hMapFile != nullptr, "Failed to create file mapping");
    if (hMapFile == nullptr) {
        LogErr("parent", "Failed to create file mapping");
        return 1;
    }
    
    SharedData* pSharedData = static_cast<SharedData*>(MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(SharedData)
    ));
    
    ASSERT_MSG(pSharedData != nullptr, "Failed to map view of file");
    if (pSharedData == nullptr) {
        LogErr("parent", "Failed to map view of file");
        CloseHandle(hMapFile);
        return 1;
    }
    
    pSharedData->number = 0;
    pSharedData->response = 0;
    pSharedData->has_request = false;
    pSharedData->has_response = false;
    pSharedData->should_terminate = false;
    
    LogMsg("parent", "Shared memory initialized");
    
    const char* event_request_name = "Local\\Lab3EventRequest";
    const char* event_response_name = "Local\\Lab3EventResponse";
    
    HANDLE hEventRequest = CreateEventA(
        nullptr,
        FALSE,
        FALSE,
        event_request_name
    );
    ASSERT_MSG(hEventRequest != nullptr, "Failed to create request event");
    
    HANDLE hEventResponse = CreateEventA(
        nullptr,
        FALSE,
        FALSE,
        event_response_name
    );
    ASSERT_MSG(hEventResponse != nullptr, "Failed to create response event");
    
    LogMsg("parent", "Synchronization events created");
    
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    char cmd[] = "child.exe";
    BOOL process_created = CreateProcess(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    ASSERT_MSG(process_created, "Failed to create child process child.exe");
    if (!process_created) {
        std::stringstream error;
        error << "Failed to create child process. Error code: 0x" << std::hex << GetLastError();
        LogErr("parent", error.str());
        UnmapViewOfFile(pSharedData);
        CloseHandle(hMapFile);
        CloseHandle(hEventRequest);
        CloseHandle(hEventResponse);
        return 1;
    }
    
    LogMsg("parent", "Child process started");
    
    Sleep(100);
    
    std::string line;
    bool should_terminate = false;
    
    while (std::getline(file, line) && !should_terminate) {
        if (line.empty()) continue;
        
        try {
            int number = std::stoi(line);
            
            std::stringstream msg;
            msg << "Sending number " << number << " to child process";
            LogMsg("parent", msg.str());
            
            pSharedData->number = number;
            pSharedData->has_request = true;
            pSharedData->has_response = false;
            
            SetEvent(hEventRequest);
            
            DWORD wait_result = WaitForSingleObject(hEventResponse, 5000);
            
            if (wait_result == WAIT_OBJECT_0) {
                if (pSharedData->has_response) {
                    int response = pSharedData->response;
                    
                    if (response == -1) {
                        LogMsg("parent", "Received termination signal from child process");
                        should_terminate = true;
                    } else if (response > 0) {
                        std::stringstream msg_resp;
                        msg_resp << "Received composite number " << response << " from child process";
                        LogMsg("parent", msg_resp.str());
                    }
                    
                    pSharedData->has_response = false;
                }
            } else if (wait_result == WAIT_TIMEOUT) {
                LogErr("parent", "Timeout waiting for response from child process");
                break;
            } else {
                LogErr("parent", "Error waiting for event");
                break;
            }
            
        } catch (const std::exception& e) {
            std::stringstream err;
            err << "Error converting string to number: " << line;
            LogErr("parent", err.str());
        }
    }
    
    pSharedData->should_terminate = true;
    SetEvent(hEventRequest);
    
    file.close();
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    std::stringstream exit_msg;
    exit_msg << "Child process exited with code " << exitCode;
    LogMsg("parent", exit_msg.str());
    
    UnmapViewOfFile(pSharedData);
    CloseHandle(hMapFile);
    CloseHandle(hEventRequest);
    CloseHandle(hEventResponse);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    LogMsg("parent", "Shutting down");
    return 0;
}

