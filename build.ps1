Param
(
    [switch]$Rebuild,
    [switch]$Clean
)

if($Clean)
{
    MSBuild DbgHelpUtils.sln -t:Clean -p:Configuration=Release -p:Platform=x64
    MSBuild DbgHelpUtils.sln -t:Clean -p:Configuration=Release -p:Platform=x86
    MSBuild DbgHelpUtils.sln -t:Clean -p:Configuration=Debug -p:Platform=x64
    MSBuild DbgHelpUtils.sln -t:Clean -p:Configuration=Debug -p:Platform=x86

    Remove-Item -Force -Recurse -ErrorAction SilentlyContinue x86
    Remove-Item -Force -Recurse -ErrorAction SilentlyContinue x64
}

if($Rebuild)
{
    $target = "Rebuild"
}
else
{
    $target = "Build"
}

MSBuild DbgHelpUtils.sln -t:$target -p:Configuration=Release -p:Platform=x64 -m
if ($LASTEXITCODE -ne 0) { throw "Failed to build Release x64" }
MSBuild DbgHelpUtils.sln -t:$target -p:Configuration=Release -p:Platform=x86 -m
if ($LASTEXITCODE -ne 0) { throw "Failed to build Release x86" }
MSBuild DbgHelpUtils.sln -t:$target -p:Configuration=Debug -p:Platform=x64 -m
if ($LASTEXITCODE -ne 0) { throw "Failed to build Debug x64" }
MSBuild DbgHelpUtils.sln -t:$target -p:Configuration=Debug -p:Platform=x86 -m
if ($LASTEXITCODE -ne 0) { throw "Failed to build Debug x86" }

Write-Host All done