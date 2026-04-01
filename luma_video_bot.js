'use strict';

const { spawn, execSync } = require('child_process');
const path = require('path');
const fs = require('fs');

function log(msg) {
  const time = new Date().toLocaleTimeString('fr-FR', { hour12: false });
  console.log(`[${time}] ${msg}`);
}

function sleep(ms) { return new Promise(r => setTimeout(r, ms)); }

function runPS(scriptContent) {
  const tmpFile = path.join(__dirname, '_tmp_luma.ps1');
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

async function main() {
  let chromium;
  try {
    ({ chromium } = require('playwright'));
  } catch (e) {
    log('ERREUR: Playwright n\'est pas installé.');
    log('Installez-le avec: npm i playwright');
    process.exit(1);
  }

  const args = parseArgs();
  const imagePath = args.image_path ? path.resolve(args.image_path) : null;
  const prompt = buildPrompt(args) || '';

  log(`Prompt: ${prompt.substring(0, 60)}...`);
  if (imagePath) log(`Image: ${imagePath}`);

  const lumaUrl = process.env.LUMA_URL || 'https://app.lumalabs.ai/board/2e466553-239c-4239-829c-b1b2a6569775';
  const headless = process.env.LUMA_HEADLESS === '1';
  const useUi = process.env.LUMA_USE_UI === '1';
  const setupOnly = process.env.LUMA_SETUP_ONLY === '1';

  const chromeUserDataDir = process.env.LUMA_CHROME_USER_DATA_DIR ||
    (process.env.LOCALAPPDATA ? path.join(process.env.LOCALAPPDATA, 'Google', 'Chrome', 'User Data') : '');
  const chromeProfileName = process.env.LUMA_CHROME_PROFILE || 'rayeng';
  const useChrome = process.env.LUMA_USE_CHROME !== '0';
  const useCdp = process.env.LUMA_USE_CDP !== '0';
  const cdpPort = parseInt(process.env.LUMA_CDP_PORT || '9222', 10);
  const killChrome = process.env.LUMA_KILL_CHROME === '1';

  function findChromeExe() {
    const candidates = [
      process.env.CHROME_PATH,
      process.env.LOCALAPPDATA ? path.join(process.env.LOCALAPPDATA, 'Google', 'Chrome', 'Application', 'chrome.exe') : '',
      'C:\\\\Program Files\\\\Google\\\\Chrome\\\\Application\\\\chrome.exe',
      'C:\\\\Program Files (x86)\\\\Google\\\\Chrome\\\\Application\\\\chrome.exe'
    ].filter(Boolean);
    return candidates.find(p => fs.existsSync(p)) || '';
  }

  function resolveProfileDir(userDataDir, profileName) {
    if (!profileName) return 'Default';
    if (profileName === 'Default' || /^Profile\\s+\\d+$/i.test(profileName)) return profileName;
    try {
      const localStatePath = path.join(userDataDir, 'Local State');
      const raw = fs.readFileSync(localStatePath, 'utf8');
      const data = JSON.parse(raw);
      const info = data && data.profile && data.profile.info_cache ? data.profile.info_cache : {};
      for (const [dir, meta] of Object.entries(info)) {
        if ((meta && meta.name || '').toLowerCase() === profileName.toLowerCase()) {
          return dir;
        }
      }
    } catch (e) {
      // fall back below
    }
    return profileName;
  }

  async function waitForCdpReady(port, timeoutMs = 20000) {
    const http = require('http');
    const started = Date.now();
    return new Promise((resolve) => {
      const tick = () => {
        if (Date.now() - started > timeoutMs) return resolve(false);
        const req = http.get({ host: '127.0.0.1', port, path: '/json/version' }, res => {
          res.on('data', () => {});
          res.on('end', () => resolve(res.statusCode === 200));
        });
        req.on('error', () => setTimeout(tick, 500));
        req.end();
      };
      tick();
    });
  }

  function clickAtAbs(x, y) {
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
Start-Sleep -Milliseconds 120
[C]::mouse_event(0x0002, 0, 0, 0, 0)
[C]::mouse_event(0x0004, 0, 0, 0, 0)
`);
  }

  function findLumaWindow() {
    const out = runPS(`
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class U {
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
}
public struct RECT { public int Left, Top, Right, Bottom; }
"@
$hwnd = $null
foreach ($p in Get-Process | Where-Object { $_.MainWindowHandle -ne 0 }) {
  if ($p.MainWindowTitle -match "Luma") { $hwnd = $p.MainWindowHandle; break }
}
if ($hwnd -eq $null) { Write-Host "NOT_FOUND"; exit }
[U]::ShowWindow($hwnd, 9) | Out-Null
Start-Sleep -Milliseconds 100
[U]::SetForegroundWindow($hwnd) | Out-Null
Start-Sleep -Milliseconds 100
$r = New-Object RECT
[U]::GetWindowRect($hwnd, [ref]$r) | Out-Null
Write-Host "OK|$($r.Left)|$($r.Top)|$($r.Right - $r.Left)|$($r.Bottom - $r.Top)"
`);
    if (!out || out.includes('NOT_FOUND') || out === 'ERROR') return null;
    const parts = out.split('|');
    return {
      x: parseInt(parts[1], 10),
      y: parseInt(parts[2], 10),
      w: parseInt(parts[3], 10),
      h: parseInt(parts[4], 10)
    };
  }

  async function uiClickVideoAndKling() {
    const win = findLumaWindow();
    if (!win) {
      log('ERREUR: Fenêtre Luma introuvable.');
      return;
    }
    log(`Fenêtre Luma: ${win.w}x${win.h} à (${win.x},${win.y})`);
    // Exactly TWO clicks: video icon (3rd button), then Kling 3.0
    const videoRx = 0.32;
    const videoRy = 0.93;
    clickAtAbs(win.x + Math.floor(win.w * videoRx), win.y + Math.floor(win.h * videoRy));
    await sleep(700);

    const klingRx = 0.34;
    const klingRy = 0.84;
    clickAtAbs(win.x + Math.floor(win.w * klingRx), win.y + Math.floor(win.h * klingRy));
    await sleep(800);
  }

  if (useUi) {
    const chromeExe = findChromeExe();
    const profileDir = resolveProfileDir(chromeUserDataDir, chromeProfileName);
    if (!chromeExe) {
      log('ERREUR: chrome.exe introuvable.');
      process.exit(1);
    }
    log(`Ouverture de Chrome (profil: ${profileDir})...`);
    spawn(chromeExe, [
      `--user-data-dir=${chromeUserDataDir}`,
      `--profile-directory=${profileDir}`,
      '--new-window',
      lumaUrl
    ], { detached: true, stdio: 'ignore' }).unref();
    await sleep(6500);
    await uiClickVideoAndKling();

    if (!setupOnly) {
      const win = findLumaWindow();
      if (!win) {
        log('ERREUR: Fenêtre Luma introuvable pour upload.');
        return;
      }

      const plusRx = parseFloat(process.env.LUMA_PLUS_RX || '0.59');
      const plusRy = parseFloat(process.env.LUMA_PLUS_RY || '0.40');
      const promptRx = parseFloat(process.env.LUMA_PROMPT_RX || '0.70');
      const promptRy = parseFloat(process.env.LUMA_PROMPT_RY || '0.66');

      if (imagePath && fs.existsSync(imagePath)) {
        log('Clic sur le bouton + (Start Frame)...');
        clickAtAbs(win.x + Math.floor(win.w * plusRx), win.y + Math.floor(win.h * plusRy));
        await sleep(1200);

        log('Sélection de l\'image via dialogue Windows...');
        const pathFile = path.join(__dirname, '_tmp_path.txt');
        fs.writeFileSync(pathFile, imagePath, 'utf-8');
        runPS(`
Add-Type -AssemblyName System.Windows.Forms
Start-Sleep -Milliseconds 500
$imgPath = Get-Content -Path '${pathFile.replace(/'/g, "''")}' -Raw -Encoding UTF8
[System.Windows.Forms.Clipboard]::SetText($imgPath.Trim())
Start-Sleep -Milliseconds 200
[System.Windows.Forms.SendKeys]::SendWait("^v")
Start-Sleep -Milliseconds 300
[System.Windows.Forms.SendKeys]::SendWait("{ENTER}")
`);
        try { fs.unlinkSync(pathFile); } catch {}
        await sleep(1500);
      }

      log('Saisie du prompt...');
      clickAtAbs(win.x + Math.floor(win.w * promptRx), win.y + Math.floor(win.h * promptRy));
      await sleep(400);
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
    }

    log('UI automation terminée.');
    return;
  }

  let context;
  if (useChrome && chromeUserDataDir && fs.existsSync(chromeUserDataDir) && useCdp) {
    const profileDir = resolveProfileDir(chromeUserDataDir, chromeProfileName);
    const chromeExe = findChromeExe();
    log(`Ouverture de Chrome (profil: ${profileDir})...`);
    log(`Chrome user-data-dir: ${chromeUserDataDir}`);
    log('Si Chrome est déjà ouvert avec ce profil, fermez-le avant de continuer.');

    if (!chromeExe) {
      log('ERREUR: chrome.exe introuvable. Utilisation de Chromium Playwright.');
    } else {
      const argsChrome = [
        `--remote-debugging-port=${cdpPort}`,
        `--user-data-dir=${chromeUserDataDir}`,
        `--profile-directory=${profileDir}`,
        '--no-first-run',
        '--no-default-browser-check',
        '--new-window',
        lumaUrl
      ];
      const launchAndAttach = async () => {
        spawn(chromeExe, argsChrome, { detached: true, stdio: 'ignore' }).unref();
        const ready = await waitForCdpReady(cdpPort, 25000);
        if (!ready) return false;
        try {
          const browser = await chromium.connectOverCDP(`http://127.0.0.1:${cdpPort}`);
          context = browser.contexts()[0];
          return true;
        } catch (e) {
          log(`ERREUR: impossible de se connecter à Chrome via CDP (${e.message}).`);
          return false;
        }
      };

      let attached = await launchAndAttach();
      if (!attached && killChrome) {
        log('Chrome semble déjà ouvert. Fermeture forcée des processus Chrome...');
        try {
          require('child_process').execSync('taskkill /F /IM chrome.exe', { stdio: 'ignore' });
        } catch {}
        await sleep(1500);
        attached = await launchAndAttach();
      }
      if (!attached) {
        log(`ERREUR: Chrome n'a pas exposé le port CDP ${cdpPort}. Fermez Chrome et relancez.`);
        process.exit(1);
      }
    }
  }

  if (!context) {
    if (useChrome && chromeUserDataDir && fs.existsSync(chromeUserDataDir)) {
      const profileDir = resolveProfileDir(chromeUserDataDir, chromeProfileName);
      log(`Ouverture de Chrome (profil: ${profileDir})...`);
      context = await chromium.launchPersistentContext(chromeUserDataDir, {
        channel: 'chrome',
        headless,
        viewport: { width: 1280, height: 800 },
        args: [`--profile-directory=${profileDir}`]
      });
    } else {
      const userDataDir = path.join(__dirname, 'luma_profile');
      log('Ouverture de Luma Labs (Playwright Chromium)...');
      context = await chromium.launchPersistentContext(userDataDir, {
        headless,
        viewport: { width: 1280, height: 800 }
      });
    }
  }

  let page = context.pages()[0];
  if (!page) page = await context.newPage();

  if (page.url() === 'about:blank' || !page.url()) {
    await page.goto(lumaUrl, { waitUntil: 'domcontentloaded' });
  } else {
    await page.goto(lumaUrl, { waitUntil: 'domcontentloaded' });
  }
  await page.waitForLoadState('networkidle').catch(() => {});

  async function clickCreateVideoIcon() {
    // Try by accessible name
    const byName = page.getByRole('button', { name: /video/i }).first();
    if (await byName.count().catch(() => 0)) {
      await byName.click().catch(() => {});
      return true;
    }
    const byLabel = page.locator('button[aria-label*="video" i], button[title*="video" i]').first();
    if (await byLabel.count().catch(() => 0)) {
      await byLabel.click().catch(() => {});
      return true;
    }

    // Fallback: click by relative position (bottom toolbar, 3rd icon)
    const vp = page.viewportSize() || { width: 1280, height: 800 };
    const candidates = [
      [0.20, 0.93],
      [0.22, 0.93],
      [0.18, 0.93],
      [0.20, 0.90]
    ];
    for (const [rx, ry] of candidates) {
      await page.mouse.click(Math.floor(vp.width * rx), Math.floor(vp.height * ry));
      await page.waitForTimeout(300);
    }
    return false;
  }

  async function clickKling() {
    const kling = page.getByText(/Kling 3\.0/i).first();
    await kling.waitFor({ timeout: 8000 });
    await kling.click();
  }

  // --- Step 1: click Video icon, then Kling 3.0 ---
  log('Clic sur l’icône vidéo (3e bouton)...');
  await clickCreateVideoIcon();
  await page.waitForTimeout(800);
  log('Sélection de "Kling 3.0"...');
  await clickKling();

  if (setupOnly) {
    log('Mode setup-only: arrêt après sélection Kling 3.0.');
    return;
  }

  // If login is required, wait for user to login
  const loginText = page.getByText(/log in|sign in|connexion/i).first();
  if (await loginText.isVisible().catch(() => false)) {
    log('Connexion requise. Merci de vous connecter dans la fenêtre ouverte...');
  }

  // Wait for a prompt input (textarea or contenteditable)
  const promptInput = page.locator('textarea').first();
  await promptInput.waitFor({ timeout: 180000 });

  if (imagePath && fs.existsSync(imagePath)) {
    // Try to use file input directly
    const fileInput = page.locator('input[type="file"]').first();
    if (await fileInput.count() > 0) {
      log('Upload image via input[type=file]');
      await fileInput.setInputFiles(imagePath);
    } else {
      // Fallback: click an upload button by text
      log('Upload image via bouton');
      const uploadBtn = page.getByRole('button', { name: /upload|image|media|ajouter|importer/i }).first();
      await uploadBtn.click({ timeout: 5000 });
      const fileChooser = await page.waitForEvent('filechooser', { timeout: 10000 }).catch(() => null);
      if (fileChooser) {
        await fileChooser.setFiles(imagePath);
      }
    }
  } else {
    log('Aucune image fournie — génération texte uniquement.');
  }

  log('Saisie du prompt...');
  await promptInput.click();
  await promptInput.fill(prompt);

  // Click generate / create
  const generateBtn = page.getByRole('button', { name: /generate|create|créer|generer|générer/i }).first();
  await generateBtn.click({ timeout: 10000 });

  log('Génération lancée.');
}

main().catch(err => {
  log(`Erreur: ${err.message}`);
  process.exit(1);
});
