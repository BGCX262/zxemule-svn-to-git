<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: zx - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\TEMP_WIN\RSPF4.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /FR"ARMV4Rel/" /Fo"ARMV4Rel/" /O2 /MC /c 
"D:\Projects\PPC\zx\zx.cpp"
]
Creating command line "clarm.exe @C:\TEMP_WIN\RSPF4.tmp" 
Creating temporary file "C:\TEMP_WIN\RSPF5.tmp" with contents
[
commctrl.lib coredll.lib aygshell.lib gx.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"ARMV4Rel/zx.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Rel/zx.exe" /subsystem:windowsce,4.20 /align:"4096" /MACHINE:ARM 
.\ARMV4Rel\ay.obj
.\ARMV4Rel\StdAfx.obj
.\ARMV4Rel\TrDos.obj
.\ARMV4Rel\wave.obj
.\ARMV4Rel\Z80.obj
.\ARMV4Rel\zx.obj
.\ARMV4Rel\zxfiles.obj
.\ARMV4Rel\zx.res
]
Creating command line "link.exe @C:\TEMP_WIN\RSPF5.tmp"
<h3>Output Window</h3>
Compiling...
zx.cpp
----------------------------------
--- Using Double Screen Buffer ---
----------------------------------
Linking...
Creating command line "bscmake.exe /nologo /o"ARMV4Rel/zx.bsc"  .\ARMV4Rel\StdAfx.sbr .\ARMV4Rel\ay.sbr .\ARMV4Rel\TrDos.sbr .\ARMV4Rel\wave.sbr .\ARMV4Rel\Z80.sbr .\ARMV4Rel\zx.sbr .\ARMV4Rel\zxfiles.sbr"
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
zx.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
