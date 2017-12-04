# CH_SERVER
For coding test
Chat server program in C using queue to save the information of clients and socket communication.
This program icludes function of private chat,test, rename of the client nickname(initially, 
it is a simple integer such as 1,2, or 3), and the list of active client.
How to compile in current OS X:


Server side:
1.
Initiate terminal.

2.
Go to the directory where you save this program.
command i.e) cd Downloads/chat_server_program.c

3. 
Compile this program. Type 
"gcc -Wall -Werror chatserver_program.c -O2 -lpthread -o chat" 
and hit enter.
Change term "chatserver_program.c" to the name of the program if you saved its name differently.
If gcc hasn't been installed yet, please go "https://solarianprogrammer.com/2017/05/21/compiling-gcc-macos/ "
     and follow the instruction.

4.
Type 
"./chat" 
and hit enter to initiate a chat server.

Client side:
1. 
if you already have telnet in your system, type in terminal,　
"telnet <IP address of chat server>  2525" 
and hit enter. 2525 is the port number I initially set in the code. You can modify easily if you want.

　　・If you do not have an telnet, please go "https://www.wikihow.com/Use-Telnet-on-Mac-OS-X"
   　　and follow instruction.

　　・If you do not know the IP address of the chat server, please go to the terminal where you will test the                     'sever' program. Please type 
      　"ipconfig getifaddr en0" 
      　before starting the chat server program. You should see the address of that terminal, which is the IP                        address of the chat server.

2. 
Enjoy. There are several commands you can use other than normal chat.
\HELP                                                 Show help
\TEST                                                 Server test: If you succeed, the system will reply "test success".
\RENAME <name>                                        Change nickname: Type the nickname you would like instead of<name>. 
\PRIVATE  <reference> <message>                       Send private message: Type the ID number and message
                                                      you want to send instead  of <reference> <message>.
\ACTIVE                                               Show the list of active clients
\QUIT                                                 Quit chatroom
