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

---
1. In Powershell run `get-mpthreat`.

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
1. Once there run `expand-archive`. **Make Sure Defender is Still Off**
1. Now run 
1. ?Take learner back to ubuntu to change the stuff themselves/show them?
1. Run `get-mpthreat` to identify the detections that defender made on the dumplsass file.
1. That file has been identified and deleted, so now let's use the one that was created with the modifications that matched the yara signatures on the slides.
1. 

> 


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

