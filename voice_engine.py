#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
WasteGuard Voice Assistant Engine
Moteur de reconnaissance vocale et synthèse vocale via Python
Utilise: speech_recognition, pyttsx3
"""

import sys
import json
import threading
from datetime import datetime

# Try to import required packages (independent flags)
try:
    import speech_recognition as sr
    SR_OK = True
    SR_ERROR = ""
except ImportError as e:
    SR_OK = False
    SR_ERROR = str(e)

try:
    import pyttsx3
    TTS_OK = True
    TTS_ERROR = ""
except ImportError as e:
    TTS_OK = False
    TTS_ERROR = str(e)

try:
    import sounddevice as sd
    import numpy as np
    SOUNDDEVICE_OK = True
except ImportError:
    SOUNDDEVICE_OK = False

# Configuration
LANGUAGE = 'fr-FR'
TIMEOUT_SECONDS = 10
PHRASE_TIME_LIMIT = 15

# Global state
engine = None
recognizer = None
microphone = None
listening = False


def log_print(msg, level="INFO"):
    """Print with timestamp"""
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {level}: {msg}", flush=True)


def init_engine():
    """Initialize TTS engine"""
    global engine
    try:
        if not TTS_OK:
            return False, f"Missing TTS dependency: {TTS_ERROR}"
        
        engine = pyttsx3.init()
        engine.setProperty('rate', 150)
        engine.setProperty('volume', 0.9)
        
        # Try to set French voice
        try:
            voices = engine.getProperty('voices')
            for voice in voices:
                if 'fr' in voice.languages[0].lower():
                    engine.setProperty('voice', voice.id)
                    break
        except:
            pass
        
        return True, "Engine initialized"
    except Exception as e:
        return False, str(e)


def init_recognizer():
    """Initialize speech recognition"""
    global recognizer, microphone
    try:
        if not SR_OK:
            return False, f"Missing speech recognition dependency: {SR_ERROR}"
        
        recognizer = sr.Recognizer()
        recognizer.energy_threshold = 4000
        microphone = None

        # Preferred backend: SpeechRecognition Microphone (PyAudio).
        try:
            microphone = sr.Microphone()
            # Quick test
            with microphone as source:
                recognizer.listen(source, timeout=0.5)
            return True, "Recognizer initialized (pyaudio)"
        except Exception:
            microphone = None

        # Fallback backend for Python versions where PyAudio wheels are unavailable.
        if SOUNDDEVICE_OK:
            return True, "Recognizer initialized (sounddevice fallback)"

        return False, "No microphone backend available (install PyAudio or sounddevice+numpy)"
    except sr.MicrophoneIndexError:
        return False, "No microphone found"
    except sr.RequestError as e:
        return False, f"Microphone error: {str(e)}"
    except:
        # Timeout is OK during test
        return True, "Recognizer initialized"


def speak(text):
    """Convert text to speech"""
    global engine
    try:
        if engine is None:
            ok, _ = init_engine()
            if not ok:
                return False
        
        log_print(f"Speaking: {text[:50]}...")
        engine.say(text)
        engine.runAndWait()
        return True
    except Exception as e:
        log_print(f"Speak error: {str(e)}", "ERROR")
        return False


def listen():
    """Listen and return recognized text"""
    global recognizer, microphone
    try:
        if recognizer is None:
            ok, _ = init_recognizer()
            if not ok:
                return None

        audio = None
        if microphone is not None:
            log_print("Listening (pyaudio)...")
            with microphone as source:
                recognizer.adjust_for_ambient_noise(source, duration=0.5)
                audio = recognizer.listen(source, timeout=TIMEOUT_SECONDS, phrase_time_limit=PHRASE_TIME_LIMIT)
        elif SOUNDDEVICE_OK:
            log_print("Listening (sounddevice fallback)...")
            sample_rate = 16000
            duration = min(PHRASE_TIME_LIMIT, 8)
            recording = sd.rec(int(duration * sample_rate), samplerate=sample_rate, channels=1, dtype='int16')
            sd.wait()
            audio_bytes = recording.tobytes()
            audio = sr.AudioData(audio_bytes, sample_rate, 2)
        else:
            log_print("No microphone backend available", "ERROR")
            return None

        text = recognizer.recognize_google(audio, language=LANGUAGE)
        log_print(f"Recognized: {text}")
        return text
    except sr.UnknownValueError:
        return None
    except sr.RequestError as e:
        log_print(f"API error: {str(e)}", "ERROR")
        return None
    except Exception as e:
        log_print(f"Listen error: {str(e)}", "ERROR")
        return None


def main():
    """Main function for one-shot commands or persistent command loop."""
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == "init":
            ok_e, msg_e = init_engine()
            ok_r, msg_r = init_recognizer()
            result = {"engine": ok_e, "recognize": ok_r}
            print(json.dumps(result), flush=True)
        
        elif command == "listen":
            ok, _ = init_recognizer()
            text = listen()
            if text:
                print(json.dumps({"ok": True, "text": text}), flush=True)
            else:
                print(json.dumps({"ok": False}), flush=True)
        
        elif command == "speak":
            if len(sys.argv) > 2:
                text = " ".join(sys.argv[2:])
                ok, _ = init_engine()
                speak(text)
                print(json.dumps({"ok": True}), flush=True)
        
        else:
            print(json.dumps({"error": "Unknown command"}), flush=True)
    else:
        # Persistent mode expected by C++ VoiceAssistant.
        # Read commands from stdin and respond using text protocol.
        ok_e, msg_e = init_engine()
        ok_r, msg_r = init_recognizer()

        if not (ok_e and ok_r):
            log_print(f"Engine: {msg_e}, Recognizer: {msg_r}", "ERROR")

        log_print("Voice engine command loop ready", "SUCCESS")

        for raw in sys.stdin:
            cmd = raw.strip()
            if not cmd:
                continue

            if cmd == "START_LISTENING":
                text = listen()
                if text:
                    print(f"RECOGNIZED:{text}", flush=True)
                else:
                    print("ERROR:Aucune voix detectee", flush=True)

            elif cmd == "STOP_LISTENING":
                # Listening is one-shot in this implementation.
                print("LISTENING_STOPPED", flush=True)

            elif cmd.startswith("SPEAK:"):
                text = cmd[6:].strip()
                if text:
                    if speak(text):
                        print("SPEECH_DONE", flush=True)
                    else:
                        print("ERROR:Echec synthese vocale", flush=True)
                else:
                    print("ERROR:Texte vide", flush=True)

            elif cmd == "PING":
                print("PONG", flush=True)

            else:
                print("ERROR:Commande inconnue", flush=True)


if __name__ == "__main__":
    main()
