@echo off
bitsadmin.exe /transfer DownloadTrembleDependencies /download /priority normal http://dependencies.rikoophorst.com/tremble-bin.rar %~dp0/tremble-bin.rar
bitsadmin.exe /transfer DownloadTrembleDependencies /download /priority normal http://dependencies.rikoophorst.com/tremble-dependencies.rar %~dp0/tremble-dependencies.rar
echo rars downloaded
If exist "C:\Program Files\WinRAR\WinRAR.exe" (
echo winrar is in the program files folder
CD "C:\Program Files\WinRAR"
unrar x %~dp0/tremble-bin.rar "%~dp0"
unrar x %~dp0/tremble-dependencies.rar "%~dp0"
) else (
If exist "D:\Programs\WinRAR\WinRAR.exe" (
echo winrar is in the d/programs folder
CD "D:\Programs\WinRAR"
unrar x %~dp0/tremble-bin.rar "%~dp0"
unrar x %~dp0/tremble-dependencies.rar "%~dp0"
)
 else (echo install winrar, you scrub. If not, just unrar this archive yourself in the directory that it's in. Have fun!)
)