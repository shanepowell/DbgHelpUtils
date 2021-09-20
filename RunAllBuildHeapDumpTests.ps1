Param
(
    [string] $ResultFile = "Report.log",
    [switch] $CheckOnly,
    [switch] $GenerateHeapLogs,
    [switch] $LastReport,
    [switch] $Verbose
)

if(!$LastReport)
{
    Remove-Item $ResultFile -ErrorAction:SilentlyContinue | Out-Null

    # run release / x64
    Write-Host "Running Release x64 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$false -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile

    # run release / x86
    Write-Host "Running Release x86 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$false -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile

    # run debug / x64
    Write-Host "Running Debug x64 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$true -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile

    # run debug / x86
    Write-Host "Running Debug x86 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$true -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile
}


$errorLines = ""
$log = Get-Content $ResultFile
foreach ($line in $log)
{ 
    if ($line -like "ERROR:*")
    {
        if($errorLines)
        {
            $errorLines += "`n"
        }
        $errorLines += $line
    }
}

if($errorLines)
{
    Write-Host "Complete with Errors:"
    Write-Host $errorLines
}
else
{
    Write-Host "Successful Complete"
}
