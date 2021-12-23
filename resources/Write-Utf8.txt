Function Write-Utf8([string] $path, [string] $filter='*.*')
{
    [IO.SearchOption] $option = [IO.SearchOption]::AllDirectories;
    [String[]] $files = [IO.Directory]::GetFiles((Get-Item $path).FullName, $filter, $option);
    foreach($file in $files)
    {
        "Writing $file...";
        [String]$s = [IO.File]::ReadAllText($file);
        [IO.File]::WriteAllText($file, $s, [Text.Encoding]::UTF8);
    }
}