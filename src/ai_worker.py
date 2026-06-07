#!/usr/bin/env python3
"""
AI worker for maintenance images.

Priority:
1) YOLO model inference (if ultralytics + model are available)
2) Fallback simple image heuristics (OpenCV/Pillow/filename)

Output is always one of: READY | MAINTENANCE | UNRELATED
"""

import os
import statistics
import sys


def _is_document_like_cv(img, gray) -> bool:
    import cv2
    import numpy as np

    if img is None or gray is None or gray.size == 0:
        return False

    total = float(gray.size)
    white_ratio = float(np.count_nonzero(gray >= 238)) / total

    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    sat_mean = float(np.mean(hsv[:, :, 1]))

    edges = cv2.Canny(gray, 60, 160)
    edge_density = float(np.count_nonzero(edges)) / total

    _, text_mask = cv2.threshold(gray, 205, 255, cv2.THRESH_BINARY_INV)
    row_activity = np.mean(text_mask > 0, axis=1)
    text_rows_ratio = float(np.mean(row_activity > 0.06))

    return (
        white_ratio >= 0.72
        and sat_mean <= 40.0
        and edge_density <= 0.10
        and text_rows_ratio >= 0.03
    )


def _is_document_like_path(image_path: str) -> bool:
    try:
        import cv2

        img = cv2.imread(image_path)
        if img is None:
            return False
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        return _is_document_like_cv(img, gray)
    except Exception:
        return False


def _simple_fallback(image_path: str) -> str:
    try:
        import cv2
        import numpy as np

        img = cv2.imread(image_path)
        if img is None:
            return "MAINTENANCE"

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        avg_brightness = float(np.mean(gray))
        variance = float(cv2.Laplacian(gray, cv2.CV_64F).var())

        if _is_document_like_cv(img, gray):
            return "UNRELATED"

        if avg_brightness < 50 or avg_brightness > 200:
            return "MAINTENANCE"
        if variance < 100:
            return "MAINTENANCE"
        return "READY"
    except Exception:
        pass

    try:
        from PIL import Image

        img = Image.open(image_path)
        pixels = list(img.convert("L").getdata())
        avg_brightness = statistics.mean(pixels) if pixels else 0
        white_ratio = (sum(1 for p in pixels if p >= 245) / len(pixels)) if pixels else 0
        if white_ratio >= 0.82:
            return "UNRELATED"
        return "READY" if 50 <= avg_brightness <= 200 else "MAINTENANCE"
    except Exception:
        pass

    filename = os.path.basename(image_path).lower()
    if any(word in filename for word in ["clean", "ok", "good", "ready"]):
        return "READY"
    if any(word in filename for word in ["broken", "bad", "maintenance", "problem"]):
        return "MAINTENANCE"
    if any(word in filename for word in ["certificat", "medical", "doc", "document", "scan", "pdf"]):
        return "UNRELATED"
    return "READY"


def _resolve_model_path() -> str:
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [
        os.path.join(here, "models", "bin_detector.onnx"),
        os.path.join(os.getcwd(), "models", "bin_detector.onnx"),
    ]
    for path in candidates:
        if os.path.exists(path):
            return path
    return ""


def _analyze_with_yolo(image_path: str):
    try:
        from ultralytics import YOLO
    except Exception:
        return None

    model_path = _resolve_model_path()
    if not model_path:
        return None

    try:
        model = YOLO(model_path)
        results = model.predict(source=image_path, conf=0.5, verbose=False)
        if not results:
            return "UNRELATED" if _is_document_like_path(image_path) else "MAINTENANCE"

        result = results[0]
        boxes = getattr(result, "boxes", None)
        if boxes is None:
            return "UNRELATED" if _is_document_like_path(image_path) else "MAINTENANCE"

        for box in boxes:
            conf = float(box.conf[0]) if hasattr(box.conf, "__len__") else float(box.conf)
            if conf > 0.5:
                return "READY"
        return "UNRELATED" if _is_document_like_path(image_path) else "MAINTENANCE"
    except Exception:
        return None


def main():
    if len(sys.argv) < 2:
        print("MAINTENANCE")
        return

    image_path = sys.argv[1]
    if not os.path.exists(image_path):
        print("MAINTENANCE")
        return

    yolo_result = _analyze_with_yolo(image_path)
    if yolo_result in {"READY", "MAINTENANCE", "UNRELATED"}:
        print(yolo_result)
        return

    print(_simple_fallback(image_path))


if __name__ == "__main__":
    main()
