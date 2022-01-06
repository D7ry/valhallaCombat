# args
param (
    [Parameter(Mandatory)][ValidateSet('COPY', 'SOURCEGEN', 'DISTRIBUTE')][string]$Mode,
    [string]$Version,
    [string]$Path,
    [string]$Project,
    [string]$Anniversary # VS passes in string
)


$ErrorActionPreference = "Stop"

$Folder = $PSScriptRoot | Split-Path -Leaf
$AcceptedExt = @('.c', '.cpp', '.cxx', '.h', '.hpp', '.hxx')


function Resolve-Files {
    param (
        [Parameter(ValueFromPipeline)][string]$a_parent = $PSScriptRoot,
        [string[]]$a_directory = @('include', 'src', 'test')
    )
    
    process {
        Push-Location $PSScriptRoot
        $capacity = 16
        if ($Folder -eq 'CommonLibSSE') {
            $capacity = 2048
        } else {
            $capacity = 16
        }
        $_generated = [System.Collections.ArrayList]::new($capacity)

        try {
            foreach ($directory in $a_directory) {
                Get-ChildItem "$a_parent/$directory" -Recurse -File -ErrorAction SilentlyContinue | Where-Object {
                    ($_.Extension -in $AcceptedExt) -and 
                    ($_.Name -ne 'Version.h')
                } | Resolve-Path -Relative | ForEach-Object {
                    $_generated.Add("`n`t`"$($_.Substring(2) -replace '\\', '/')`"") | Out-Null
                }

                if (!$env:RebuildInvoke) {
                    Write-Host "`t<$a_parent/$directory>"
                    foreach ($file in $_generated) {
                        Write-Host "$file"
                    }
                }
            }
        } finally {
            Pop-Location
        }

        return $_generated
    }
}


Write-Host "`n`t<$Folder> [$Mode]"


# @@COPY
if ($Mode -eq 'COPY') {
    $GameBase = $null
    $MO2 = $null
    $Destination = $null

    # process newly added files
    $BuildFolder = Get-ChildItem (Get-Item $Path).Parent.Parent.FullName "$Project.sln" -Depth 2 -File -Exclude ('*CMakeFiles*', '*CLib*')
    $NewFiles = Get-ChildItem $BuildFolder.DirectoryName -File | Where-Object {$_.Extension -in $AcceptedExt}
    if ($NewFiles) { # trigger ZERO_CHECK
        $NewFiles | Move-Item -Destination "$PSScriptRoot/src" -Force -Confirm:$false -ErrorAction:SilentlyContinue | Out-Null
        [IO.File]::WriteAllText("$PSScriptRoot/CMakeLists.txt", [IO.File]::ReadAllText("$PSScriptRoot/CMakeLists.txt"))
    }

    # Build Target
    $AE = [bool][Int32]$Anniversary
    $vcpkg = [IO.File]::ReadAllText("$PSScriptRoot/vcpkg.json") | ConvertFrom-Json
    if ($AE) {
        $GameBase = $env:SkyrimAEPath
        $MO2 = $env:MO2SkyrimAEPath
        Write-Host "`t$Folder $Version | ANNIVERSARY EDITION"
    } else {
        $GameBase = $env:SkyrimSEPath
        $MO2 = $env:MO2SkyrimSEPath
        Write-Host "`t$Folder $Version | SPECIAL EDITION"
    }

    $BtnCopyDataText = $null
    if ($MO2) {
        $Destination = Join-Path "$MO2/mods" $vcpkg.'features'.'mo2-install'.'description'
        $BtnCopyDataText = 'Copy to MO2'
    } else {
        $Destination = Join-Path "$GameBase" "Data" 
        $BtnCopyDataText = 'Copy to Data'
    }

    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing

    [System.Windows.Forms.Application]::EnableVisualStyles()
    $MsgBox = New-Object System.Windows.Forms.Form -Property @{
        TopLevel = $true
        TopMost = $true
        ClientSize = '350, 250'
        Text = $Project
        StartPosition = 'CenterScreen'
        FormBorderStyle = 'FixedDialog'
        MaximizeBox = $false
        MinimizeBox = $false
        Font = New-Object System.Drawing.Font('Segoe UI', 10, [System.Drawing.FontStyle]::Regular)
    }
    
    $Message = New-Object System.Windows.Forms.Label -Property @{
        ClientSize = '190, 140'
        Location = New-Object System.Drawing.Point(20, 20)
        Text = "$Project has been built."
    }
    
    $BtnCopyData = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '90, 50'
        Location = New-Object System.Drawing.Point(20, 180)
        Text = $BtnCopyDataText
        Add_Click = {
            New-Item -Type Directory "$Destination/SKSE/Plugins" -Force | Out-Null

            # binary
            Copy-Item "$Path/$Project.dll" "$Destination/SKSE/Plugins/$Project.dll" -Force
            $Message.Text += "`nBinary file copied!"

            # configs
            Get-ChildItem $PSScriptRoot -Recurse | Where-Object {
                ($_.Extension -in '.toml', '.json', '.ini') -and 
                ($_.Name -ne 'vcpkg.json')
            } | ForEach-Object {
                Copy-Item $_.FullName "$Destination/SKSE/Plugins/$($_.Name)" -Force
                $Message.Text += "`n$($_.Name) copied!"
            }

            # shockwave
            if (Test-Path "$PSScriptRoot/Interface/*.swf" -PathType Leaf) {
                New-Item -Type Directory "$Destination/Interface" -Force | Out-Null
                Copy-Item "$PSScriptRoot/Interface" "$Destination" -Recurse -Force
                $Message.Text += "`nShockwave files copied!"
            }

            $BtnCopyData.Enabled = $false;
        }
    }

    $BtnLaunchSKSE = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '90, 50'
        Text = 'Launch SKSE'
        Location = New-Object System.Drawing.Point(130, 180)
        Add_Click = {
            Push-Location $GameBase
            Start-Process ./skse64_loader.exe
            Pop-Location
            $MsgBox.Close()
        }
    }
    
    $BtnOpenFolder = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '90, 50'
        Text = 'Open Folder'
        Location = New-Object System.Drawing.Point(240, 180)
        Add_Click = {
            Invoke-Item $Path
        }
    }

    $BtnExit = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '90, 50'
        Text = 'Exit'
        Location = New-Object System.Drawing.Point(240, 110)
        Add_Click = {
            $MsgBox.Close()
        }
    }

    # papyrus
    if (Test-Path "$PSScriptRoot/Scripts/Source/*.psc" -PathType Leaf) {
        $BtnBuildPapyrus = New-Object System.Windows.Forms.Button -Property @{
            ClientSize = '90, 50'
            Text = 'Build Papyrus'
            Location = New-Object System.Drawing.Point(240, 20)
            Add_Click = {
                New-Item -Type Directory "$Destination/Scripts" -Force | Out-Null
                & "$GameBase/Papyrus Compiler/PapyrusCompiler.exe" "$PSScriptRoot/Scripts/Source" -f="$GameBase/Papyrus Compiler/TESV_Papyrus_Flags.flg" -i="$GameBase/Data/Scripts/Source;./Scripts/Source" -o="$PSScriptRoot/Scripts" -a
    
                Copy-Item "$PSScriptRoot/Scripts" "$Destination" -Recurse -Force
                Remove-Item "$Destination/Scripts/Source" -Force -Confirm:$false -ErrorAction Ignore
                $Message.Text += "`nPapyrus scripts copied!"
            }
        }

        $MsgBox.Controls.Add($BtnBuildPapyrus)
    }
                
    $MsgBox.Controls.Add($Message)
    $MsgBox.Controls.Add($BtnCopyData)
    $MsgBox.Controls.Add($BtnLaunchSKSE)
    $MsgBox.Controls.Add($BtnOpenFolder)
    $MsgBox.Controls.Add($BtnExit)

    # Check CMake VERSION
    $OutputVersion
    $OriginalVersion = $vcpkg.'version-string'
    [IO.File]::ReadAllLines("$($BuildFolder.Directory)/include/Version.h") | ForEach-Object {
        if ($_.Trim().StartsWith('inline constexpr auto NAME = "')) {
            $OutputVersion = $_.Trim().Substring(30, 5)
            if ($OutputVersion -ne $vcpkg.'version-string') {
                $Message.Text += "`nVersionInfo changed! Updating CMakeLists..."  

                $CMakeLists = [IO.File]::ReadAllText("$PSScriptRoot/CMakeLists.txt") -replace "VERSION\s$($vcpkg.'version-string')", "VERSION $OutputVersion"
                [IO.File]::WriteAllText("$PSScriptRoot/CMakeLists.txt", $CMakeLists)

                $vcpkg.'version-string' = $OutputVersion
                $vcpkg = $vcpkg | ConvertTo-Json -Depth 9
                [IO.File]::WriteAllText("$PSScriptRoot/vcpkg.json", $vcpkg)
                
                $Message.Text += "`n$Project has been changed from $($OriginalVersion) to $($OutputVersion)`n`nThis update will be in effect after next successful build!"
            }
        }
    }

    $MsgBox.ShowDialog() | Out-Null
    Exit
}


# @@SOURCEGEN
if ($Mode -eq 'SOURCEGEN') {
    Write-Host "`tGenerating CMake sourcelist..."
    Remove-Item "$Path/sourcelist.cmake" -Force -Confirm:$false -ErrorAction Ignore

    $generated = 'set(SOURCES'
    $generated += $PSScriptRoot | Resolve-Files
    if ($Path) {
        $generated += $Path | Resolve-Files
    }
    $generated += "`n)"
    [IO.File]::WriteAllText("$Path/sourcelist.cmake", $generated)

    # update vcpkg.json accordinly
    $vcpkg = [IO.File]::ReadAllText("$PSScriptRoot/vcpkg.json") | ConvertFrom-Json
    $vcpkg.'name' = $vcpkg.'name'.ToLower()
    $vcpkg.'version-string' = $Version    
    if (!($vcpkg | Get-Member features)) {
        $features = @"
{
"mo2-install": {
    "description": ""
}
}
"@ | ConvertFrom-Json
        $features.'mo2-install'.'description' = $Folder
        $vcpkg | Add-Member -Name 'features' -Value $features -MemberType NoteProperty
    }

    # patch regression
    $vcpkg.PsObject.Properties.Remove('script-version')
    $vcpkg.PsObject.Properties.Remove('build-config')
    $vcpkg.PsObject.Properties.Remove('build-target')
    if ($vcpkg | Get-Member install-name) {
        $vcpkg.'features'.'mo2-install'.'description' = $vcpkg.'install-name'
    }
    $vcpkg.PsObject.Properties.Remove('install-name')

    # inversed version control
    if (Test-Path "$Path/version.rc" -PathType Leaf) {
        $VersionResource = [IO.File]::ReadAllText("$Path/version.rc") -replace "`"FileDescription`",\s`"$Folder`"",  "`"FileDescription`", `"$($vcpkg.'description')`""
        [IO.File]::WriteAllText("$Path/version.rc", $VersionResource)
    }

    $vcpkg = $vcpkg | ConvertTo-Json -Depth 9
    [IO.File]::WriteAllText("$PSScriptRoot/vcpkg.json", $vcpkg)
}


# @@DISTRIBUTE
if ($Mode -eq 'DISTRIBUTE') { # update script to every project
    Get-ChildItem "$PSScriptRoot/*/*" -Directory | Where-Object {
        $_.Name -notin @('vcpkg', 'Build', '.git', 'PluginTutorialCN') -and
        (Test-Path "$_/CMakeLists.txt" -PathType Leaf)
    } | ForEach-Object {
        Write-Host "`n`tUpdated <$_>"
        Robocopy.exe "$PSScriptRoot" "$_" '!Update.ps1' /MT /NJS /NFL /NDL /NJH | Out-Null
    }
}

# SIG # Begin signature block
# MIIR2wYJKoZIhvcNAQcCoIIRzDCCEcgCAQExCzAJBgUrDgMCGgUAMGkGCisGAQQB
# gjcCAQSgWzBZMDQGCisGAQQBgjcCAR4wJgIDAQAABBAfzDtgWUsITrck0sYpfvNR
# AgEAAgEAAgEAAgEAAgEAMCEwCQYFKw4DAhoFAAQUo4IETWy/XGwRCnxvB1YN/MJ5
# ng2ggg1BMIIDBjCCAe6gAwIBAgIQV1bI9hFtibxLuv7jpq+icTANBgkqhkiG9w0B
# AQsFADAbMRkwFwYDVQQDDBBES1NjcmlwdFNlbGZDZXJ0MB4XDTIxMTIxNzAxNTg0
# M1oXDTIyMTIxNzAyMTg0M1owGzEZMBcGA1UEAwwQREtTY3JpcHRTZWxmQ2VydDCC
# ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANQWNd4o4p2MhRPwLWC8oxvq
# trmcjhMstx2mwvqfT9DIRrlhqtpTkJXyWKML82Azf84i1/lI5g3eILnfAhGU7/1Q
# AtZSDMw5xlA73Y+XBEzAUZ1S4BvEN2y/5OeWYH+YKxDLXh7lt2KQQ4jF/6k+/5RN
# DsPWTkbZosfsLsOqkya6LiaYURAGBWYdsDbfi1xPZmDQ5sdwfiOiLq/jPWSCK4Jl
# Ic9xr3+K1zOSuDYStnUbLx62jC7FgMlgMjtHFFj7Cu/IqTK0W1Nm++5KY4t7YUc9
# ngm6aWaujsID28lk6DVgn7INvioZinZyl1eWzESNkBFOzG5rhOdz/I8a7yqpR3EC
# AwEAAaNGMEQwDgYDVR0PAQH/BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMB0G
# A1UdDgQWBBRC5iIjTNsF4fv6NX70SldETlW60DANBgkqhkiG9w0BAQsFAAOCAQEA
# g9Lz2SELbbdAuqQP83av7QB+r5FFnPnbhfb3DcAprzDfbG0hQ+J7c3fVWYT0gK1J
# V2Qsl7Jm5nHlpXZn30kNTOSTk5wLPO9AUquFF3stabsi+DiFHzPZ1Xrehxqeik0G
# WB2tq+7sBO5CYDJxohNAfDe1WrChpJ0v2kyjLN2gke+GZzk/unfcVBxjBKxAr7Se
# BMpwc6Dx+ngokZBT8w84E33QUbiEvq/i1bfv0Fc/2/2tibQDIAuYmmSBqD+CxKg6
# oG0XH4fbXKOdhmWC+hm8IRPgHlPT0EYEM4CWhQ5LRysDgMATX/ZwlvExUwDrIUq7
# VrkACWSWu3uMvJb5HeyLMTCCBP4wggPmoAMCAQICEA1CSuC+Ooj/YEAhzhQA8N0w
# DQYJKoZIhvcNAQELBQAwcjELMAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0
# IEluYzEZMBcGA1UECxMQd3d3LmRpZ2ljZXJ0LmNvbTExMC8GA1UEAxMoRGlnaUNl
# cnQgU0hBMiBBc3N1cmVkIElEIFRpbWVzdGFtcGluZyBDQTAeFw0yMTAxMDEwMDAw
# MDBaFw0zMTAxMDYwMDAwMDBaMEgxCzAJBgNVBAYTAlVTMRcwFQYDVQQKEw5EaWdp
# Q2VydCwgSW5jLjEgMB4GA1UEAxMXRGlnaUNlcnQgVGltZXN0YW1wIDIwMjEwggEi
# MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDC5mGEZ8WK9Q0IpEXKY2tR1zoR
# Qr0KdXVNlLQMULUmEP4dyG+RawyW5xpcSO9E5b+bYc0VkWJauP9nC5xj/TZqgfop
# +N0rcIXeAhjzeG28ffnHbQk9vmp2h+mKvfiEXR52yeTGdnY6U9HR01o2j8aj4S8b
# Ordh1nPsTm0zinxdRS1LsVDmQTo3VobckyON91Al6GTm3dOPL1e1hyDrDo4s1SPa
# 9E14RuMDgzEpSlwMMYpKjIjF9zBa+RSvFV9sQ0kJ/SYjU/aNY+gaq1uxHTDCm2mC
# tNv8VlS8H6GHq756WwogL0sJyZWnjbL61mOLTqVyHO6fegFz+BnW/g1JhL0BAgMB
# AAGjggG4MIIBtDAOBgNVHQ8BAf8EBAMCB4AwDAYDVR0TAQH/BAIwADAWBgNVHSUB
# Af8EDDAKBggrBgEFBQcDCDBBBgNVHSAEOjA4MDYGCWCGSAGG/WwHATApMCcGCCsG
# AQUFBwIBFhtodHRwOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHwYDVR0jBBgwFoAU
# 9LbhIB3+Ka7S5GGlsqIlssgXNW4wHQYDVR0OBBYEFDZEho6kurBmvrwoLR1ENt3j
# anq8MHEGA1UdHwRqMGgwMqAwoC6GLGh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9z
# aGEyLWFzc3VyZWQtdHMuY3JsMDKgMKAuhixodHRwOi8vY3JsNC5kaWdpY2VydC5j
# b20vc2hhMi1hc3N1cmVkLXRzLmNybDCBhQYIKwYBBQUHAQEEeTB3MCQGCCsGAQUF
# BzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wTwYIKwYBBQUHMAKGQ2h0dHA6
# Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydFNIQTJBc3N1cmVkSURUaW1l
# c3RhbXBpbmdDQS5jcnQwDQYJKoZIhvcNAQELBQADggEBAEgc3LXpmiO85xrnIA6O
# Z0b9QnJRdAojR6OrktIlxHBZvhSg5SeBpU0UFRkHefDRBMOG2Tu9/kQCZk3taaQP
# 9rhwz2Lo9VFKeHk2eie38+dSn5On7UOee+e03UEiifuHokYDTvz0/rdkd2NfI1Jp
# g4L6GlPtkMyNoRdzDfTzZTlwS/Oc1np72gy8PTLQG8v1Yfx1CAB2vIEO+MDhXM/E
# EXLnG2RJ2CKadRVC9S0yOIHa9GCiurRS+1zgYSQlT7LfySmoc0NR2r1j1h9bm/cu
# G08THfdKDXF+l7f0P4TrweOjSaH6zqe/Vs+6WXZhiV9+p7SOZ3j5NpjhyyjaW4em
# ii8wggUxMIIEGaADAgECAhAKoSXW1jIbfkHkBdo2l8IVMA0GCSqGSIb3DQEBCwUA
# MGUxCzAJBgNVBAYTAlVTMRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsT
# EHd3dy5kaWdpY2VydC5jb20xJDAiBgNVBAMTG0RpZ2lDZXJ0IEFzc3VyZWQgSUQg
# Um9vdCBDQTAeFw0xNjAxMDcxMjAwMDBaFw0zMTAxMDcxMjAwMDBaMHIxCzAJBgNV
# BAYTAlVTMRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdp
# Y2VydC5jb20xMTAvBgNVBAMTKERpZ2lDZXJ0IFNIQTIgQXNzdXJlZCBJRCBUaW1l
# c3RhbXBpbmcgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC90DLu
# S82Pf92puoKZxTlUKFe2I0rEDgdFM1EQfdD5fU1ofue2oPSNs4jkl79jIZCYvxO8
# V9PD4X4I1moUADj3Lh477sym9jJZ/l9lP+Cb6+NGRwYaVX4LJ37AovWg4N4iPw7/
# fpX786O6Ij4YrBHk8JkDbTuFfAnT7l3ImgtU46gJcWvgzyIQD3XPcXJOCq3fQDpc
# t1HhoXkUxk0kIzBdvOw8YGqsLwfM/fDqR9mIUF79Zm5WYScpiYRR5oLnRlD9lCos
# p+R1PrqYD4R/nzEU1q3V8mTLex4F0IQZchfxFwbvPc3WTe8GQv2iUypPhR3EHTyv
# z9qsEPXdrKzpVv+TAgMBAAGjggHOMIIByjAdBgNVHQ4EFgQU9LbhIB3+Ka7S5GGl
# sqIlssgXNW4wHwYDVR0jBBgwFoAUReuir/SSy4IxLVGLp6chnfNtyA8wEgYDVR0T
# AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwEwYDVR0lBAwwCgYIKwYBBQUH
# AwgweQYIKwYBBQUHAQEEbTBrMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdp
# Y2VydC5jb20wQwYIKwYBBQUHMAKGN2h0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNv
# bS9EaWdpQ2VydEFzc3VyZWRJRFJvb3RDQS5jcnQwgYEGA1UdHwR6MHgwOqA4oDaG
# NGh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEFzc3VyZWRJRFJvb3RD
# QS5jcmwwOqA4oDaGNGh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEFz
# c3VyZWRJRFJvb3RDQS5jcmwwUAYDVR0gBEkwRzA4BgpghkgBhv1sAAIEMCowKAYI
# KwYBBQUHAgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9
# bAcBMA0GCSqGSIb3DQEBCwUAA4IBAQBxlRLpUYdWac3v3dp8qmN6s3jPBjdAhO9L
# hL/KzwMC/cWnww4gQiyvd/MrHwwhWiq3BTQdaq6Z+CeiZr8JqmDfdqQ6kw/4stHY
# fBli6F6CJR7Euhx7LCHi1lssFDVDBGiy23UC4HLHmNY8ZOUfSBAYX4k4YU1iRiSH
# Y4yRUiyvKYnleB/WCxSlgNcSR3CzddWThZN+tpJn+1Nhiaj1a5bA9FhpDXzIAbG5
# KHW3mWOFIoxhynmUfln8jA/jb7UBJrZspe6HUSHkWGCbugwtK22ixH67xCUrRwII
# fEmuE7bhfEJCKMYYVs9BNLZmXbZ0e/VWMyIvIjayS6JKldj1po5SMYIEBDCCBAAC
# AQEwLzAbMRkwFwYDVQQDDBBES1NjcmlwdFNlbGZDZXJ0AhBXVsj2EW2JvEu6/uOm
# r6JxMAkGBSsOAwIaBQCgeDAYBgorBgEEAYI3AgEMMQowCKACgAChAoAAMBkGCSqG
# SIb3DQEJAzEMBgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgorBgEEAYI3
# AgEVMCMGCSqGSIb3DQEJBDEWBBTnxvHIpxrLHfs/KqVXlLgmRgg9PjANBgkqhkiG
# 9w0BAQEFAASCAQCKisgeMX0GuMRf/tbYdyJ88LSVA5lW6p1qUbxSevTxO6fuzsWF
# rEZorWNlE3FNotjpGDhyHsaRmKLhqvxjOJklMp5L/DEb6dtgFcE9TPrYm2BtV1Zq
# C9siv4lgapzgetzpvEXWmsPrrUloN+u3rnDsJw1+NSy5giEDjcaqzA2CVC4F77TH
# r+8bnGy4yd6HFK1/mT4h+RxPMVnYlzwJChRMQS26hYNszN8KLSKEIUMWXgHFpopD
# shtLMEGOBmNC0Jb/Mjy6jCM3HhKApVs+Ofpc2L0GiIH4jAbuyWfl8q8VbMWyCBe2
# Y+BxE8NJvoh1Qc88KMiUWrX7Q3UBhqo1JIZ5oYICMDCCAiwGCSqGSIb3DQEJBjGC
# Ah0wggIZAgEBMIGGMHIxCzAJBgNVBAYTAlVTMRUwEwYDVQQKEwxEaWdpQ2VydCBJ
# bmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5jb20xMTAvBgNVBAMTKERpZ2lDZXJ0
# IFNIQTIgQXNzdXJlZCBJRCBUaW1lc3RhbXBpbmcgQ0ECEA1CSuC+Ooj/YEAhzhQA
# 8N0wDQYJYIZIAWUDBAIBBQCgaTAYBgkqhkiG9w0BCQMxCwYJKoZIhvcNAQcBMBwG
# CSqGSIb3DQEJBTEPFw0yMjAxMDUxODE3MTJaMC8GCSqGSIb3DQEJBDEiBCBnFNwC
# eVeRaLQ0d8kCLXba2RjzgU2y3NceOetOQ4l2szANBgkqhkiG9w0BAQEFAASCAQAM
# Cwx5hBHVddWxUizG6EvPFgAFQL4gTtLYacTJSQtHWHwg8Nfd1JWKONP85DfiCGIh
# 97qejG3HNCfHo7XHFnVgKV6N5GlmqLuwzKJDzNZRddc/OoEd2NHAkUBHnpjRGP0O
# XdG6Hp549xFtWqVYPicfH2tglNv6eHH7DJXfZle61zfJnYC9XR5xFL/m3JenajGT
# klr0RJ2Rbg+c9iW7ZXd67OfqYWaN/1AZd5azN0DIv75QiysLrmixqIvJvsgErQNn
# tTmocdVxCyTd57pjisj1m7YVwmn+lnfhmLQ3WUADIUb/jaWCngIpZ7T+QqUtO0f2
# pE+46T9MXSUI+QJ/ull+
# SIG # End signature block
