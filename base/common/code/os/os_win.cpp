#include "os_win.h"
#include "os.h"


using namespace os;

#define SYS_CALL_AND_LOG(call)                                                         \
    do {                                                                               \
        BOOL RES = call;                                                               \
        if (RES == FALSE) {                                                            \
            std::stringstream error;                                                   \
            error << #call << " failed. Error code: 0x" << std::hex << GetLastError(); \
            LogErr("os/win_32", error.str());                                          \
        }                                                                              \
    } while (0)

bool CreateProc(const ProcessStartInfo& info)
{
    STARTUPINFO cif;
    ZeroMemory(&cif, sizeof(STARTUPINFO));

    if (!info.flags & ProcessStartInfo::FOCUSED) {
        cif.wShowWindow = SW_SHOWMINNOACTIVE;
        cif.dwFlags = STARTF_USESHOWWINDOW;
    }

    BOOL redirectHandles = FALSE;
    if (info.redirectStdin || info.redirectStdout || info.redirectStderr) {
        cif.dwFlags |= STARTF_USESTDHANDLES;
        redirectHandles = TRUE;

        if (info.redirectStdin) {
            cif.hStdInput = info.redirectStdin;
        }
        if (info.redirectStdout) {
            cif.hStdOutput = info.redirectStdout;
        }
        if (info.redirectStderr) {
            cif.hStdError = info.redirectStderr;
        }
    }

    BOOL isSuccess = FALSE;
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    std::string cmd = info.executeCmdAsShell ? info.cmd : GenerateAppCmd(info.cmd.c_str(), info.args);

    DWORD creationFlags = CREATE_DEFAULT_ERROR_MODE;
    if (info.flags & ProcessStartInfo::HIDDEN) {
        creationFlags |= CREATE_NO_WINDOW;
    } else {
        if (!(info.flags & ProcessStartInfo::USE_PARENT_CONSOLE)) {
            creationFlags |= CREATE_NEW_CONSOLE;
        }
    }

    BOOL res = CreateProcess(info.executeCmdAsShell ? nullptr : const_cast<char*>(info.cmd.c_str()),
        const_cast<char*>(cmd.data()),
        nullptr,
        nullptr,
        redirectHandles,
        creationFlags,
        nullptr,
        info.workDir.empty() ? nullptr : info.workDir.c_str(),
        &cif,
        &pi);

    if (res == FALSE) {
        std::stringstream error;
        error << "Create process failed, error: 0x" << std::hex << GetLastError();
        LogErr("os/win_32", error.str());
    }

    if (pi.hThread != nullptr && pi.hThread != INVALID_HANDLE_VALUE) {
        CloseHandle(pi.hThread);
    }

    if (pi.hProcess == INVALID_HANDLE_VALUE) {
        std::stringstream error;
        error << "Create process return invalid process handle, error: 0x" << std::hex << GetLastError();
        LogErr("os/win_32", error.str());
        return nullptr;
    }

    return reinterpret_cast<ProcessHandle>(pi.hProcess);
}

std::string GenerateAppCmd(const char* cmd, const std::vector<std::string>& args)
{
    static constexpr const char SEPARATOR = ' ';

    std::stringstream stream;
    stream << cmd;
    for (const std::string& arg : args) {
        bool needWrap = std::find(all(arg), SEPARATOR) != arg.end();

        if (needWrap) {
            stream << SEPARATOR << "\"" << arg << "\"";
        } else {
            stream << SEPARATOR << arg;
        }
    }
    return stream.str();
}

void CloseProc(ProcessHandle process)
{
    CloseHandle(process);
}

void KillProc(ProcessHandle process, uint32_t ret)
{
    TerminateProcess(process, static_cast<DWORD>(ret));
}

bool CreatePipe(FileHandle& outReadPipe, FileHandle& outWritePipe)
{
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE pipeRead, pipeWrite;
    SYS_CALL_AND_LOG(CreatePipe(&pipeRead, &pipeWrite, &sa, 0));
    outReadPipe = reinterpret_cast<FileHandle>(pipeRead);
    outWritePipe = reinterpret_cast<FileHandle>(pipeWrite);
    return outReadPipe && outWritePipe;
}

int PipeWrite(FileHandle pipe, const void* buf, int count)
{
    DWORD countWritten;
    if (WriteFile(pipe, buf, count, &countWritten, nullptr)) {
        return countWritten;
    }
    return -1;
}

int os::PipeRead(FileHandle pipe, void* buf, int count)
{
    DWORD countRead;
    if (ReadFile(pipe, buf, count, &countRead, nullptr)) {
        return countRead;
    }
    return -1;
}