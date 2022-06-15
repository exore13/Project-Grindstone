# Console Profile Extras

## Bash

```Bash
### Structured and full info LS
alias lsa="pwd; ls -lha"

### Copy current directory to clipboard | Only tried on WSL2 Ubuntu. Probably works using xclip on native linux
alias pwdc="echo cd $(pwd) | clip.exe" 
```

---

## Powershell

```Powershell
### Structured and full info LS | Get-ChildItem + hidden + human readable
Function Format-FileSize() {
    Param ([int64]$size)
    If     ($size -gt 1TB) {[string]::Format("{0:0.00} TB", $size / 1TB)}
    ElseIf ($size -gt 1GB) {[string]::Format("{0:0.00} GB", $size / 1GB)}
    ElseIf ($size -gt 1MB) {[string]::Format("{0:0.00} MB", $size / 1MB)}
    ElseIf ($size -gt 1KB) {[string]::Format("{0:0.00} kB", $size / 1KB)}
    ElseIf ($size -gt 0)   {[string]::Format("{0:0.00} B", $size)}
    Else                   {""}
}

Function lsa {
    Get-ChildItem -Force | Select-Object Mode, LastWriteTime, @{Name="Size";Expression={Format-FileSize($_.Length)}}, Name
}


### Copy current directory to clipboard | Allows fast cd with multiple terminals
Function pwdc {
    $t = pwd
    Set-Clipboard -Value "cd '$t'"
    Write-Host "Path copied to clipboard"
}


### Fast directory jump
Function cdgit {
    cd 'D:\AA - Github' # Replace with your own main repos folder
}
```
