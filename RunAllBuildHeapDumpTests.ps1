Param
(
    [ValidateSet("vs2019", "vs2022")][string]$Compiler = "vs2022",
    [string] $ResultFile,
    [switch] $CheckOnly,
    [switch] $GenerateHeapLogs,
    [switch] $LastReport,
    [switch] $Verbose
)

Function Test-Admin {
    $currentUser = New-Object Security.Principal.WindowsPrincipal $([Security.Principal.WindowsIdentity]::GetCurrent())
    $currentUser.IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)
}

if($Verbose)
{
    $VerbosePreference = "continue"
}

if (!$CheckOnly -and (Test-Admin) -eq $false) 
{
    Write-Host "This script can only be run with Administrator"
    exit
}

if(!$ResultFile)
{
    $compilerResultsFile = @{
        "vs2019" = "ReportVs2019.log";
        "vs2022" = "ReportVs2022.log";
    }
    $ResultFile = $compilerResultsFile[$Compiler]
}

if(!$LastReport)
{
    Remove-Item $ResultFile -ErrorAction:SilentlyContinue | Out-Null

    # run release / x64
    Write-Host "Running $Compiler Release x64 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$false -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler

    # run release / x86
    Write-Host "Running $Compiler Release x86 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$false -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler

    # run debug / x64
    Write-Host "Running $Compiler Debug x64 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$false -TestDebug:$true -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler

    # run debug / x86
    Write-Host "Running $Compiler Debug x86 tests"
    .\RunHeapDumpTests.ps1 -TestX86:$true -TestDebug:$true -CheckOnly:$CheckOnly -GenerateHeapLogs:$GenerateHeapLogs -Verbose:$Verbose -ResultFile:$ResultFile -Compiler:$Compiler
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
