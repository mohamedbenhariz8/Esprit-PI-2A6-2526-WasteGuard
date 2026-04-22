#!/usr/bin/env python3
"""
Fix Excel export logo properly.

Excel's HTML parser does NOT support data:image/base64 URIs.
The only reliable method is:
  1. Save the logo as a real PNG file in the SAME directory as the .xls
  2. Reference it with just the filename (relative path) in the <img src>

This is the only approach that works in Microsoft Excel for HTML/XLS files.
"""

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

OLD = '''    // Load best available logo for Excel export (in priority order)
    QImage logo;
    // 1. Try the full-quality logo from Qt resources
    logo.load(":/wasteguard_logo.png");
    // 2. Try from disk next to executable
    if (logo.isNull()) logo.load("wasteguard_logo.png");
    // 3. Try login logo resource
    if (logo.isNull()) logo.load(":/login_logo.png");
    // 4. Try logo.png from disk
    if (logo.isNull()) logo.load("logo.png");

    QString base64Logo;
    QString logoHtml; // fallback SVG inline logo
    if (!logo.isNull()) {
        // Scale down if very large to keep file size reasonable
        if (logo.width() > 400 || logo.height() > 200)
            logo = logo.scaled(400, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QByteArray imgArray;
        QBuffer buffer(&imgArray);
        buffer.open(QIODevice::WriteOnly);
        logo.save(&buffer, "PNG");
        base64Logo = QString("data:image/png;base64,") + QString::fromLatin1(imgArray.toBase64());
        logoHtml = QString("<img src=\\"%1\\" height=\\"80\\" style=\\"margin-bottom:8px;\\" alt=\\"WasteGuard Logo\\"/>").arg(base64Logo);
    } else {
        // SVG text logo as reliable fallback - always renders in any browser/Excel
        logoHtml = "<svg xmlns=\\'http://www.w3.org/2000/svg\\' width=\\'260\\' height=\\'70\\'>"
                   "<rect width=\\'260\\' height=\\'70\\' rx=\\'12\\' fill=\\'#0f2b4c\\'/>"
                   "<text x=\\'130\\' y=\\'28\\' font-family=\\'Segoe UI,Arial\\' font-size=\\'13\\' "
                   "fill=\\'#34d399\\' text-anchor=\\'middle\\' font-weight=\\'bold\\'>WASTE</text>"
                   "<text x=\\'130\\' y=\\'50\\' font-family=\\'Segoe UI,Arial\\' font-size=\\'18\\' "
                   "fill=\\'white\\' text-anchor=\\'middle\\' font-weight=\\'bold\\'>GUARD</text>"
                   "<text x=\\'130\\' y=\\'63\\' font-family=\\'Segoe UI,Arial\\' font-size=\\'8\\' "
                   "fill=\\'#94a3b8\\' text-anchor=\\'middle\\'>Smart Waste Management</text>"
                   "</svg>";
    }'''

NEW = '''    // ── Excel logo: save as real PNG file next to the .xls ──────────────────
    // IMPORTANT: Excel HTML (.xls) does NOT support data:base64 URIs.
    // The only reliable method is a real file saved in the same directory,
    // referenced by filename only (relative path).
    QImage logo;
    logo.load(":/wasteguard_logo.png");
    if (logo.isNull()) logo.load("wasteguard_logo.png");
    if (logo.isNull()) logo.load(":/login_logo.png");
    if (logo.isNull()) logo.load("logo.png");

    // Scale to a reasonable display size
    if (!logo.isNull() && (logo.width() > 400 || logo.height() > 200))
        logo = logo.scaled(400, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Determine the export directory and save the logo there
    QString exportDir   = QFileInfo(fileName).absolutePath();
    QString logoFileName = "wasteguard_logo_export.png";
    QString logoFilePath = exportDir + "/" + logoFileName;

    QString logoHtml;
    if (!logo.isNull() && logo.save(logoFilePath, "PNG")) {
        // Reference by filename only — Excel finds it in the same folder as the .xls
        logoHtml = QString("<img src=\\"%1\\" height=\\"80\\" style=\\"margin-bottom:8px;\\" alt=\\"WasteGuard Logo\\"/>")
                       .arg(logoFileName);
    } else {
        // Pure HTML/CSS text fallback — always works regardless of image support
        logoHtml = "<div style=\\"display:inline-block;background:#0f2b4c;color:white;"
                   "padding:12px 28px;border-radius:10px;font-family:Segoe UI,Arial;"
                   "font-size:22px;font-weight:bold;letter-spacing:2px;\\">"
                   "<span style=\\"color:#34d399;\\">WASTE</span>GUARD</div>";
    }'''

if OLD in content:
    content = content.replace(OLD, NEW)
    print("[OK] Logo block replaced (LF)")
else:
    crlf = OLD.replace('\n', '\r\n')
    if crlf in content:
        content = content.replace(crlf, NEW.replace('\n', '\r\n'))
        print("[OK] Logo block replaced (CRLF)")
    else:
        print("[ERROR] Logo block not found - trying partial match")
        # Try to find the unique marker
        marker = 'logo.load(":/wasteguard_logo.png");\n    if (logo.isNull()) logo.load("wasteguard_logo.png");'
        if marker in content:
            print(f"  Marker found, manual inspection needed")
        else:
            marker_crlf = marker.replace('\n', '\r\n')
            if marker_crlf in content:
                print(f"  Marker found (CRLF)")
        exit(1)

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print("[DONE] Excel logo fix applied correctly!")
print("The logo will be saved as 'wasteguard_logo_export.png' in the same folder as the .xls file.")
