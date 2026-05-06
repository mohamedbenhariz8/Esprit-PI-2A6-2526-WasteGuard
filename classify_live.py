import requests
import numpy as np
import cv2
import time
from PIL import Image
from transformers import pipeline

URL = "http://192.168.1.18/capture.raw"

WIDTH = 160
HEIGHT = 120

classifier = pipeline(
    "image-classification",
    model="watersplash/waste-classification"
)

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

    if "cardboard" in label:
        return "CARDBOARD"
    if "metal" in label:
        return "METAL"
    if "glass" in label:
        return "GLASS"
    return "GENERAL_WASTE"

last_classify_time = 0
current_text = "Waiting..."

while True:
    try:
        response = requests.get(URL, timeout=3)
        raw = response.content

        if len(raw) != WIDTH * HEIGHT * 2:
            print("Wrong size:", len(raw))
            continue

        rgb = convert_rgb565(raw)

        now = time.time()

        # classify once every 2 seconds
        if now - last_classify_time > 2:
            pil_img = Image.fromarray(rgb, "RGB").resize((224, 224))
            results = classifier(pil_img)

            best = results[0]
            label = best["label"]
            score = best["score"]
            final_bin = map_to_bin(label)

            current_text = f"{final_bin} ({label}: {score:.2f})"
            print(current_text)

            last_classify_time = now

        frame = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)
        frame = cv2.resize(frame, (320, 240), interpolation=cv2.INTER_NEAREST)

        cv2.putText(
            frame,
            current_text,
            (10, 25),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            (255, 255, 255),
            1,
            cv2.LINE_AA
        )

        cv2.imshow("Waste Classification Live", frame)

        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

        time.sleep(0.1)

    except Exception as e:
        print("Error:", e)
        time.sleep(1)

cv2.destroyAllWindows()