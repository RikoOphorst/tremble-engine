# tremble-engine
This is a project I worked on during my second year at NHTV. I led the team of 8 people who worked on it (all programmers). It is supposed to be a multiplayer FPS engine, but unfortunately it turned out more to be a rendering demo than anything else. All rendering work on this engine was done by me, except for the shadow rendering (which was done by another programmer).

# Compiling tremble
In order to compile, there are some dependencies that you need to download.

1. Go to http://dependencies.rikoophorst.com/tremble/ and download the tremble-engine.rar file.
2. It contains two folders: bin & dependencies. Both of these folders are important.
3. Unpack the tremble-engine.rar to the root folder of the project.
4. The "bin" and "dependencies" folder should be in the same folder as the .sln for the project.
5. Open the .sln (in Visual Studio 2015, preferably!) after you have downloaded & unpacked the dependencies rar file.
6. Hit CTRL+SHIFT+B to compile the project.
7. Hit F5 to run the project.

Sidenote: the "bin" folder that you download already contains a Debug and Release executable of the engine. You should be able to run either of them.

# Running the engine in multiplayer mode
Running the engine in multiplayer mode is a two step process. You need to launch one app as the host, and another as a client.

## Starting the host (DO THIS FIRST!)
1. Start an instance of the game. It is located at `[project_root]/bin/x64/[Release|Debug]/game.exe`.
2. You will be prompted with a settings window before the game actually launches.
3. Set `Scene` to `Multiplayer_Test\MultiplayerTestEnvironment.tmap`.
4. Set `Load Scene` to true/enabled.
5. Set `God Mode` to false/disabled.
6. Set `Frustum culling` to true/enabled.
7. Set `Depth Pre-Pass` to true/enabled.
8. Set `Physics base plane` to false/disabled.
9. Set `D3D12 debug layer` to false/disabled.
11. Set `Light demo` to false/disabled.
12. Set `Spawn clients` to false/disabled.
13. **Set `Host` to true/enabled.**
14. All other options that weren't mentioned either don't do anything (and thus can be ignored) or the functionality of them is implied.
15. Hit `OK` after configuring the settings, then move on to the second part of launching the game (right under this!).

## Starting the client (DO THIS SECOND!)
1. Start an instance of the game. It is located at `[project_root]/bin/x64/[Release|Debug]/game.exe`.
2. You will be prompted with a settings window before the game actually launches.
3. Set `Scene` to `Multiplayer_Test\MultiplayerTestEnvironment.tmap`.
4. Set `Load Scene` to true/enabled.
5. Set `God Mode` to false/disabled.
6. Set `Frustum culling` to true/enabled.
7. Set `Depth Pre-Pass` to true/enabled.
8. Set `Physics base plane` to false/disabled.
9. Set `D3D12 debug layer` to false/disabled.
11. Set `Light demo` to false/disabled.
12. Set `Spawn clients` to false/disabled.
13. **Set `Host` to false/disabled.**
14. **Set `Host IP address` to `127.0.0.1` or whatever the EXTERNAL IP address of the host is.**
15. All other options that weren't mentioned either don't do anything (and thus can be ignored) or the functionality of them is implied.
16. Hit `OK` after configuring the settings, then move on to the third part of launching the game (right under this!).

## Starting the actual game
1. So now you should have two instances of the game running: one as host, and one as client.
2. The client upon launch should immediately connect to the host. You can know that connection was succesful if a map was loaded in the client instance (i.e. no empty blue backbuffer).
3. If the connection was unsuccessful, RIP. Good luck troubleshooting, my friend.
4. If the connection was successful, continue on.
5. Identify which of the two game instances is the host. You can know that because in the host instance, there is a text on screen that literally says "HOST".
6. In this host instance, hit the ENTER key.
7. Congratulations, you can now start playing the game.

## Some gameplay instructions.
Movement is bound to the WASD keys.
You have 4 weapons: a pistol, an assault rifle, a raygun and a shotgun. You can switch weapons by scrolling the mouse, or pressing the number keys (1, 2, 3, 4).
You can throw grenades by pressing F. Warning: lots of grenades will crash the game.
On the map, there are reddish colored planes. If you right click on them, you throw out a grapple hook and get slung towards there.

Gameplay is extremely buggy, so you might have to restart the application a few times before you can get a stable play session.
