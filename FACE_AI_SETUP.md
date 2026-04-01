# Face AI Authentication Setup

This project now supports **AI-first face authentication**:
- Local descriptor match (offline)
- Face API verification (online) when keys are configured

## 1) Enable API mode

Set these environment variables on Windows (PowerShell):

```powershell
setx WG_FACE_API_KEY "YOUR_KEY"
setx WG_FACE_API_SECRET "YOUR_SECRET"
setx WG_FACE_API_URL "https://api-us.faceplusplus.com/facepp/v3/compare"
```

Close and reopen Qt Creator / terminal after `setx`.

## 2) Build and run

1. Clean Project
2. Rebuild All
3. Run app

When Face API keys are present, login uses:
- **API-only** verification (100% IA path)
- No local-score gate in this mode
- API confidence threshold from provider response

## 3) Notes

- If an employee has no saved reference photo, API verification cannot run for that account.
- Local mode is used only when API keys are missing.
