#pragma once

#include "common/comm.h"

namespace os {
struct FileHandleDummy { };
typedef FileHandleDummy* FileHandle;

struct ProcessHandleDummy { };
typedef ProcessHandleDummy* ProcessHandle;

struct ProcessStartInfo {
    enum FLAGS : BYTE {
        FOCUSED = 0,
        HIDDEN,
        USE_PARENT_CONSOLE,

        LAST
    };

    std::string cmd;
    std::vector<std::string> args;
    std::string workDir;
    BYTE flags;
    bool executeCmdAsShell = false;

    FileHandle redirectStdin = nullptr;
    FileHandle redirectStdout = nullptr;
    FileHandle redirectStderr = nullptr;

    ProcessStartInfo() = default;
};

bool CreateProc(const ProcessStartInfo& args);

std::string GenerateAppCmd(const char* cmd, const std::vector<std::string>& args);

void CloseProc(ProcessHandle proc);

void KillProc(ProcessHandle proc, uint32_t ret = -1);

bool CreatePipe(FileHandle& readPipe, FileHandle& writePipe);

int PipeWrite(FileHandle pipe, const void* buf, int count);

int PipeRead(FileHandle pipe, void* buf, int count);
} // namespace