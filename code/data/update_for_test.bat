cd/d %~dp0 
"..\tools\HaoZip\HaoZipC.exe" a res.zip .\res\*
move /y res.zip ..\target\debug\exec\res.zip
if "%1" == "-silent" goto end
..\target\Debug\exec\dns_selector.exe
:end
