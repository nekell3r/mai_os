#include "../common/comm.h"
#include <cstdlib>

struct SharedData {
    int number;
    int response;
    bool has_request;
    bool has_response;
    bool should_terminate;
};

bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

int main() {
    LogMsg("child", "Child process started");
    
    const char* mapping_name = "Local\\Lab3SharedMemory";
    HANDLE hMapFile = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        mapping_name
    );
    
    ASSERT_MSG(hMapFile != nullptr, "Failed to open file mapping");
    if (hMapFile == nullptr) {
        LogErr("child", "Failed to open file mapping");
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
        LogErr("child", "Failed to map view of file");
        CloseHandle(hMapFile);
        return 1;
    }
    
    LogMsg("child", "Shared memory opened");
    
    const char* event_request_name = "Local\\Lab3EventRequest";
    const char* event_response_name = "Local\\Lab3EventResponse";
    
    HANDLE hEventRequest = OpenEventA(
        EVENT_ALL_ACCESS,
        FALSE,
        event_request_name
    );
    ASSERT_MSG(hEventRequest != nullptr, "Failed to open request event");
    
    HANDLE hEventResponse = OpenEventA(
        EVENT_ALL_ACCESS,
        FALSE,
        event_response_name
    );
    ASSERT_MSG(hEventResponse != nullptr, "Failed to open response event");
    
    LogMsg("child", "Synchronization events opened");
    
    while (true) {
        DWORD wait_result = WaitForSingleObject(hEventRequest, INFINITE);
        
        if (wait_result != WAIT_OBJECT_0) {
            LogErr("child", "Error waiting for event");
            break;
        }
        
        if (pSharedData->should_terminate) {
            LogMsg("child", "Received termination signal, shutting down");
            break;
        }
        
        if (!pSharedData->has_request) {
            continue;
        }
        
        int number = pSharedData->number;
        pSharedData->has_request = false;
        
        std::stringstream msg;
        msg << "Received number " << number;
        LogMsg("child", msg.str());
        
        if (number < 0) {
            LogMsg("child", "Number is negative, shutting down");
            pSharedData->response = -1;
            pSharedData->has_response = true;
            SetEvent(hEventResponse);
            break;
        }
        
        if (is_prime(number)) {
            std::stringstream msg_prime;
            msg_prime << "Number " << number << " is prime, shutting down";
            LogMsg("child", msg_prime.str());
            pSharedData->response = -1;
            pSharedData->has_response = true;
            SetEvent(hEventResponse);
            break;
        }
        
        std::stringstream msg_composite;
        msg_composite << "Number " << number << " is composite, sending to parent";
        LogMsg("child", msg_composite.str());
        
        pSharedData->response = number;
        pSharedData->has_response = true;
        
        SetEvent(hEventResponse);
    }
    
    UnmapViewOfFile(pSharedData);
    CloseHandle(hMapFile);
    CloseHandle(hEventRequest);
    CloseHandle(hEventResponse);
    
    LogMsg("child", "Shutting down");
    return 0;
}

