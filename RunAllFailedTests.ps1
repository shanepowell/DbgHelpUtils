Param
(
    [ValidateSet("vs2019", "vs2022")][string]$Compiler = "vs2022",
    [string] $ResultFile = "failed\Report.log",
    [switch] $GenerateHeapLogs,
    [switch] $LastReport,
    [string[]] $Filter,
    [int] $ConcurrentLimit = 0,
    [switch] $Verbose
)

if($Verbose)
{
    $VerbosePreference = "continue"
}

if ($ConcurrentLimit -le 0)
{
    $ConcurrentLimit = $env:NUMBER_OF_PROCESSORS
}

$engine = Get-Process -id $pid | Get-Item

Function GenerateTempReportFile($tempReportFiles)
{
    $tempFile = New-TemporaryFile
    $tempReportFiles.Add($tempFile)
    return $tempFile
}

Function RunTest($jobs, $tempReportFiles, $testArguments)
{
    if($ConcurrentLimit -eq 1)
    {
        $tempFile = GenerateTempReportFile $tempReportFiles
        Write-Verbose "Command: .\RunHeapDumpTests.ps1 $testArguments -ResultFile:`"$($tempFile.FullName)`" -ConcurrentLimit:1"
        Invoke-Expression -Command ".\RunHeapDumpTests.ps1 $testArguments -ResultFile:`"$($tempFile.FullName)`" -ConcurrentLimit:1"
        return
    }

    # waiting for a job to complete to start another job
    if(($jobs | Where-Object -FilterScript { -not $_.HasExited }).Count -ge $ConcurrentLimit)
    {
        Write-Verbose "Waiting to start job"
        while(($jobs | Where-Object -FilterScript { -not $_.HasExited }).Count -ge $ConcurrentLimit)
        {
            Start-Sleep -Milliseconds 500
        }
        Write-Verbose "Wait complete"
    }

    $tempFile = GenerateTempReportFile $tempReportFiles
    Write-Verbose "Command: $($engine.FullName) -NoLogo -NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File .\RunHeapDumpTests.ps1 $testArguments -ResultFile:`"$($tempFile.FullName)`""
    $arguments = "-NoLogo -NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File .\RunHeapDumpTests.ps1 $testArguments -ResultFile:`"$($tempFile.FullName)`""

    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = $engine.FullName
    $pinfo.UseShellExecute = $false
    $pinfo.Arguments = $arguments
    $pinfo.WorkingDirectory = Get-Location
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null

    $jobs.Add($p)
    Write-Verbose "Started job [$($p.Id)]"
}

Function RunTests($jobs, $tempReportFiles, $type, $name, $baseArguments)
{
    # run release / x64
    Write-Host "Running $Compiler Release x64 test on $type $name"
    $rv = RunTest $jobs $tempReportFiles "-TestX86:`$false -TestDebug:`$false $baseArguments"

    # run release / x86
    Write-Host "Running $Compiler Release x86 tests on $type $name"
    $rv = RunTest $jobs $tempReportFiles "-TestX86:`$true -TestDebug:`$false $baseArguments"

    # run debug / x64
    Write-Host "Running $Compiler Debug x64 tests on $type $name"
    $rv = RunTest $jobs $tempReportFiles "-TestX86:`$false -TestDebug:`$true $baseArguments"

    # run debug / x86
    Write-Host "Running $Compiler Debug x86 tests on $type $name"
    $rv = RunTest $jobs $tempReportFiles "-TestX86:`$true -TestDebug:`$true $baseArguments"
}

if(!$LastReport)
{
    Remove-Item $ResultFile -ErrorAction:SilentlyContinue | Out-Null
    $dmps = Get-ChildItem failed\*.dmp -Recurse
    $tempReportFiles = New-Object System.Collections.ArrayList
    $jobs = New-Object System.Collections.ArrayList
    foreach ($dmp in $dmps)
    {
        $CheckDumpHasStackTrace = $dmp.BaseName -match '_ust'
        $SkipFakeOffsetCheck = Test-Path -path ("$($dmp.DirectoryName)\skip_fake_offset_check.txt") -PathType Leaf

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
            $baseArguments = "-CheckOnly -GenerateHeapLogs:`$$GenerateHeapLogs -Verbose:`$$Verbose -Compiler:$Compiler -CheckDumpFileBaseName `"$name`" -CheckDumpHasStackTrace:`$$CheckDumpHasStackTrace -SkipFakeOffsetCheck:`$$SkipFakeOffsetCheck -DumpFolder `"$($dmp.Directory)`" -ConcurrentLimit:$ConcurrentLimit"

            RunTests $jobs $tempReportFiles 'set' $name $baseArguments
        }
        else
        {
            $baseArguments = "-CheckOnly -GenerateHeapLogs:`$$GenerateHeapLogs -Verbose:`$$Verbose -Compiler:$Compiler -CheckDumpFileBaseName `"$($dmp.BaseName)`" -CheckDumpHasStackTrace:`$$CheckDumpHasStackTrace -SkipFakeOffsetCheck:`$$SkipFakeOffsetCheck -SingleDumpOnly -DumpFolder `"$($dmp.Directory)`" -ConcurrentLimit:$ConcurrentLimit"
            RunTests $jobs $tempReportFiles 'single' $dmp $baseArguments
        }
    }

    Write-Verbose "Waiting for all jobs to complete"
    # wait for all jobs
    foreach($p in $jobs)
    {
        $p.WaitForExit()
    }

    Write-Verbose "Combining report output"

    # Combine result files
    foreach ($reportFile in $tempReportFiles)
    {
        if(Test-Path -Path $reportFile -PathType Leaf)
        {
            Write-Verbose "Append $reportFile to $ResultFile"
            Get-Content $reportFile | Add-Content $ResultFile
        } else {
            Write-Error "Report File $reportFile doesn't exist!"
        }
    }
    
    # wait for the above to complete it's file handling as the file may still be locked when we get to the remove temp files stage...
    #Start-Sleep -Seconds 5
    
    #$tempReportFiles | Remove-Item
}

Write-Verbose "Reading report output"

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
    Write-Host "Completed with Errors"
}
else
{
    Write-Host "Successful Complete"
}
