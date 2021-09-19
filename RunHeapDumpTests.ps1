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
    [string] $ResultFile,
    [switch] $CheckOnly,
    [switch] $GenerateHeapLogs,
    [switch] $TestX86,
    [switch] $TestDebug,
    [switch] $Verbose
)

Function RunAllAllocationApplicationArgs($options, $validateoptions)
{
    RunAllocationAllocationTypesApplication "lfh" $options $validateoptions
    RunAllocationAllocationTypesApplication "virtual" $options $validateoptions
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

    $dmp_1 = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)_1.dmp"
    $dmp_2 = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)_2.dmp"
    $log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).log"
    $json = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).json"
    if(!$CheckOnly)
    {
        Write-Verbose "Remove all files [$dmp_1] [$dmp_2] [$log] [$json]"
        remove-item $dmp_1 -ErrorAction:SilentlyContinue | Out-Null
        remove-item $dmp_2 -ErrorAction:SilentlyContinue | Out-Null
        remove-item $log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $json -ErrorAction:SilentlyContinue | Out-Null
        Write-Verbose "Run: . `"$PSScriptRoot\$arch_dir\$config\$app_name`" `"--test`" `"$arg`" `"--type`" `"$alloc`" `"--dmp1`" $dmp_1 `"--dmp2`" $dmp_2 `"--log`" $log `"--json`" $json"
        . "$PSScriptRoot\$arch_dir\$config\$app_name" "--test" "$arg" "--type" "$alloc" "--dmp1" $dmp_1 "--dmp2" $dmp_2 "--log" $log "--json" $json
    }

    if($GenerateHeapLogs)
    {
        $full_log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)_full.log"
        $debug_full_log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)_debugfull.log"
        $full_diff_log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)_fulldiff.log"
        $debug_full_diff_log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options)_debugfulldiff.log"

        Write-Verbose "Remove all files [$full_log] [$debug_full_log] [$full_diff_log] [$debug_full_diff_log]"
        remove-item $full_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $debug_full_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $full_diff_log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $debug_full_diff_log -ErrorAction:SilentlyContinue | Out-Null

        Write-Verbose "Run: . `"$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_1 > $full_log"
        . "$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_1 > $full_log
        Write-Verbose "Run: . `"$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_1 > $debug_full_log"
        . "$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_1 > $debug_full_log

        Write-Verbose "Run: . `"$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $full_diff_log"
        . "$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapstat all --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $full_diff_log
        Write-Verbose "Run: . `"$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe`" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $debug_full_diff_log"
        . "$PSScriptRoot\$BinFolder\$ReleaseFolder\MiniDumper.exe" --heap --crtheap --heapentries --heapdebug --heapstat all --symbols --dumpfile $dmp_2 --basediffdumpfile $dmp_1 > $debug_full_diff_log
    }
    Write-Verbose "Run: . `"$PSScriptRoot\$BinFolder\$ReleaseFolder\ValidateHeapEntries.exe`" `"--dmp1`" $dmp_1 `"--dmp2`" $dmp_2 `"--log`" $ResultFile `"--json`" $json $validateoptions"
    . "$PSScriptRoot\$BinFolder\$ReleaseFolder\ValidateHeapEntries.exe" "--dmp1" $dmp_1 "--dmp2" $dmp_2 "--log" $ResultFile "--json" $json $validateoptions
}

function Test-Admin {
    $currentUser = New-Object Security.Principal.WindowsPrincipal $([Security.Principal.WindowsIdentity]::GetCurrent())
    $currentUser.IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)
}

if($Verbose)
{
    $VerbosePreference = "continue"
}

if ((Test-Admin) -eq $false) 
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

if(!$DumpFolder)
{
    $DumpFolder = "$($BinFolder)$($ReleaseFolder)CrashDumps"
}

if(!$ResultFile)
{
    $ResultFile = "$($DumpFolder)\Report.log"
}

if (!(Test-Path $DumpFolder))
{
    New-Item $DumpFolder -ItemType Directory | Out-Null
}

Remove-Item $ResultFile -ErrorAction:SilentlyContinue | Out-Null

$app_name = "AllocationSetupTests.exe"
$app_image_options = "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\AllocationSetupTests.exe"
$global_flag = "GlobalFlag"
$page_heap_flags = "PageHeapFlags"
$front_end_heap = "FrontEndHeapDebugOptions"
$expected_stacetrace = "--stacktrace"

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
