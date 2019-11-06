@author		Hamza Mian, Brandon Becker
@project	Project 2 (Network Programming) Chat Application

Build/Run Instructions:
 - Open the solution in Visual Studio
 - Make sure the project properties contain the correct Platform Toolset and Windows SDK Version
 - Select the configuration (Release/Debug) (x86/x64)
 - Build the solution
 - Run ChatServer.exe from the output folder
 - Run ChatClient.exe to connect to the server (you can run as many instances as you want)

Client Instructions:
 - Start by typing your name when asked
 - Once the Chat Client is connected to the server, you can join any room
	- /join [room_name] [ENTER_KEY]
 - You can also leave any room
	- /leave [room_name] [ENTER_KEY]
 - You can send message to all your joined rooms
	- [message] [ENTER_KEY]
 - You can disconnect and exit the application by pressing Ctrl + C