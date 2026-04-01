# WasteGuard Project Sanitation Script
# This script removes all build artifacts, backups, and temporary files 
# to make the project folder "Universal" and ready to be moved between computers.

$CurrentDir = Get-Location
Write-Host "Cleaning WasteGuard Project in $CurrentDir..." -ForegroundColor Cyan

# 1. Remove Build Folders
$BuildFolders = @("debug", "release", ".qtcreator", ".vscode", ".idea", "__pycache__", ".venv")
foreach ($folder in $BuildFolders) {
    if (Test-Path $folder) {
        Write-Host "Removing folder: $folder" -ForegroundColor Yellow
        Remove-Item -Path $folder -Recurforce -ErrorAction SilentlyContinue
    }
}

# 2. Remove Build Artifacts & Generated Files
$Artifacts = @(
    "Makefile*", 
    ".qmake.stash", 
    "ui_mainwindow.h", 
    "test_ui_mainwindow.h", 
    "*.o", "*.obj", "*.a", "*.lib", "*.dll", "*.exe",
    "build.log", "build_output.log", "debug_build.log", "build_voice_fix.log"
)
foreach ($pattern in $Artifacts) {
    if (Get-Item $pattern -ErrorAction SilentlyContinue) {
        Write-Host "Removing artifact: $pattern" -ForegroundColor Yellow
        Remove-Item -Path $pattern -Force -ErrorAction SilentlyContinue
    }
}

# 3. Remove Backups and Temporary UI files
$Backups = @("mainwindow.ui.backup*", "mainwindow_temp.ui")
foreach ($pattern in $Backups) {
    if (Get-Item $pattern -ErrorAction SilentlyContinue) {
        Write-Host "Removing backup: $pattern" -ForegroundColor Yellow
        Remove-Item -Path $pattern -Force -ErrorAction SilentlyContinue
    }
}

# 4. Remove redundant/old scripts (Cleaning to only essential ones as per README_FINAL.md)
# Note: Keeping verify_setup.bat and voice_engine.py as they are core.
$JunkScripts = @(
    "patch*.py", "safe_patch*.py", "search*.py", "find_*.py", "fix_*.py", 
    "move_*.py", "print_*.py", "append_*.py", "inject_*.py", "restore_header.py", 
    "update_ui.py", "debug.py", "debug2.py", "safe_patch_regex.py", 
    "merge_ui_final.py", "google_veo_bot.py", "invideo_bot.py", 
    "luma_video_bot.js", "meta_video_bot.js", "add_export_excel_stock.py"
)
foreach ($pattern in $JunkScripts) {
    $files = Get-ChildItem -Path $pattern -ErrorAction SilentlyContinue
    foreach ($file in $files) {
        Write-Host "Removing junk script: $($file.Name)" -ForegroundColor Gray
        Remove-Item -Path $file.FullName -Force -ErrorAction SilentlyContinue
    }
}

# 5. Remove accidental files
if (Test-Path "nul)") { Remove-Item "nul)" -Force }

Write-Host "Cleaning complete! The project is now 'Computer B' ready." -ForegroundColor Green
Write-Host "NOTE: To run on a new computer, you MUST have an ODBC DSN named 'Source_Projet2A' configured." -ForegroundColor White
Write-Host "Documentation kept: README_FINAL.md, QUICKSTART.md, etc." -ForegroundColor Cyan
