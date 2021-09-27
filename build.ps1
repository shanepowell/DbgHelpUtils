Param
(
    [ValidateSet("vs2019", "vs2022")]
    [string]$Compiler = "vs2022",
    [switch]$GenerateBuildFileOnly
)

$compilers = @{
    "vs2019" = "Visual Studio 16 2019";
    "vs2022" = "Visual Studio 17 2022"
 }

Write-Host Generate build files
cmake -S .  -B build/$($Compiler)x64 -G $compilers[$Compiler] -A x64
cmake -S .  -B build/$($Compiler)x86 -G $compilers[$Compiler] -A win32

if(!$GenerateBuildFileOnly)
{
    Write-Host Build all targets
    cmake --build .\build\$($Compiler)x64 --config Debug
    cmake --build .\build\$($Compiler)x64 --config Release

    cmake --build .\build\$($Compiler)x86 --config Debug
    cmake --build .\build\$($Compiler)x86 --config Release
}

Write-Host All done