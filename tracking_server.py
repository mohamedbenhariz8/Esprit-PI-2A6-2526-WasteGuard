import os
import json
import time
import subprocess
import urllib.request
import threading
from http.server import HTTPServer, BaseHTTPRequestHandler
from pathlib import Path


def _resolve_base_dir() -> Path:
    env_base = os.environ.get("TRACKING_BASE_DIR", "").strip()
    if env_base:
        p = Path(env_base).expanduser()
        if p.exists():
            return p
    return Path(__file__).resolve().parent


BASE_DIR = _resolve_base_dir()
DATA_FILE = Path(os.environ.get("TRACKING_DATA_FILE", str(BASE_DIR / "delivery_tracking.json")))
URL_FILE = Path(os.environ.get("TRACKING_URL_FILE", str(BASE_DIR / "ngrok_url.txt")))
HTML_FILE = Path(os.environ.get("TRACKING_HTML_FILE", str(BASE_DIR / "tracking.html")))
PORT = int(os.environ.get("TRACKING_PORT", "5050"))

LOCATION_DATA = {
    "current": None,
    "history": []
}


def save_to_file():
    DATA_FILE.parent.mkdir(parents=True, exist_ok=True)
    with DATA_FILE.open("w", encoding="utf-8") as f:
        json.dump(LOCATION_DATA, f)


class TrackingHandler(BaseHTTPRequestHandler):
    def _set_headers(self, status_code=200, content_type='application/json'):
        self.send_response(status_code)
        self.send_header('Content-type', content_type)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()

    def do_OPTIONS(self):
        self._set_headers()

    def do_GET(self):
        if self.path == '/':
            self._set_headers(content_type='text/html')
            try:
                with HTML_FILE.open('rb') as f:
                    self.wfile.write(f.read())
            except Exception as e:
                self.wfile.write(f"Error reading HTML: {e}".encode())
        elif self.path == '/get_location':
            self._set_headers()
            self.wfile.write(json.dumps(LOCATION_DATA).encode())
        else:
            self.send_response(404)
            self.end_headers()

    def do_POST(self):
        if self.path == '/update_location':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))

            if "lat" in data and "lng" in data:
                loc = {"lat": data["lat"], "lng": data["lng"]}
                LOCATION_DATA["current"] = loc
                LOCATION_DATA["history"].append(loc)
                save_to_file()

                self._set_headers()
                self.wfile.write(json.dumps({"status": "success", "recorded": loc}).encode())
            else:
                self._set_headers(400)
                self.wfile.write(json.dumps({"status": "error"}).encode())


def start_ngrok():
    try:
        def fetch_public_url():
            for api_port in (4040, 4041, 4042):
                try:
                    req = urllib.request.Request(f"http://127.0.0.1:{api_port}/api/tunnels")
                    with urllib.request.urlopen(req, timeout=2) as response:
                        data = json.loads(response.read().decode())
                        tunnels = data.get("tunnels", [])
                        if tunnels and tunnels[0].get("public_url"):
                            return tunnels[0]["public_url"]
                except Exception:
                    continue
            return ""

        try:
            subprocess.Popen(["ngrok", "http", str(PORT)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except FileNotFoundError:
            try:
                subprocess.Popen(["ngrok.exe", "http", str(PORT)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            except FileNotFoundError:
                URL_FILE.parent.mkdir(parents=True, exist_ok=True)
                with URL_FILE.open("w", encoding="utf-8") as f:
                    f.write("ERREUR : ngrok.exe est introuvable. Telechargez-le !")
                return

        time.sleep(3)
        public_url = fetch_public_url()

        URL_FILE.parent.mkdir(parents=True, exist_ok=True)
        if public_url:
            with URL_FILE.open("w", encoding="utf-8") as f:
                f.write(public_url)
            print(f"Ngrok URL saved to {URL_FILE}: {public_url}")
        else:
            with URL_FILE.open("w", encoding="utf-8") as f:
                f.write("ERREUR ngrok : impossible de recuperer l'URL publique.")
    except Exception as e:
        URL_FILE.parent.mkdir(parents=True, exist_ok=True)
        msg = str(e) if str(e).strip() else repr(e)
        with URL_FILE.open("w", encoding="utf-8") as f:
            f.write(f"ERREUR ngrok : {msg}")
        print(f"Failed to start ngrok or get URL: {msg}")


if __name__ == "__main__":
    print(f"Demarrage du serveur de tracking sur le port {PORT}...")
    threading.Thread(target=start_ngrok, daemon=True).start()

    server_address = ('0.0.0.0', PORT)
    httpd = HTTPServer(server_address, TrackingHandler)
    httpd.serve_forever()
