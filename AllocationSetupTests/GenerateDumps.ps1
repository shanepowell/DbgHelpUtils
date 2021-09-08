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
    [Parameter(Mandatory=$true)][string] $DumpFolder,
    [Parameter(Mandatory=$true)][string] $ResultFile
)

Function Run-AllAllocationApplicationArgs($options)
{
    Run-AllocationAllocationTypesApplication "lfh" $options
    Run-AllocationAllocationTypesApplication "virtual" $options
    Run-AllocationAllocationTypesApplication "sizes" $options
}

Function Run-AllocationAllocationTypesApplication($arg, $options)
{
    Run-AllocationX86X64Application $arg "heapalloc" $options 
    Run-AllocationX86X64Application $arg "malloc" $options
    Run-AllocationX86X64Application $arg "new" $options
}

Function Run-AllocationX86X64Application($arg, $alloc, $options)
{
    Run-AllocationReleaseDebugApplication $arg "." "x86" $alloc $options
    Run-AllocationReleaseDebugApplication $arg "x64" "x64" $alloc $options
}

Function Run-AllocationReleaseDebugApplication($arg, $arch_dir, $arch, $alloc, $options)
{
    Run-AllocationApplication $arg "debug" $arch_dir $arch $alloc $options
    Run-AllocationApplication $arg "release" $arch_dir $arch $alloc $options
}

Function Run-AllocationApplication($arg, $config, $arch_dir, $arch, $alloc, $options)
{
    $dmp = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).dmp"
    remove-item $dmp -ErrorAction:SilentlyContinue
    $log = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).log"
    remove-item $log -ErrorAction:SilentlyContinue
    $json = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($alloc)$($options).json"
    remove-item $json -ErrorAction:SilentlyContinue
    . "$PSScriptRoot\..\$arch_dir\$config\$app_name" "--$arg" "--use$alloc" "--dmp" $dmp "--log" $log "--json" $json
    . "$PSScriptRoot\..\x64\Release\ValidateHeapEntries.exe" "--dmp" $dmp "--log" $ResultFile "--json" $json
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

remove-item $ResultFile -ErrorAction:SilentlyContinue

$app_name = "AllocationSetupTests.exe"
$app_image_options = "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\AllocationSetupTests.exe"
$global_flag = "GlobalFlag"
$page_heap_flags = "PageHeapFlags"
$front_end_heap = "FrontEndHeapDebugOptions"

#. $GFlags /i $app_name -ust -hpa
Remove-Item $app_image_options -Recurse -ErrorAction:SilentlyContinue
Run-AllAllocationApplicationArgs ""

#. $GFlags /i $app_name +ust -hpa
New-Item $app_image_options
New-ItemProperty -Path $app_image_options -Name $global_flag -PropertyType DWord -Value 0x1000
Run-AllAllocationApplicationArgs "_ust"

#. $GFlags /i $app_name +ust +hpa
Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2001000
New-ItemProperty -Path $app_image_options -Name $page_heap_flags -PropertyType DWord -Value 0x3
Run-AllAllocationApplicationArgs "_ust_hpa"

# enable segment heap type
Remove-ItemProperty -Path $app_image_options -Name $global_flag
Remove-ItemProperty -Path $app_image_options -Name $page_heap_flags
New-ItemProperty -Path $app_image_options -Name $front_end_heap -PropertyType DWord -Value 0x08
Run-AllAllocationApplicationArgs "_segment"

#. $GFlags /i $app_name +ust -hpa
New-ItemProperty -Path $app_image_options -Name $global_flag -PropertyType DWord -Value 0x1000
Run-AllAllocationApplicationArgs "_segment_ust"

#. $GFlags /i $app_name +ust +hpa
Set-ItemProperty -Path $app_image_options -Name $global_flag -Value 0x2001000
New-ItemProperty -Path $app_image_options -Name $page_heap_flags -PropertyType DWord -Value 0x3
Run-AllAllocationApplicationArgs "_segment_ust_hpa"

#. $GFlags /i $app_name -ust -hpa
Remove-Item $app_image_options -Recurse -ErrorAction:SilentlyContinue
