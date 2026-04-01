/**
 * WasteGuard — Meta AI Video Generation Bot (Desktop Automation)
 * 
 * Opens Meta AI app → Créer une vidéo → Upload image → Type prompt → Send
 */

const { execSync } = require('child_process');
const path = require('path');
const fs = require('fs');

function log(msg) {
    const time = new Date().toLocaleTimeString('fr-FR', { hour12: false });
    console.log(`[${time}] ${msg}`);
}

function parseArgs() {
    const args = {};
    const argv = process.argv.slice(2);
    for (let i = 0; i < argv.length; i += 2) {
        const key = argv[i].replace('--', '');
        const val = argv[i + 1] || '';
        args[key] = val;
    }
    return args;
}

function buildPrompt(args) {
    const name = args.product_name || 'Smart Waste Bin';
    const ref = args.reference || '';
    const capacity = args.capacity || '';
    const battery = args.battery || '';
    const features = args.features || '';
    const featuresList = features
        .split(',')
        .map(f => f.trim())
        .filter(Boolean);
    const featuresText = featuresList.length ? featuresList.join(', ') : 'GPS, capteur UV';
    const capText = capacity ? `${capacity}L` : '';
    const batText = battery ? `${battery}mAh` : '';
    const refText = ref ? ` (ref: ${ref})` : '';
    const specs = [
        capText ? `Capacity: ${capText}.` : '',
        batText ? `Battery: ${batText}.` : ''
    ].filter(Boolean).join(' ');

    return [
        '4-second cinematic 3D product animation from the image.',
        `Futuristic smart waste bin "${name}"${refText}.`,
        specs,
        `Features: ${featuresText}.`,
        'Exploded-view effect: key components separate outward smoothly, float, then reassemble cleanly.',
        'Focus components: GPS module, UV sensor, LED ring, battery block.',
        'Timeline: 0-1s hero orbit, 1-3s exploded view with components revealed, 3-4s reassemble and end on centered hero.',
        'Style: premium Apple-like reveal, dark studio background, soft volumetric light, smooth motion, no jitter.',
        'Materials: matte + brushed metal, subtle LED glow.',
        'No text, no logos, no UI overlays, no watermark.'
    ].filter(Boolean).join(' ');
}

function runPS(scriptContent) {
    const tmpFile = path.join(__dirname, '_tmp_bot.ps1');
    try {
        fs.writeFileSync(tmpFile, scriptContent, 'utf-8');
        return execSync(
            `powershell -NoProfile -ExecutionPolicy Bypass -File "${tmpFile}"`,
            { encoding: 'utf-8', timeout: 60000 }
        ).trim();
    } catch (e) {
        log(`PS erreur: ${e.message}`);
        return 'ERROR';
    } finally {
        try { fs.unlinkSync(tmpFile); } catch {}
    }
}

// Click at a position relative to the window
function clickAt(wX, wY, wW, wH, pctX, pctY, label) {
    const x = wX + Math.floor(wW * pctX);
    const y = wY + Math.floor(wH * pctY);
    log(`  → Clic "${label}" à (${x}, ${y})`);
    runPS(`
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class C {
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern void mouse_event(int f, int dx, int dy, int c, int e);
}
"@
[C]::SetCursorPos(${x}, ${y})
Start-Sleep -Milliseconds 150
[C]::mouse_event(0x0002, 0, 0, 0, 0)
[C]::mouse_event(0x0004, 0, 0, 0, 0)
`);
}

function sleep(ms) { return new Promise(r => setTimeout(r, ms)); }

async function main() {
    const args = parseArgs();
    const imagePath = args.image_path ? path.resolve(args.image_path) : null;
    const prompt = buildPrompt(args) || '';

    log(`Prompt: ${prompt.substring(0, 60)}...`);
    if (imagePath) log(`Image: ${imagePath}`);
    log('');
    log('⚠️  NE TOUCHEZ PAS la souris/clavier !');

    // ─── STEP 1: Open Meta AI ───
    const shortcut = path.join(process.env.APPDATA,
        'Microsoft', 'Windows', 'Start Menu', 'Programs', 'Chrome Apps', 'Meta AI.lnk');
    if (!fs.existsSync(shortcut)) { log('ERREUR: Raccourci Meta AI introuvable.'); process.exit(1); }

    log('1. Ouverture de Meta AI...');
    runPS(`Start-Process -FilePath '${shortcut}'`);
    await sleep(6000);

    // ─── STEP 2: Focus window + get dimensions ───
    log('2. Focus fenêtre...');
    const focusResult = runPS(`
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class U {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
}
public struct RECT { public int Left, Top, Right, Bottom; }
public struct POINT { public int X; public int Y; }
"@
$hwnd = $null
foreach ($p in Get-Process | Where-Object { $_.MainWindowHandle -ne 0 }) {
    if ($p.MainWindowTitle -match "Meta") { $hwnd = $p.MainWindowHandle; break }
}
if ($hwnd -eq $null) { Write-Host "NOT_FOUND"; exit }
[U]::ShowWindow($hwnd, 9)
Start-Sleep -Milliseconds 200
[U]::SetForegroundWindow($hwnd)
Start-Sleep -Milliseconds 200
[U]::SetForegroundWindow($hwnd)
$r = New-Object RECT
[U]::GetClientRect($hwnd, [ref]$r)
$pt = New-Object POINT
$pt.X = $r.Left
$pt.Y = $r.Top
[U]::ClientToScreen($hwnd, [ref]$pt) | Out-Null
$w = $r.Right - $r.Left
$h = $r.Bottom - $r.Top
Write-Host "OK|$($pt.X)|$($pt.Y)|$w|$h"
`);

    if (focusResult.includes('NOT_FOUND')) { log('ERREUR: Fenêtre non trouvée !'); process.exit(1); }

    const p = focusResult.split('|');
    const wX = parseInt(p[1]), wY = parseInt(p[2]), wW = parseInt(p[3]), wH = parseInt(p[4]);
    log(`   Fenêtre: ${wW}x${wH} à (${wX},${wY})`);
    await sleep(500);

    // ─── STEP 3: Click "Créer une vidéo" (adjusted slightly down) ───
    log('3. Clic "Créer une vidéo"...');
    // Using 0.50 instead of 0.48 (user said "move it down")
    clickAt(wX, wY, wW, wH, 0.82, 0.50, 'Créer une vidéo');
    await sleep(2000);

    function clickCandidates(label, candidates) {
        for (const [x, y] of candidates) {
            clickAt(wX, wY, wW, wH, x, y, label);
        }
    }

    // ─── STEP 4: Click the "+" button (left of input) ───
    if (imagePath && fs.existsSync(imagePath)) {
        log('4. Clic sur le bouton "+"...');
        clickCandidates('+ Button', [
            [0.37, 0.41],
            [0.36, 0.43],
            [0.39, 0.40],
            [0.35, 0.41]
        ]);
        await sleep(1200);

        // ─── STEP 5: Click "Ajouter du contenu multimédia et des fichiers" ───
        log('5. Clic "Ajouter du contenu multimédia"...');
        clickCandidates('Menu Item', [
            [0.50, 0.46],
            [0.52, 0.47],
            [0.48, 0.45],
            [0.50, 0.50]
        ]);
        await sleep(2200);

        // ─── STEP 6: File dialog — use clipboard to paste path (more reliable than typing) ───
        log('6. Sélection de l\'image dans le dialogue Windows...');
        // Write the path to a temp file to avoid escaping issues entirely
        const pathFile = path.join(__dirname, '_tmp_path.txt');
        fs.writeFileSync(pathFile, imagePath, 'utf-8');

        runPS(`
Add-Type -AssemblyName System.Windows.Forms
# Wait for dialog to appear
Start-Sleep -Milliseconds 1000
# Read path from file and set clipboard
$imgPath = Get-Content -Path '${pathFile.replace(/'/g, "''")}' -Raw -Encoding UTF8
[System.Windows.Forms.Clipboard]::SetText($imgPath.Trim())
Start-Sleep -Milliseconds 300
# Paste (Ctrl+V) and Enter
[System.Windows.Forms.SendKeys]::SendWait("^v")
Start-Sleep -Milliseconds 500
[System.Windows.Forms.SendKeys]::SendWait("{ENTER}")
`);
        try { fs.unlinkSync(pathFile); } catch {}
        log('   Image sélectionnée !');
        await sleep(4000);
    } else {
        log('4. Pas d\'image — prompt texte uniquement.');
    }

    // ─── STEP 7: Click input area and paste prompt ───
    log('7. Clic sur la zone de saisie...');
    clickAt(wX, wY, wW, wH, 0.55, 0.37);
    await sleep(500);

    log('8. Collage du prompt...');
    const promptFile = path.join(__dirname, '_tmp_prompt.txt');
    fs.writeFileSync(promptFile, prompt, 'utf-8');
    runPS(`
Add-Type -AssemblyName System.Windows.Forms
$text = Get-Content -Path '${promptFile.replace(/\\/g, '\\\\')}' -Raw -Encoding UTF8
[System.Windows.Forms.Clipboard]::SetText($text)
Start-Sleep -Milliseconds 200
[System.Windows.Forms.SendKeys]::SendWait("^v")
`);
    try { fs.unlinkSync(promptFile); } catch {}
    await sleep(1000);

    // ─── STEP 8: Press Enter ───
    log('9. Envoi !');
    runPS(`
Add-Type -AssemblyName System.Windows.Forms
[System.Windows.Forms.SendKeys]::SendWait("{ENTER}")
`);

    log('');
    log('════════════════════════════════════════════');
    log('SUCCESS: Prompt + image envoyés à Meta AI !');
    log('La vidéo est en cours de génération.');
    log('════════════════════════════════════════════');
}

main().catch(e => { log(`Erreur: ${e.message}`); process.exit(1); });
