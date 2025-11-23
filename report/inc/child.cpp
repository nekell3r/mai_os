#include "../common/comm.h"
#include <cstdlib>

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
    
    int number;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    ASSERT_MSG(hStdin != INVALID_HANDLE_VALUE, "Failed to get stdin handle");
    ASSERT_MSG(hStdout != INVALID_HANDLE_VALUE, "Failed to get stdout handle");
    
    while (true) {
        DWORD bytes_read;
        BOOL read_success = ReadFile(hStdin, &number, sizeof(int), &bytes_read, nullptr);
        if (!read_success) {
            LogErr("child", "Error reading from pipe");
            break;
        }
        
        if (bytes_read == 0) {
            LogMsg("child", "Parent closed pipe, terminating");
            break;
        }
        
        ASSERT_MSG(bytes_read == sizeof(int) || bytes_read == 0, "Incomplete data read from pipe");
        if (bytes_read != sizeof(int)) {
            LogErr("child", "Incomplete data read");
            break;
        }
        
        std::stringstream msg;
        msg << "Received number " << number;
        LogMsg("child", msg.str());
        
        if (number < 0) {
            LogMsg("child", "Number is negative, terminating");
            int response = -1;
            DWORD bytes_written;
            WriteFile(hStdout, &response, sizeof(int), &bytes_written, nullptr);
            return 0;
        }
        
        if (is_prime(number)) {
            std::stringstream msg_prime;
            msg_prime << "Number " << number << " is prime, terminating";
            LogMsg("child", msg_prime.str());
            int response = -1;
            DWORD bytes_written;
            WriteFile(hStdout, &response, sizeof(int), &bytes_written, nullptr);
            return 0;
        }
        
        std::stringstream msg_composite;
        msg_composite << "Number " << number << " is composite, sending to parent";
        LogMsg("child", msg_composite.str());
        DWORD bytes_written;
        BOOL write_success = WriteFile(hStdout, &number, sizeof(int), &bytes_written, nullptr);
        ASSERT_MSG(write_success && bytes_written == sizeof(int), "Error writing composite number to pipe");
    }
    
    LogMsg("child", "Terminating");
    return 0;
}

