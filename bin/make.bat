SolutionGenerator.exe -bp -make
md ..\ide\sandbox\x64\Debug ..\ide\sandbox\x64\Release
copy ..\lib\DXC\dxcompiler.dll ..\ide\sandbox\x64\Release\
copy ..\lib\DXC\dxcompiler.dll ..\ide\sandbox\x64\Debug\
md ..\ide\game\x64\Debug ..\ide\game\x64\Release
copy ..\lib\DXC\dxcompiler.dll ..\ide\game\x64\Release\
copy ..\lib\DXC\dxcompiler.dll ..\ide\game\x64\Debug\