# generate all the dump files used in testing heap layouts

Param
(
    [string] $DumpFolder = "G:\temp\crashdumps",
    [string] $GFlags = "G:\utils\gflags\gflags.exe"
)

Function Run-AllAllocationApplicationArgs($options)
{
    Run-AllocationX86X64Application "lfh" $options
    Run-AllocationX86X64Application "virtual" $options
}

Function Run-AllocationX86X64Application($arg, $options)
{
    Run-AllocationReleaseDebugApplication $arg "." "x86" $options
    Run-AllocationReleaseDebugApplication $arg "x64" "x64" $options
}

Function Run-AllocationReleaseDebugApplication($arg, $arch_dir, $arch, $options)
{
    Run-AllocationApplication $arg "debug" $arch_dir $arch $options
    Run-AllocationApplication $arg "release" $arch_dir $arch $options
}

Function Run-AllocationApplication($arg, $config, $arch_dir, $arch, $options)
{
    $alloc_dmp = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($options)alloc.dmp"
    remove-item $alloc_dmp -ErrorAction:SilentlyContinue
    $free_dmp = "$DumpFolder\$($app_name)_$($arch)_$($config)_$($arg)_$($options)free.dmp"
    remove-item $free_dmp -ErrorAction:SilentlyContinue
    . "$PSScriptRoot\..\$arch_dir\$config\$app_name" "--$arg" "--alloc" $alloc_dmp "--free" $free_dmp
}

$app_name = "AllocationSetupTests.exe"

. $GFlags /i $app_name -ust -hpa
Run-AllAllocationApplicationArgs ""

. $GFlags /i $app_name +ust -hpa
Run-AllAllocationApplicationArgs "ust_"

. $GFlags /i $app_name +ust +hpa
Run-AllAllocationApplicationArgs "ust_hpa_"

. $GFlags /i $app_name -ust -hpa
