# generate all the dump files used in testing heap layouts
# gflags:
# HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\AllocationSetupTests.exe
# 
# +ust       = GlobalFlag    REG_DWORD 0x1000
# +ust +hpa  = GlobalFlag    REG_DWORD 0x2001000
#            = PageHeapFlags REG_DWORD 0x3
# 
# Enable Segment Heap = FrontEndHeapDebugOptions REG_DWORD 0x08
#

Param
(
    [string] $DumpFolder,
    [ValidateSet("vs2019", "vs2022")][string]$Compiler = "vs2022",
    [string] $ResultFile = "Report.log",
    [switch] $CheckOnly,
    [switch] $GenerateHeapLogs,
    [switch] $TestX86,
    [switch] $TestDebug,
    [switch] $ClearResultsLog,
    [string[]] $Filter,
    [switch] $Verbose,
    [string] $CheckDumpFileBaseName,
    [switch] $CheckDumpHasStackTrace,
    [switch] $SingleDumpOnly
)

Function RunAllAllocationApplicationArgs($options, $validateoptions)
{
    RunAllocationAllocationTypesApplication "lfh" $options $validateoptions
    RunAllocationAllocationTypesApplication "large" $options $validateoptions
    RunAllocationAllocationTypesApplication "sizes" $options $validateoptions
}

Function RunAllocationAllocationTypesApplication($arg, $options, $validateoptions)
{
    RunAllocationX86X64Application $arg "heapalloc" $options $validateoptions
    RunAllocationX86X64Application $arg "malloc" $options $validateoptions
    RunAllocationX86X64Application $arg "new" $options $validateoptions
}

Function RunAllocationX86X64Application($arg, $alloc, $options, $validateoptions)
{
    RunAllocationReleaseDebugApplication $arg "x86" "x86" $alloc $options $validateoptions
    RunAllocationReleaseDebugApplication $arg "x64" "x64" $alloc $options $validateoptions
}

Function RunAllocationReleaseDebugApplication($arg, $arch_dir, $arch, $alloc, $options, $validateoptions)
{
    RunAllocationApplication $arg "debug" $arch_dir $arch $alloc $options $validateoptions
    RunAllocationApplication $arg "release" $arch_dir $arch $alloc $options $validateoptions
}

Function RunAllocationApplication($arg, $config, $arch_dir, $arch, $alloc, $options, $validateoptions)
{
    $base_name = "$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)"
    $dmp_1 = "$DumpFolder\$($base_name)_1.dmp"
    $dmp_2 = "$DumpFolder\$($base_name)_2.dmp"
    $log = "$DumpFolder\$base_name.log"
    $json = "$DumpFolder\$base_name.json"

    if($Filter -and $null -eq ($Filter | Where-Object { $dmp_1 -match $_ -or $dmp_2 -match $_ }))
    {
        return
    }

    if(!$CheckOnly)
    {
        Write-Verbose "Remove all files [$dmp_1] [$dmp_2] [$log] [$json]"
        remove-item $dmp_1 -ErrorAction:SilentlyContinue | Out-Null
        remove-item $dmp_2 -ErrorAction:SilentlyContinue | Out-Null
        remove-item $log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $json -ErrorAction:SilentlyContinue | Out-Null
        Write-Verbose "Run: . `"$ExeFolder\$app_name`" `"--test`" `"$arg`" `"--type`" `"$alloc`" `"--dmp1`" $dmp_1 `"--dmp2`" $dmp_2 `"--log`" $log `"--json`" $json"
        . "$ExeFolder\$app_name" "--test" "$arg" "--type" "$alloc" "--dmp1" $dmp_1 "--dmp2" $dmp_2 "--log" $log "--json" $json
    }
    
    RunAllocationApplicationChecks $validateoptions $base_name
}

Function RunAllocationApplicationChecks($validateoptions, $base_name)
{
    $dmp_1 = "$DumpFolder\$($base_name)_1.dmp"
    $dmp_2 = "$DumpFolder\$($base_name)_2.dmp"
    $json = "$DumpFolder\$base_name.json"

    if($GenerateHeapLogs)
    {
        $dmp_1_full_log = "$DumpFolder\$($base_name)_1_full.log"
        $dmp_1_debug_full_log = "$DumpFolder\$($base_name)_1_debugfull.log"
        $dmp_2_full_diff_log = "$DumpFolder\$($base_name)_2_fulldiff.log"
        $dmp_2_debug_full_diff_log = "$DumpFolder\$($base_name)_2_debugfulldiff.log"

        Write-Verbose "Remove all files [$dmp_1_full_log] [$dmp_1_debug_full_log] [$dmp_2_full_diff_log] [$dmp_2_debug_full_diff_log]"
        remove-item $dmp_1_full_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $dmp_1_debug_full_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $dmp_2_full_diff_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $dmp_2_debug_full_diff_log -ErrorAction:SilentlyContinue | Out-Null

        Write-Verbose "Run: . `"$ExeFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_1 > $dmp_1_full_log"
        . "$ExeFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_1 > $dmp_1_full_log
        Write-Verbose "Run: . `"$ExeFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_1 > $dmp_1_debug_full_log"
        . "$ExeFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_1 > $dmp_1_debug_full_log

        Write-Verbose "Run: . `"$ExeFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $dmp_2_full_diff_log"
        . "$ExeFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $dmp_2_full_diff_log
        Write-Verbose "Run: . `"$ExeFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $dmp_2_debug_full_diff_log"
        . "$ExeFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $dmp_2_debug_full_diff_log
    }
    Write-Verbose "Run: . `"$ExeFolder\ValidateHeapEntries.exe`" `"--dmp1`" $dmp_1 `"--dmp2`" $dmp_2 `"--log`" $ResultFile `"--json`" $json $validateoptions"
    . "$ExeFolder\ValidateHeapEntries.exe" "--dmp1" $dmp_1 "--dmp2" $dmp_2 "--log" $ResultFile "--json" $json $validateoptions
}

Function RunAllocationApplicationSingleDumpChecks($validateoptions, $base_name)
{
    $dmp = "$DumpFolder\$($base_name).dmp"
    $json = "$DumpFolder\$base_name.json"

    if($GenerateHeapLogs)
    {
        $dmp_full_log = "$DumpFolder\$($base_name)_full.log"
        $dmp_debug_full_log = "$DumpFolder\$($base_name)_debugfull.log"

        Write-Verbose "Remove all files [$dmp_full_log] [$dmp_debug_full_log]"
        remove-item $dmp_full_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $dmp_debug_full_log -ErrorAction:SilentlyContinue | Out-Null

        Write-Verbose "Run: . `"$ExeFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp > $dmp_full_log"
        . "$ExeFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp > $dmp_full_log
        Write-Verbose "Run: . `"$ExeFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp > $dmp_debug_full_log"
        . "$ExeFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp > $dmp_debug_full_log
    }
    Write-Verbose "Run: . `"$ExeFolder\ValidateHeapEntries.exe`" `"--dmp1`" $dmp `"--log`" $ResultFile `"--json`" $json $validateoptions"
    . "$ExeFolder\ValidateHeapEntries.exe" "--dmp1" $dmp "--log" $ResultFile "--json" $json $validateoptions
}

Function RunStandardTests()
{
    $app_image_options = "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\AllocationSetupTests.exe"
    $global_flag = "GlobalFlag"
    $page_heap_flags = "PageHeapFlags"
    $front_end_heap = "FrontEndHeapDebugOptions"

    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name -ust -hpa
        Write-Verbose "Clear all gflags"
        Remove-Item $app_image_options -Recurse -ErrorAction:SilentlyContinue | Out-Null
    }
    RunAllAllocationApplicationArgs "" ""
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name +ust -hpa
        Write-Verbose "Set GFlags +ust"
        New-Item $app_image_options | Out-Null
        New-ItemProperty -Path $app_image_options -Name $global_flag -PropertyType DWord -Value 0x1000 | Out-Null
    }
    RunAllAllocationApplicationArgs "_ust" $expected_stacetrace
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name +ust +hpa
        Write-Verbose "Set GFlags +ust +hpa"
        Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2001000 | Out-Null
        New-ItemProperty -Path $app_image_options -Name $page_heap_flags -PropertyType DWord -Value 0x3 | Out-Null
    }
    RunAllAllocationApplicationArgs "_ust_hpa" $expected_stacetrace
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name +hpa
        Write-Verbose "Set GFlags +hpa"
        Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2000000 | Out-Null
    }
    RunAllAllocationApplicationArgs "_hpa" ""
    
    if(!$CheckOnly)
    {
        # enable segment heap type
        Write-Verbose "Set GFlags +segmentheap"
        Remove-ItemProperty -Path $app_image_options -Name $global_flag | Out-Null
        Remove-ItemProperty -Path $app_image_options -Name $page_heap_flags | Out-Null
        New-ItemProperty -Path $app_image_options -Name $front_end_heap -PropertyType DWord -Value 0x08 | Out-Null
    }
    RunAllAllocationApplicationArgs "_segment" ""
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name +ust -hpa
        Write-Verbose "Set GFlags +segmentheap +ust"
        New-ItemProperty -Path $app_image_options -Name $global_flag -PropertyType DWord -Value 0x1000 | Out-Null
    }
    RunAllAllocationApplicationArgs "_segment_ust" $expected_stacetrace
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name +ust +hpa
        Write-Verbose "Set GFlags +segmentheap +ust +hpa"
        Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2001000 | Out-Null
        New-ItemProperty -Path $app_image_options -Name $page_heap_flags -PropertyType DWord -Value 0x3 | Out-Null
    }
    RunAllAllocationApplicationArgs "_segment_ust_hpa" $expected_stacetrace
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name +hpa
        Write-Verbose "Set GFlags +segmentheap +hpa"
        Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2000000 | Out-Null
    }
    RunAllAllocationApplicationArgs "_segment_hpa"
    
    if(!$CheckOnly)
    {
        #. $GFlags /i $app_name -ust -hpa
        Write-Verbose "Clear all gflags"
        Remove-Item $app_image_options -Recurse -ErrorAction:SilentlyContinue | Out-Null
    }
}

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

if($TestDebug)
{
    $ReleaseFolder = "Debug"
}
else
{
    $ReleaseFolder = "Release"
}

if($TestX86)
{
    $BinFolder = "x86"
}
else
{
    $BinFolder = "x64"
}

$compilers = @{
    "vs2019" = "v142";
    "vs2022" = "v143";
}
$CompilerDir = $compilers[$Compiler]

if(!$DumpFolder)
{
    $DumpFolder = "$Compiler$($BinFolder)$($ReleaseFolder)CrashDumps"
}

$ExeFolder = "$PSScriptRoot\$BinFolder\$CompilerDir\$ReleaseFolder"

if (!(Test-Path $DumpFolder))
{
    New-Item $DumpFolder -ItemType Directory | Out-Null
}

if($ClearResultsLog)
{
    Remove-Item $ResultFile -ErrorAction:SilentlyContinue | Out-Null
}

$app_name = "AllocationSetupTests.exe"
$expected_stacetrace = "--stacktrace"

if($CheckDumpFileBaseName)
{
    if($SingleDumpOnly)
    {
        if($CheckDumpHasStackTrace)
        {
            RunAllocationApplicationSingleDumpChecks $expected_stacetrace $CheckDumpFileBaseName 
        }
        else
        {
            RunAllocationApplicationSingleDumpChecks "" $CheckDumpFileBaseName
        }
  
    }
    else
    {
        if($CheckDumpHasStackTrace)
        {
            RunAllocationApplicationChecks $expected_stacetrace $CheckDumpFileBaseName 
        }
        else
        {
            RunAllocationApplicationChecks "" $CheckDumpFileBaseName
        }
    }
}
else
{
    RunStandardTests
}
