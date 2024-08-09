# Evastion Capability Development

## Lab Setup

Ubuntu Server: IP: 

1. In the Pluralsight lab environment.  Hold "CTRL+SHFT+ALT" or on Mac "OPT+CMD+SHFT" to open the left menu.  
1. Click the "Pluralsight" Header in the top left.
1. Choose the "Ubuntu" server.  This is where we will do our development.
1. Wait for Ubuntu to come up, if you haven't launched it yet, it may have a black screen for just a moment.
1. In the terminal that pops up, run `cd ~/lab`
1. Then run vscode `code .`


### Problem 1:  Anti-Virus Signature Detection
Anti Virus Essentially uses a list of strings to check statically through binaries and other files to detect strings that it considers malicous.  If you want to check multiple engines at the same time you can run your binaries through virus total, and check on the detections. For this lab we will focus on defender and some yara sigantures that we want to defeat.  File read events trigger antivirus actions on various files.

#### Sample Evil File Detection
1. 
1. 
1. 
1. 



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

