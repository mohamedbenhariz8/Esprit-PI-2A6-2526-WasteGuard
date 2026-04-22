#!/usr/bin/env python3
"""
Fix Excel export logo: replace tiny/broken WASTEGUARD (1) (1).png
with the proper wasteguard_logo.png (125KB) and login_logo.png as fallback.
If all files fail, embed a styled SVG text logo directly in the HTML.
"""

filepath = "mainwindow.cpp"

with open(filepath, "r", encoding="utf-8") as f:
    content = f.read()

OLD_LOGO = '''    QImage logo(":/WASTEGUARD (1) (1).png");
    if (logo.isNull()) logo.load("WASTEGUARD (1) (1).png");
    if (logo.isNull()) logo.load(":/wasteguard_logo.png");
    QString base64Logo;
    if (!logo.isNull()) {
        QByteArray imgArray;
        QBuffer buffer(&imgArray);
        buffer.open(QIODevice::WriteOnly);
        logo.save(&buffer, "PNG");
        base64Logo = QString("data:image/png;base64,") + QString::fromLatin1(imgArray.toBase64());
    }'''

NEW_LOGO = '''    // Load best available logo for Excel export (in priority order)
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
        logoHtml = QString("<img src=\\\"%1\\\" height=\\\"80\\\" style=\\\"margin-bottom:8px;\\\" alt=\\\"WasteGuard Logo\\\"/>").arg(base64Logo);
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

if OLD_LOGO in content:
    content = content.replace(OLD_LOGO, NEW_LOGO)
    print("[OK] Logo load block replaced (LF)")
else:
    crlf = OLD_LOGO.replace('\n', '\r\n')
    if crlf in content:
        content = content.replace(crlf, NEW_LOGO.replace('\n', '\r\n'))
        print("[OK] Logo load block replaced (CRLF)")
    else:
        print("[ERROR] Logo load block not found")
        exit(1)

# Also replace the HTML output that uses base64Logo to use logoHtml instead
OLD_IMG_OUT = '''    if (!base64Logo.isEmpty()) {
        out << "<img src=\\"" << base64Logo << "\\" height=\\"80\\" alt=\\"WasteGuard Logo\\"/><br>\\n";
    }'''

NEW_IMG_OUT = '''    if (!logoHtml.isEmpty()) {
        out << logoHtml.toHtmlEscaped().replace("&#39;", "'") << "<br>\\n";
    }'''

# Actually logoHtml is already safe HTML, we should NOT escape it
# Let's write it differently - output the logoHtml directly
NEW_IMG_OUT = '''    if (!logoHtml.isEmpty()) {
        out << logoHtml << "<br>\\n";
    }'''

if OLD_IMG_OUT in content:
    content = content.replace(OLD_IMG_OUT, NEW_IMG_OUT)
    print("[OK] Logo HTML output replaced (LF)")
else:
    crlf = OLD_IMG_OUT.replace('\n', '\r\n')
    if crlf in content:
        content = content.replace(crlf, NEW_IMG_OUT.replace('\n', '\r\n'))
        print("[OK] Logo HTML output replaced (CRLF)")
    else:
        print("[ERROR] Logo HTML output not found")
        exit(1)

with open(filepath, "w", encoding="utf-8") as f:
    f.write(content)

print("[DONE] Excel export logo fix applied!")
