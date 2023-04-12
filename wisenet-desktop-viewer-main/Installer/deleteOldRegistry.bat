@echo on

set path=%path%;%SystemRoot%\System32;

:: This scripts looks for the path to the uninstall program of Wisenet Viewer

:: Loop through all the uninstall entries
for /f "tokens=1 usebackq" %%a in (`reg query "HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall"`) do (
    :: %%a is key for each program
    for /f "usebackq tokens=2*" %%b in (`REG QUERY "%%a" /v "DisplayName"`) do (
        :: %%c is name of each program
        if "%%c"=="Wisenet Viewer" (reg delete %%a /f > nul)
    )
)