Param
(
    [ValidateSet("vs2019", "vs2022")][string]$Compiler = "vs2022",
    [string] $ResultFile = "failed\Report.log",
    [switch] $GenerateHeapLogs,
    [switch] $LastReport,
    [string[]] $Filter,
    [switch] $Verbose
)

if(!$LastReport)
{
    Remove-Item $ResultFile -ErrorAction:SilentlyContinue | Out-Null
    $dmps = Get-ChildItem failed\*.dmp -Recurse
    foreach ($dmp in $dmps)
    {
        $CheckDumpHasStackTrace = $dmp.BaseName -match '_ust'

        if($Filter -and $null -eq ($Filter | Where-Object { $dmp -match $_ }))
        {
            continue
        }
    
        if($dmp.BaseName -match '_2$')
        {
            # ignore... as the _1 will be processed...
        }
        elseif ($dmp.BaseName -match '_1$')
        {
            $name = $dmp.BaseName.SubString(0, $dmp.BaseName.Length - 2)
            # run release / x64
            Write-Host "Running $Compiler Release x64 test on set $name"
            .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$false -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $name -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -DumpFolder $dmp.Directory

            # run release / x86
            Write-Host "Running $Compiler Release x86 tests on set $name"
            .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$false -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $name -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -DumpFolder $dmp.Directory

            # run debug / x64
            Write-Host "Running $Compiler Debug x64 tests on set $name"
            .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$true -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $name -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -DumpFolder $dmp.Directory

            # run debug / x86
            Write-Host "Running $Compiler Debug x86 tests on set $name"
            .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$true -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $name -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -DumpFolder $dmp.Directory
        }
        else
        {
            # run release / x64
            Write-Host "Running $Compiler Release x64 test on single $dmp"
            .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$false -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $dmp.BaseName -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -SingleDumpOnly -DumpFolder $dmp.Directory

            # run release / x86
            Write-Host "Running $Compiler Release x86 tests on single $dmp"
            .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$false -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $dmp.BaseName -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -SingleDumpOnly -DumpFolder $dmp.Directory

            # run debug / x64
            Write-Host "Running $Compiler Debug x64 tests on single $dmp"
            .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$true -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $dmp.BaseName -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -SingleDumpOnly -DumpFolder $dmp.Directory

            # run debug / x86
            Write-Host "Running $Compiler Debug x86 tests on single $dmp"
            .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$true -CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler -CheckDumpFileBaseName $dmp.BaseName -CheckDumpHasStackTrace:$CheckDumpHasStackTrace -SingleDumpOnly -DumpFolder $dmp.Directory
        }
    }
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
