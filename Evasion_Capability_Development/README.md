# Evastion Capability Development

## Lab Setup

Ubuntu Desktop:
Windows Desktop:

1. In the Pluralsight lab environment.  Hold "CTRL+SHFT+ALT" or on Mac "OPT+CMD+SHFT" to open the left menu.  
1. Click the "Pluralsight" Header in the top left.
1. Choose the "Ubuntu" server.  This is where we will do our development.
1. Wait for Ubuntu to come up, if you haven't launched it yet, it may have a black screen for just a moment.
1. In the terminal that pops up, run `cd ~/lab`
1. Then run vscode `code .`





### Problem 1:  Anti-Virus Signature Detection
Anti Virus Essentially uses a list of strings to check statically through binaries and other files to detect strings that it considers malicous.  If you want to check multiple engines at the same time you can run your binaries through virus total, and check on the detections. For this lab we will focus on defender and some yara sigantures that we want to defeat.  File read events trigger antivirus actions on various files.

#### Sample Evil File Detection
1. Open up the **Windows Desktop**
1. Right click, & run as administrator cmd shortcut on the Desktop.
1. In the CMD console, switch directories to the LAB_FILES\Evasion_Capability directory. `cd c:\Users\Public\Desktop\LAB_FILES\Evasion_Capability_Development`
1. Now copy the file "evil1.exe" to another folder.  



### Problem 2: EDR & RE Basic detections at Runtime
Anti-Virus is one thing, but the next issue for malware arises at runtime. The reason for this is that the event of 

#### Runtime Detection!
Strings in memory! OH noes

1. 
1. 
1.
1.



### Problem 3: EDR Hooking API Calls
EDR hooks calls to windows api. Think crowdstrike kernel level access ;).

1. 
1.
1.
1.

####
1. 

