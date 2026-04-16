#!/usr/bin/env python3
"""
Hand Tracker for WasteGuard 3D Viewer
-------------------------------------
Uses MediaPipe Hands to detect hand gestures and output JSON commands
to stdout for the Qt application to read.

Gestures:
  - Index finger only                            -> Draw
  - Open hand                                    -> Erase
  - Pinch (thumb + index tips close) + movement  -> Rotate
  - Closed fist + movement                       -> Move / Pan
  - Fist / no hand                               -> Idle

Requirements:
  pip install mediapipe opencv-python

Usage:
  python -u hand_tracker.py
"""

import sys
import json
import math
import base64
import argparse

try:
    import cv2
    import mediapipe as mp
except ImportError:
    print(json.dumps({"error": "Missing dependencies. Install: pip install mediapipe opencv-python"}), flush=True)
    sys.exit(1)

mp_hands = mp.solutions.hands
mp_drawing = mp.solutions.drawing_utils
mp_styles = mp.solutions.drawing_styles

WEBCAM_INDEX = 0
FRAME_WIDTH = 640
FRAME_HEIGHT = 480
PINCH_THRESHOLD = 0.06
MOVEMENT_THRESHOLD = 0.008
SEND_FRAME_EVERY = 1
SMOOTHING = 0.45


class GestureTracker:
    def __init__(self):
        self.prev_pinch_pos = None
        self.prev_palm_pos = None
        self.frame_count = 0
        self.prev_gesture = "idle"

    def distance(self, p1, p2):
        return math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2)

    def midpoint(self, p1, p2):
        return ((p1.x + p2.x) / 2, (p1.y + p2.y) / 2)

    def palm_center(self, hand_landmarks):
        wrist = hand_landmarks.landmark[mp_hands.HandLandmark.WRIST]
        mcp = hand_landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_MCP]
        return self.midpoint(wrist, mcp)

    def is_finger_extended(self, hand_landmarks, finger_tip, finger_pip):
        tip = hand_landmarks.landmark[finger_tip]
        pip = hand_landmarks.landmark[finger_pip]
        wrist = hand_landmarks.landmark[mp_hands.HandLandmark.WRIST]
        return self.distance(tip, wrist) > self.distance(pip, wrist)

    def detect_gesture(self, hand_landmarks):
        thumb_tip = hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_TIP]
        index_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]

        index_up = self.is_finger_extended(
            hand_landmarks,
            mp_hands.HandLandmark.INDEX_FINGER_TIP,
            mp_hands.HandLandmark.INDEX_FINGER_PIP,
        )
        middle_up = self.is_finger_extended(
            hand_landmarks,
            mp_hands.HandLandmark.MIDDLE_FINGER_TIP,
            mp_hands.HandLandmark.MIDDLE_FINGER_PIP,
        )
        ring_up = self.is_finger_extended(
            hand_landmarks,
            mp_hands.HandLandmark.RING_FINGER_TIP,
            mp_hands.HandLandmark.RING_FINGER_PIP,
        )
        pinky_up = self.is_finger_extended(
            hand_landmarks,
            mp_hands.HandLandmark.PINKY_TIP,
            mp_hands.HandLandmark.PINKY_PIP,
        )

        pinch_dist = self.distance(thumb_tip, index_tip)
        result = {"gesture": "idle", "dx": 0, "dy": 0, "zoom": 0}

        if pinch_dist < (PINCH_THRESHOLD * 1.35):
            pinch_pos = self.midpoint(thumb_tip, index_tip)
            if self.prev_pinch_pos is not None:
                dx = (pinch_pos[0] - self.prev_pinch_pos[0]) * 220
                dy = (pinch_pos[1] - self.prev_pinch_pos[1]) * 220
                if abs(dx) > MOVEMENT_THRESHOLD * 100 or abs(dy) > MOVEMENT_THRESHOLD * 100:
                    result = {
                        "gesture": "rotate",
                        "dx": round(dx * SMOOTHING, 2),
                        "dy": round(dy * SMOOTHING, 2),
                        "zoom": 0,
                    }
            self.prev_pinch_pos = pinch_pos
            self.prev_palm_pos = None

        elif index_up and not middle_up and not ring_up and not pinky_up and pinch_dist > PINCH_THRESHOLD * 1.35:
            result = {
                "gesture": "draw",
                "dx": 0,
                "dy": 0,
                "zoom": 0,
                "nx": round(index_tip.x, 4),
                "ny": round(index_tip.y, 4),
            }
            self.prev_pinch_pos = None
            self.prev_palm_pos = None

        elif index_up and middle_up and ring_up and pinky_up:
            palm = self.palm_center(hand_landmarks)
            result = {
                "gesture": "erase",
                "dx": 0,
                "dy": 0,
                "zoom": 0,
                "nx": round(palm[0], 4),
                "ny": round(palm[1], 4),
            }
            self.prev_pinch_pos = None
            self.prev_palm_pos = palm

        elif not index_up and not middle_up and not ring_up and not pinky_up:
            palm = self.palm_center(hand_landmarks)
            if self.prev_palm_pos is not None:
                dx = (palm[0] - self.prev_palm_pos[0]) * 150
                dy = (palm[1] - self.prev_palm_pos[1]) * 150
                if abs(dx) > MOVEMENT_THRESHOLD * 100 or abs(dy) > MOVEMENT_THRESHOLD * 100:
                    result = {
                        "gesture": "pan",
                        "dx": round(dx * SMOOTHING, 2),
                        "dy": round(dy * SMOOTHING, 2),
                        "zoom": 0,
                    }
            self.prev_pinch_pos = None
            self.prev_palm_pos = palm

        else:
            self.prev_pinch_pos = None
            self.prev_palm_pos = None

        self.prev_gesture = result["gesture"]
        return result


def encode_frame(frame, quality=35, max_width=160):
    h, w = frame.shape[:2]
    if w <= 0 or h <= 0:
        return ""
    scale = min(1.0, max_width / float(w))
    small = cv2.resize(frame, (max(1, int(w * scale)), max(1, int(h * scale))))
    ok, buf = cv2.imencode(".jpg", small, [cv2.IMWRITE_JPEG_QUALITY, quality])
    if not ok:
        return ""
    return base64.b64encode(buf).decode("ascii")


def open_camera():
    candidates = []
    if sys.platform == "win32":
        candidates = [cv2.CAP_DSHOW, cv2.CAP_MSMF, None]
    else:
        candidates = [None]

    for backend in candidates:
        cap = cv2.VideoCapture(WEBCAM_INDEX, backend) if backend is not None else cv2.VideoCapture(WEBCAM_INDEX)
        if not cap.isOpened():
            cap.release()
            continue

        cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)
        ret, frame = cap.read()
        if ret and frame is not None:
            return cap, frame

        cap.release()

    return None, None


def main():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--show-window", action="store_true")
    args, _ = parser.parse_known_args()

    cap, first_frame = open_camera()
    if cap is None:
        print(json.dumps({"error": "Cannot open webcam"}), flush=True)
        sys.exit(1)

    tracker = GestureTracker()

    init_payload = {"gesture": "init", "message": "Hand tracker started"}
    if first_frame is not None:
        init_frame = cv2.flip(first_frame, 1)
        encoded = encode_frame(init_frame)
        if encoded:
            init_payload["frame"] = encoded
    print(json.dumps(init_payload), flush=True)

    with mp_hands.Hands(
        model_complexity=0,
        min_detection_confidence=0.6,
        min_tracking_confidence=0.5,
        max_num_hands=1,
    ) as hands:
        while True:
            try:
                if sys.platform == "win32":
                    import msvcrt

                    if msvcrt.kbhit():
                        line = sys.stdin.readline().strip()
                        if line == "QUIT":
                            break
            except Exception:
                pass

            ret, frame = cap.read()
            if not ret or frame is None:
                continue

            tracker.frame_count += 1
            frame = cv2.flip(frame, 1)

            rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            rgb.flags.writeable = False
            results = hands.process(rgb)
            rgb.flags.writeable = True

            gesture_data = {"gesture": "idle", "dx": 0, "dy": 0, "zoom": 0}

            if results.multi_hand_landmarks:
                hand_landmarks = results.multi_hand_landmarks[0]
                mp_drawing.draw_landmarks(
                    frame,
                    hand_landmarks,
                    mp_hands.HAND_CONNECTIONS,
                    mp_styles.get_default_hand_landmarks_style(),
                    mp_styles.get_default_hand_connections_style(),
                )
                gesture_data = tracker.detect_gesture(hand_landmarks)

            if tracker.frame_count % SEND_FRAME_EVERY == 0:
                encoded = encode_frame(frame)
                if encoded:
                    gesture_data["frame"] = encoded

            print(json.dumps(gesture_data), flush=True)

            if args.show_window:
                cv2.putText(
                    frame,
                    f"Gesture: {gesture_data['gesture']}",
                    (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.8,
                    (0, 255, 0),
                    2,
                )
                cv2.imshow("WasteGuard - Hand Tracker", frame)
                key = cv2.waitKey(1) & 0xFF
                if key == ord("q") or key == 27:
                    break

    cap.release()
    cv2.destroyAllWindows()
    print(json.dumps({"gesture": "quit", "message": "Hand tracker stopped"}), flush=True)


if __name__ == "__main__":
    main()
