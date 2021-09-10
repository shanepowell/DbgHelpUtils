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
    [string] $DumpFolder = "CrashDumps",
    [string] $ResultFile = "CrashDumps\Report.log",
    [switch] $CheckOnly
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
    RunAllocationReleaseDebugApplication $arg "." "x86" $alloc $options $validateoptions
    RunAllocationReleaseDebugApplication $arg "x64" "x64" $alloc $options $validateoptions
}

Function RunAllocationReleaseDebugApplication($arg, $arch_dir, $arch, $alloc, $options, $validateoptions)
{
    RunAllocationApplication $arg "debug" $arch_dir $arch $alloc $options $validateoptions
    RunAllocationApplication $arg "release" $arch_dir $arch $alloc $options $validateoptions
}

Function RunAllocationApplication($arg, $config, $arch_dir, $arch, $alloc, $options, $validateoptions)
{

    $dmp = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).dmp"
    $log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).log"
    $json = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).json"
    if(!$CheckOnly)
    {
        remove-item $dmp -ErrorAction:SilentlyContinue | Out-Null
        remove-item $log -ErrorAction:SilentlyContinue | Out-Null
        remove-item $json -ErrorAction:SilentlyContinue | Out-Null
        . "$PSScriptRoot\$arch_dir\$config\$app_name" "--$arg" "--use$alloc" "--dmp" $dmp "--log" $log "--json" $json
    }
    . "$PSScriptRoot\x64\Release\ValidateHeapEntries.exe" "--dmp" $dmp "--log" $ResultFile "--json" $json $validateoptions
}

function Test-Admin {
    $currentUser = New-Object Security.Principal.WindowsPrincipal $([Security.Principal.WindowsIdentity]::GetCurrent())
    $currentUser.IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)
}

if ((Test-Admin) -eq $false) 
{
    Write-Host "This script can only be run with Administrator"
    exit
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
    Remove-Item $app_image_options -Recurse -ErrorAction:SilentlyContinue | Out-Null
}
RunAllAllocationApplicationArgs "" ""

if(!$CheckOnly)
{
    #. $GFlags /i $app_name +ust -hpa
    New-Item $app_image_options | Out-Null
    New-ItemProperty -Path $app_image_options -Name $global_flag -PropertyType DWord -Value 0x1000 | Out-Null
}
RunAllAllocationApplicationArgs "_ust" $expected_stacetrace

if(!$CheckOnly)
{
    #. $GFlags /i $app_name +ust +hpa
    Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2001000 | Out-Null
    New-ItemProperty -Path $app_image_options -Name $page_heap_flags -PropertyType DWord -Value 0x3 | Out-Null
}
RunAllAllocationApplicationArgs "_ust_hpa" $expected_stacetrace

if(!$CheckOnly)
{
    #. $GFlags /i $app_name +hpa
    Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2000000 | Out-Null
}
RunAllAllocationApplicationArgs "_hpa" ""

if(!$CheckOnly)
{
    # enable segment heap type
    Remove-ItemProperty -Path $app_image_options -Name $global_flag | Out-Null
    Remove-ItemProperty -Path $app_image_options -Name $page_heap_flags | Out-Null
    New-ItemProperty -Path $app_image_options -Name $front_end_heap -PropertyType DWord -Value 0x08 | Out-Null
}
RunAllAllocationApplicationArgs "_segment" ""

if(!$CheckOnly)
{
    #. $GFlags /i $app_name +ust -hpa
    New-ItemProperty -Path $app_image_options -Name $global_flag -PropertyType DWord -Value 0x1000 | Out-Null
}
RunAllAllocationApplicationArgs "_segment_ust" $expected_stacetrace

if(!$CheckOnly)
{
    #. $GFlags /i $app_name +ust +hpa
    Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2001000 | Out-Null
    New-ItemProperty -Path $app_image_options -Name $page_heap_flags -PropertyType DWord -Value 0x3 | Out-Null
}
RunAllAllocationApplicationArgs "_segment_ust_hpa" $expected_stacetrace

if(!$CheckOnly)
{
    #. $GFlags /i $app_name +hpa
    Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2000000 | Out-Null
}
RunAllAllocationApplicationArgs "_segment_hpa"

if(!$CheckOnly)
{
    #. $GFlags /i $app_name -ust -hpa
    Remove-Item $app_image_options -Recurse -ErrorAction:SilentlyContinue | Out-Null
}
