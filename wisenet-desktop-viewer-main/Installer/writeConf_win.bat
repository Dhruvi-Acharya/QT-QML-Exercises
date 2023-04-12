@echo on

set PATH=%~1
set VERSION=%~2
set PRODUCT_UUID=%~3

del /f "%PATH%"
echo %VERSION% >> "%PATH%"
echo %PRODUCT_UUID% >> "%PATH%"
