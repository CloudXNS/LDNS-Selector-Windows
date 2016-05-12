@echo off
echo ------------------------------------------------  preprare for dns_selector ----------------------------------------------------
cd/d %~dp0 
if not exist ..\third\cpprestsdk\Binaries\ "..\tools\HaoZip\HaoZipC.exe" x ..\third\cpprestsdk\Binaries.7z -o..\third\cpprestsdk\ -y

echo ------------------------------------------------  preprare for dns_selector completed ------------------------------------------