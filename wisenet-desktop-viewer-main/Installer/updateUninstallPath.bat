@echo on

set path=%path%;%SystemRoot%\System32;

set PRODUCT_UUID=%~1
set REG_KEY=HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\%PRODUCT_UUID%

:: This scripts looks for the path to the uninstall program of Wisenet Viewer
for /f "usebackq tokens=2*" %%a in (`REG QUERY %REG_KEY% /v "UninstallString"`) do (
            :: %%b is the uninstaller path. add quotes before and after the path.
            REG ADD %REG_KEY% /v "UninstallString" /d "\"%%~b\"" /f > nul)
        )
    )
)
