# Ghidra Setup

In this section, we're going to open and configure as required for the workshop.

1. Double-click `ghidraRun.bat` on the desktop
 - Once Ghidra opens, you'll be at the Project window
2. Open the provided Ghidra project by choosing `File > Open Project`. Navigate to `Home > Documents > ghidra_projects > lb3builder.gpr` and select to open the project.
3. Double-click on the `builder.exe` file in the recently-opened project.
4. Select the `Edit` menu and select `Tool Options`
5. From the left-hand nav menu, expand `Listing Fields`
6. Choose `Cursor text highlight` in this menu
7. Change the `Mouse button the activate` from `MIDDLE` -> `LEFT`
8. Press `OK` to save your settings

    The above steps walk you through opening the provided (and commented!) project and set the left-click option to highlight variables throughout the code.

# Begin code review

1. To begin code review, in the left-hand nav menu, find the `Symbol Tree` area, expand the `Exports` folder, and double-click on `entry`.

This will bring you to the entry point of the builder. We will now bein the code review.

# What Happens First?

1. Note that the `entry` function calls a function, which I've labeled `MainFunction`. Upon return of the function, `ExitProcess(0)` is called.

    When a function returns, the return value of `0` usually indicates success. So even if the overall builder fails, it returns a value of `0`, indicating to the OS that it returned successfully.
    
2. Let's begin review of the actual code. To do so, double-click on the `MainFunction` label to open the main function that does all the work.

# MainFunction Review

1. To begin our review, pay attention to the `Decompilation` window, to the right.
    - Are we all on the same page, so to say? I will take Qs here to verify that the Decompilation window is showing for everyone. Please note that you can grab the window on the left-hand side to increase the screen real estate of the Decompilation window.

2. At `00403b39`, we see a call to `GetCommandLineW`, which will fetch the command line parameters for the process. This is a critical step in the builder, as the options for building are provided on the command line.

3. At `00403b8e` we begin looping through arguments. 

4. At `00403c89` the code checks if a config, public key, and outfile file have been provided.

5. At `00403cde` we call what I've labeled the `BuildEXE` function, which begins the building of an `.exe` file.

6. Double-click on `BuildEXE` to see what the function that builds the EXE does. This is where the fun starts.

# EXE Building

You should be at `BuildEXE`, which is at `00403204`.

    - If you're lost, simply press the `G` key, enter `00403204`, and hit enter. You will now be in lock-step with us.
    
1. At `00403257`, the config file is read from disk.

2. At `00403287`, the public key file is read from disk.

    - Please note that both the config and key file have been created/set prior to the lab.

# Pulling the template from resources
    
1. At `0040342a`, `GetModuleHandleW` is called with a parameter of `0x0`, which mean sit will returns the module handle to the current process -- i.e. the builder itself

2. 