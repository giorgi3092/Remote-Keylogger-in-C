Compiling the Keylogger
1.	Open Visual Studio and press Ctrl+Shift+n to create a new project. Choose “Empty Project” and choose a name of your choice.
2.	Press Ctrl+Shift+A to create a new file and choose a name of your choice, for example, “main.c”. This file will contain the actual program, the keylogger which runs on Windows machine. 
 
Figure 1: creating a file in Visual Studio
3.	Download the main.c program from my project documents, copy the contents and paste in the Visual Studio file that you created in the previous step.
4.	Press Ctrl+Shift+B to build the program.
 
Figure 2: This is what the output of Visual Studio should look like.
Now, the keylogger is compiled and .exe binary file is in your projects folder. Find your Visual Studio projects folder and your .exe binary file will be in the “debug” folder of your project.
Now, we need to compile the server.c file (which is in my project files list) on the server.

Compiling the Server Code
1)	Open PuTTY program and enter “159.89.26.242” IP address. Leave the port to 22 as default. For login, enter “tester” and for password enter “Testing789”. You should be able to log in to the server now.
 
 
Figure 3: This is what PuTTY screen should look like

2)	Now, run the commands in order.
3)	Copy “rm *” and press enter to remove any files from the present working directory.
4)	Type “nano server.c” to create and open file server.c where we will place the server C code.
 
5)	Now, open the server.c file from my project files list, copy the contents and copy in this window. Normally, copying is done by placing mouse the cursor on putty window and pressing Right Mouse Button and Left Mouse Button at the same time. Next, to save the file with the code, press Ctrl+X, press “Y” button and hit enter. 
6)	To compile the program type “gcc server.c” in the command line.
7)	To run the logging server and pass the port run the following “./a.out 8888”
8)	Now the server is running in Live Mode and listening for connections. It is our program that will connect and send information to the server and the server will write those characters in a .txt(although not requires because everything is a file in Unix) file locally.
 
Now, that we have server running, DO NOT close PuTTY and open the Keylogger .exe binary executable from the Visual Studio project folder that we created. The program will open and disappear immediately so that it is unnoticeable while running. 
Now type whatever you want, and it will be written locally into the “log.txt” file in the “debug” folder of your project where you launched the keylogger, that is, in the same directory.
What is more interesting is that it will also be logged on the server, in the same directory where we ran “./a.out 8888” command. Meanwhile, you can locate the keylogger in Task Manager processes. Select it and hit “End task”.
Now, we can cancel the server script too. Switch to PuTTY window and press Ctrl+C a few times to terminate the server.c program (compiled as “a.out”). Type “less log.txt” in the command line and hit Enter. You will be able to see exactly what you typed in the testing process, that is, when keylogger was running. Sometimes packets are lost though. That is because TCP protocol is connection based protocol not message based. So, it ensures there is an uninterruptible connection between the keylogger and the server, but messages, that is, packets can be lost, especially on slow internet.
This is what my text file on the server looked like while I was typing this! All the keys are documented on https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx . For example, [LMB] means that a person pressed Left Mouse Button (LMB). [LAK] means Left Arrow Key and so on… Backspace is logged as “[BACKSPACE]” because the program cannot tell which text a user is erasing. So, only the log file viewer (a human being) can discern, by context, which part of which text was erased. 
 
