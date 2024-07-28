# Ghidra Setup

In this section, we're going to open and configure as required for the workshop.

1. Double-click `ghidraRun.bat` on the desktop
 - Once Ghidra opens, you'll be at the Project window
2. Open the provided Ghidra project by choosing `File > Open Project`. Navigate to `Home > Documents > ghidra_projects > lb3builder.gpr` and select to open the project.
3. Double-click on the `builder.exe` file in the recently-opened project.
4. Select the `Edit` menu and select `Tool Options`
5. From the left-hand nav menu, expand `Listing Fields`
6. Choose `Cursor text highglith` in this menu
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

1. 