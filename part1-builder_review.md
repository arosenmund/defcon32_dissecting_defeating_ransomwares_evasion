# Ghidra Analysis

In this section, we're going to open and configure as required for the workshop.

1. Double-click `ghidraRun.bat` on the desktop
 - Once Ghidra opens, you'll be at the Project window
1. Open the provided Ghidra project by choosing `File > Open Project`. Navigate to `Home > Documents > ghidra_projects > lb3builder.gpr` and select to open the project.
1. Double-click on the `builder.exe` file in the recently-opened project.
1. Select the `Edit` menu and select `Tool Options`
1. From the left-hand nav menu, expand `Listing Fields`
1. Choose `Cursor text highlight` in this menu
1. Change the `Mouse button the activate` from `MIDDLE` -> `LEFT`
1. Press `OK` to save your settings

    The above steps walk you through opening the provided (and commented!) project and set the left-click option to highlight variables throughout the code.

## Begin code review

1. To begin code review, in the left-hand nav menu, find the `Symbol Tree` area, expand the `Exports` folder, and double-click on `entry`.

This will bring you to the entry point of the builder. We will now bein the code review.

## What Happens First?

1. Note that the `entry` function calls a function, which I've labeled `MainFunction`. Upon return of the function, `ExitProcess(0)` is called.

    When a function returns, the return value of `0` usually indicates success. So even if the overall builder fails, it returns a value of `0`, indicating to the OS that it returned successfully.
    
1. Let's begin review of the actual code. To do so, double-click on the `MainFunction` label to open the main function that does all the work.

## MainFunction Review

1. To begin our review, pay attention to the `Decompilation` window, to the right.
    
    Are we all on the same page, so to say? I will take Qs here to verify that the Decompilation window is showing for everyone. Please note that you can grab the window on the left-hand side to increase the screen real estate of the Decompilation window.

1. At `0x00403b39`, we see a call to `GetCommandLineW`, which will fetch the command line parameters for the process. This is a critical step in the builder, as the options for building are provided on the command line.

1. At `0x00403b8e` we begin looping through arguments. 

1. At `0x00403bbc`, we see the `-pass` parameter being checked.

1. At `0x00403c89` the code checks if a config, public key, and outfile file have been provided.

1. At `0x00403cde` we call what I've labeled the `BuildEXE` function, which begins the building of an `.exe` file.

1. Double-click on `BuildEXE` to see what the function that builds the EXE does. This is where the fun starts.

## EXE Building

You should be at `BuildEXE`, which is at `0x00403204`.
    - If you're lost, simply press the `G` key, enter `0x00403204`, and hit enter. You will now be in lock-step with us.
    
1. At `0x00403257`, the config file is read from disk.

1. At `0x00403287`, the public key file is read from disk.
    - Please note that both the config and key file have been created/set prior to the lab.

## Pulling the template from resources
    
1. At `0x0040342a`, `GetModuleHandleW` is called with a parameter of `0x0`, which mean sit will returns the module handle to the current process -- i.e. the builder itself

1. At `0x00403463`, the EXE template from resource 101 is loaded (65 in decimal).

1. At `0x00403469`, the resource pulled is "locked", which really just means a pointer to it is built.

1. At `0x004034bc`, the resource pulled, which is the EXE encryptor, is copied into a newly-created buffer.

1. At `0x004034da`, the EXE is memory is encoded.

1. At `0x004034f3`, GeneratePasscode() is called.

    Double click this address to enter the function where the fun passcode stuff occurs.

## Generating the passcode    

1. At `0x0040273a`, CreatePasscode() is called. When this returns, the passcode will be in the array argument passed to the function.

    From here, multiple levels of processing occur. We will not have time to delve into these further.
    
## Writing files

1. At `0x00403523`, the output file string is built.

1. At `0x00403543`, the passcode file string is built.

1. At `0x00403582`, the WriteFiles() function is called. This call creates the passcode file.

1. At `0x004035b4`, the WriteFiles() function is called again. This call creates the ransomware binary itself.

# x32dbg Analysis

Let's debug this bad boy!!

1. Double-click `x32dbg` on the Desktop.

1. 