import os
import sys
import requests
import numpy as np
from PIL import Image
from transformers import pipeline

# URL is taken from the ESP32_CAM_URL env var (set by MainWindow to match
# whatever IP the bin-status dialog is polling). The hardcoded value is
# only a fallback when the script is run by hand.
URL = os.environ.get("ESP32_CAM_URL", "http://10.43.149.88/capture.raw")
WIDTH = 160
HEIGHT = 120

def convert_rgb565(raw):
    data = np.frombuffer(raw, dtype=np.uint16)
    data = data.byteswap()
    data = data.reshape((HEIGHT, WIDTH))

    r = ((data >> 11) & 0x1F) << 3
    g = ((data >> 5) & 0x3F) << 2
    b = (data & 0x1F) << 3

    rgb = np.dstack((r, g, b)).astype(np.uint8)
    return rgb

def map_to_bin(label):
    label = label.lower()
    if "plastic" in label: return "PLASTIC"
    if "metal" in label: return "METAL"
    if "glass" in label: return "GLASS"
    return "GENERAL_WASTE"

def main():
    try:
        response = requests.get(URL, timeout=3)
        raw = response.content

        if len(raw) != WIDTH * HEIGHT * 2:
            print(f"ERROR: Wrong size: {len(raw)}", file=sys.stderr)
            sys.exit(1)

        rgb = convert_rgb565(raw)

        # Load classifier
        classifier = pipeline("image-classification", model="watersplash/waste-classification")
        
        pil_img = Image.fromarray(rgb, "RGB").resize((224, 224))
        results = classifier(pil_img)

        best = results[0]
        final_bin = map_to_bin(best["label"])

        # Print ONLY the bin name so the Qt app can easily parse it
        print(f"AI_RESULT:{final_bin}")
        sys.exit(0)

    except requests.exceptions.RequestException:
        print("Erreur: Impossible de se connecter a l'ESP32-CAM", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        err_msg = str(e)
        if len(err_msg) > 60:
            err_msg = err_msg[:57] + "..."
        print(f"Erreur IA: {err_msg}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
