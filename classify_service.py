"""
Persistent waste-classification daemon used by the Qt app.

The Qt MainWindow starts this script once (long-running QProcess) and then
sends commands on stdin. The script keeps the HuggingFace pipeline loaded
in memory so each classification round-trip is fast (~300 ms vs ~10 s for
the cold-spawn classify_single.py).

Protocol (line-based, both directions):
    stdin  -> CLASSIFY                         capture + classify
    stdin  -> URL <full-url>                   override the camera URL at runtime
    stdin  -> QUIT                             clean shutdown

    stdout <- LOADING_MODEL                    emitted right at startup
    stdout <- READY                            model is loaded, ready for CLASSIFY
    stdout <- AI_RESULT:<MAT>:<label>:<score>  e.g. AI_RESULT:PLASTIC:plastic:0.83
    stdout <- AI_ERROR:<reason>                anything that goes wrong

The Qt app only cares about LOADING_MODEL / READY / AI_RESULT / AI_ERROR
prefixes; the trailing fields after AI_RESULT are informational.
"""

import os
import sys
import requests
import numpy as np
from PIL import Image
from transformers import pipeline


URL = os.environ.get("ESP32_CAM_URL", "http://10.43.149.88/capture.raw")
WIDTH = 160
HEIGHT = 120


def emit(line: str) -> None:
    """Write a line to stdout and flush so the Qt app sees it immediately."""
    sys.stdout.write(line + "\n")
    sys.stdout.flush()


def convert_rgb565(raw: bytes) -> np.ndarray:
    data = np.frombuffer(raw, dtype=np.uint16).byteswap().reshape((HEIGHT, WIDTH))
    r = ((data >> 11) & 0x1F) << 3
    g = ((data >> 5) & 0x3F) << 2
    b = (data & 0x1F) << 3
    return np.dstack((r, g, b)).astype(np.uint8)


def map_to_bin(label: str) -> str:
    """Maps the model's free-form label to one of the 4 bin codes the
    Arduino sketch understands (PLASTIC=0, METAL=1, GLASS=2, GENERAL_WASTE=3).
    Cardboard / paper / organic / trash all collapse into GENERAL_WASTE
    because the physical bin layout only has 4 compartments."""
    s = label.lower()
    if "plastic" in s:
        return "PLASTIC"
    if "metal" in s:
        return "METAL"
    if "glass" in s:
        return "GLASS"
    return "GENERAL_WASTE"


def classify(classifier, url: str) -> None:
    try:
        resp = requests.get(url, timeout=3)
    except requests.exceptions.RequestException as e:
        emit(f"AI_ERROR:cam-unreachable:{e}")
        return

    raw = resp.content
    expected = WIDTH * HEIGHT * 2
    if len(raw) != expected:
        emit(f"AI_ERROR:wrong-size:{len(raw)}")
        return

    try:
        rgb = convert_rgb565(raw)
        img = Image.fromarray(rgb, "RGB").resize((224, 224))
        results = classifier(img)
    except Exception as e:
        emit(f"AI_ERROR:inference:{e}")
        return

    best = results[0]
    label = best["label"]
    score = float(best["score"])
    final = map_to_bin(label)
    emit(f"AI_RESULT:{final}:{label}:{score:.3f}")


def main() -> int:
    global URL

    emit("LOADING_MODEL")
    try:
        classifier = pipeline(
            "image-classification",
            model="watersplash/waste-classification",
        )
    except Exception as e:
        emit(f"AI_ERROR:model-load:{e}")
        return 1
    emit("READY")

    for raw_line in sys.stdin:
        cmd = raw_line.strip()
        if not cmd:
            continue
        if cmd == "CLASSIFY":
            classify(classifier, URL)
        elif cmd.startswith("URL "):
            URL = cmd[4:].strip()
            emit(f"URL_OK:{URL}")
        elif cmd == "QUIT":
            break
        else:
            emit(f"AI_ERROR:unknown-command:{cmd}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
