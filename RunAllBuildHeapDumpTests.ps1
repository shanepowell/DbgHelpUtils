Param
(
    [switch] $CheckOnly,
    [switch] $GenerateHeapLogs,
    [switch] $Verbose
)

# run release / x64
Write-Host "Running Release x64 tests"
.\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$false -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose

# run release / x86
Write-Host "Running Release x86 tests"
.\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$false -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose

# run debug / x64
Write-Host "Running Debug x64 tests"
.\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$true -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose

# run debug / x86
Write-Host "Running Debug x86 tests"
.\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$true -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose
