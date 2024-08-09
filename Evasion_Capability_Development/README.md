# Evastion Capability Development

## Lab Setup

Ubuntu Desktop (VSCODE)
Windows Desktop (CMD & POWERSHELL)


### Problem 1:  Anti-Virus Signature Detection
Anti Virus Essentially uses a list of strings to check statically through binaries and other files to detect strings that it considers malicous.  If you want to check multiple engines at the same time you can run your binaries through virus total, and check on the detections. For this lab we will focus on defender and some yara sigantures that we want to defeat.  File read events trigger antivirus actions on various files.

#### Sample Evil File Detection
1. Open up the **Windows Desktop**
1. Right click, the cmd shortcut on the Desktop, and run as administrator. (two finger clikc on MacOS)
1. In the CMD console, switch directories to the LAB_FILES\Evasion_Capability directory. `cd c:\Users\Public\Desktop\LAB_FILES\Evasion_Capability_Development`
1. Type powershell in the search bar, and right click and run powershell, (not powershell 7) as administrator.
> We will use both CMD and Powershell.
1. In Powershell run `get-mpthreat`.
> You will find the `dumplsass.exe` that happened at the time the files were downloaded from the Github.  This is because Defender was triggered by the CreateFile event.  Anti-Virus triggers, and sometiems triggers different kinds of signatures and rules for different actions.

---
**Now Setup in Ubuntu Desktop**
1. In the Pluralsight lab environment.  Hold "CTRL+SHFT+ALT" or on Mac "OPT+CMD+SHFT" to open the left menu.  
1. Click the "Pluralsight" Header in the top left.
1. Choose the "Ubuntu" server.  This is where we will do our development.
1. Wait for Ubuntu to come up, if you haven't launched it yet, it may have a black screen for just a moment.
1. Open a terminal, use the icon on the bottom. In the terminal that pops up, run `cd ~/lab`
1. Then run vscode `code .` (This will take a moment to come up, technically it is the first run.)
1. Click "Yes I trust the Authors"  (of course you do!)
1. On the left navigation menu, click Evasion_Capability_Development>1-signature-evasion, and open the file "lsass-dump.c"
> We will walk through some of the lines. Specifically the string literals.
1. Now open the lsass-dump-evasion.c file. You can open it with a "right-click" and chose to open it to the side.
1. Take a look at lines 56, 97, 100.  You can also use ctrl+f and search for "evasion".
> These directly line up with the yara signatures found on virust total. They were essentially instructions for how to accomplish evasion.  The new evasion file is already compiled for you.

---
**Swap Back to the Windows Desktop**

1. In the powershell window, make sure you are in the directory `cd c:\Users\Public\Desktop\LAB_FILES\Evasion_Capability_Development\1-signature-evasion`.
1. Once there run `expand-archive dumplsass.exe.zip`. **Make Sure Defender is Still Off**
1. With the file write already done, turn defender on!
1. In the powershell console, turn on windows defender and remove .exe exclusions with this command in powershell:
```
Set-MpPreference -DisableRealtimeMonitoring $False
Remove-MpPreference -ExclusionExtension ".exe"
Get-MpPreference
Get-MpComputerStatus
```
> The last command you can read the output to validate your changes, and a semi current rule set date. We will execute the rest of the workshop with defender on, and no added exclusions. From Get-MpComputerStatus you can see **RealTimePRotectionEnabled: True** is set.

1. In powershell, also navigate to following directory: `cd c:\Users\Public\Desktop\LAB_FILES\Evasion_Capability_Development\1-signature-evasion`. (You are probably already there.)
1. Now run the dumplsass.exe from the archive. `.\dumplsass.exe`
> It will now instead get got on the even "Process Create" analyze the file being loaded into memory, and catch it as malware. This will also cause defender to delte the file.
1. Run `get-mpthreat` to identify the detections that defender made on the dumplsass file.
1. That file has been identified and deleted, so now let's use the one that was created with the modifications that matched the yara signatures on the slides.
1. Now run `.\dumplsass-evade.exe`
> This is not detected my defender for the strings in the file, nor is it caugh at run time. Simply by changing a few lines...it is however hanging.
1. Exit the powershell window, the whole system is hanging, so you need to close the window to get functions back.
1. Now, check the expected output. In the CMD console run the following `dir c:\Users\Public` 
> Here you will find lp.dump with 0 bytes.  The code accessed the PID, but when going to create the file it has to load the contents of the process into a memory buffer. Windows defender recognizes the contents of it's own LSASS process with defender signatures and will block that activity on the event of it trying to be written to a file.
1. In CMD console, get into a powershell prompt.  `powershell`
1. Run `get-mpthreat`
> You will see there are no additional detections than from the previous detection of the file writes.




### Problem 2: EDR & RE Basic detections at Runtime
Anti-Virus is one thing, but the next issue for malware arises at runtime, or worse, pesky humans trying to RE things. 

#### API Hashing

1. On the Windows Desktop in an Administrative cmd console, ensure you are in this directory `cd c:\Users\Public\Desktop\LAB_FILES\Evasion_Capability_Development\2-api-hashing` 
1. From here run notepade.exe to start an existing process. 
1. Minimize notepade.
1. Now in the cmd console run `.\api-calls.exe`
> This will open a new notepade using shell execute, as well as gain access to the the thread handle of the existing notepad, as well as run the other api calls that show in the console.  But!  Notice defender doesn't care at all. But, if you upload this to virus total you get hits from EDR vendors...yes even crowdstrike. 

---

**Swap Back to Ubuntu Desktop**

1. Close the curren tabs.
1. In the left hand navigation open 2-api-hashing>api-call2.c
> Walk through the code, starting on line 47, "main" you can see the comments for the the various API's being called directly. As in, because we linked them by loading the windows.h file with the include statement.  This is the code that is executed in the api-calls.exe file you just ran. These then show up in clear text in the exe, which can be used as signatures, and provide information about what the exe is/can do for your RE.
1. To avoid this, we need to obfuscate how we call those API's from the libraries already loaded in memory on the system. Open the api-hash-gen-constant.py
1. 



### Problem 3: EDR Hooking API Calls
EDR hooks calls to windows api. Think crowdstrike kernel level access ;).

1. 
1.
1.
1.

####
1. 

