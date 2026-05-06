#!/usr/bin/env python3
"""
AI Worker TEST MODE: Returns different results based on image filename
This is for TESTING the C++ logic only!

Usage: python ai_worker_test.py <image_path>

Filename patterns:
- Contains "clean", "ok", "good", "ready" → Returns "READY"
- Contains "broken", "bad", "maintenance", "problem" → Returns "MAINTENANCE"
- Default: Alternates between READY and MAINTENANCE based on timestamp
"""

import sys
import os
import time

def main():
    if len(sys.argv) < 2:
        print("MAINTENANCE")
        sys.exit(0)
    
    image_path = sys.argv[1]
    
    # Validate image path
    if not os.path.exists(image_path):
        print("MAINTENANCE")
        sys.exit(0)
    
    # Get filename (lowercase for comparison)
    filename = os.path.basename(image_path).lower()
    
    # Check filename patterns
    if any(word in filename for word in ["clean", "ok", "good", "ready", "avant"]):
        print("READY")
        sys.exit(0)
    elif any(word in filename for word in ["broken", "bad", "maintenance", "problem", "apres"]):
        print("MAINTENANCE")
        sys.exit(0)
    else:
        # Default: alternate based on timestamp (for testing)
        # Even seconds = READY, Odd seconds = MAINTENANCE
        if int(time.time()) % 2 == 0:
            print("READY")
        else:
            print("MAINTENANCE")
        sys.exit(0)

if __name__ == "__main__":
    main()
