# Install Python and Voice Dependencies - WasteGuard
# =================================================

$ErrorActionPreference = "Continue"
Write-Host "==============================================="
Write-Host " WasteGuard - Python Voice Setup"
Write-Host "==============================================="
Write-Host ""

# Vérifier si Python est installé
Write-Host "[1] Checking for Python installation..."
try {
    $pythonVersion = python --version 2>&1
    Write-Host "[OK] Python found: $pythonVersion"
} catch {
    Write-Host "[INFO] Python not in PATH, checking alternative locations..."
    
    # Chercher Python dans les emplacements standards
    $pythonPaths = @(
        "C:\Users\$env:USERNAME\.local\bin\python3.14.exe",
        "C:\Users\$env:USERNAME\.local\bin\python3.exe",
        "C:\Program Files\Python314\python.exe",
        "C:\Program Files\Python311\python.exe",
        "C:\Program Files (x86)\Python314\python.exe"
    )
    
    $found = $false
    foreach ($path in $pythonPaths) {
        if (Test-Path $path) {
            Write-Host "[OK] Found Python at: $path"
            $pythonCmd = $path
            $found = $true
            break
        }
    }
    
    if (-not $found) {
        Write-Host "[ERROR] Python not found!"
        Write-Host ""
        Write-Host "SOLUTION: Install Python 3.11+ from https://www.python.org/downloads/"
        Write-Host ""
        Write-Host "When installing, make sure to:"
        Write-Host "1. Check 'Add Python to PATH'"
        Write-Host "2. Click 'Install Now'"
        Write-Host ""
        Write-Host "After installation, run this script again."
        Read-Host "Press Enter to open Python download page..."
        Start-Process "https://www.python.org/downloads/"
        exit
    }
}

# Utiliser python du PATH si trouvé
$pythonCmd = "python"

Write-Host ""
Write-Host "[2] Installing voice dependencies..."
Write-Host "    Installing: speech_recognition, pyttsx3, sounddevice"
Write-Host ""

# Installer les packages
& $pythonCmd -m pip install --upgrade pip | Out-Null
& $pythonCmd -m pip install speech_recognition pyttsx3 sounddevice

# Vérifier que tout s'est bien installé
Write-Host ""
Write-Host "[3] Verifying installation..."

$packages = @("speech_recognition", "pyttsx3", "sounddevice")
$allGood = $true

foreach ($package in $packages) {
    try {
        & $pythonCmd -c "import $package" 2>&1 | Out-Null
        Write-Host "[OK] $package installed"
    } catch {
        Write-Host "[ERROR] Failed to import $package"
        $allGood = $false
    }
}

Write-Host ""
if ($allGood) {
    Write-Host "==============================================="
    Write-Host " ✓ SUCCESS - Voice engine is ready!"
    Write-Host "==============================================="
    Write-Host ""
    Write-Host "Next steps:"
    Write-Host "1. Close Qt Creator completely"
    Write-Host "2. Rebuild the project (Clean All, then Ctrl+B)"
    Write-Host "3. Restart WasteGuard"
    Write-Host "4. Click the microphone button to test"
    Write-Host ""
} else {
    Write-Host "==============================================="
    Write-Host " ⚠ Some packages failed to install"
    Write-Host "==============================================="
    Write-Host ""
    Write-Host "Try running this command manually:"
    Write-Host "$pythonCmd -m pip install --upgrade pip"
    Write-Host "$pythonCmd -m pip install speech_recognition pyttsx3 sounddevice"
    Write-Host ""
}

Read-Host "Press Enter to close"
