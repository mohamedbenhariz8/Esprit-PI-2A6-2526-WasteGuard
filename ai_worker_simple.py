#!/usr/bin/env python3
"""
Simple AI Worker: Analyzes images using basic image processing
No YOLO required - uses OpenCV for simple detection
"""

import sys
import os

def analyze_image_simple(image_path):
    """
    Simple image analysis without YOLO
    Returns READY or MAINTENANCE based on image brightness/quality
    """
    try:
        import cv2
        import numpy as np
        
        # Read image
        img = cv2.imread(image_path)
        if img is None:
            return "MAINTENANCE"
        
        # Convert to grayscale
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # Calculate average brightness
        avg_brightness = np.mean(gray)
        
        # Calculate image variance (sharpness indicator)
        variance = cv2.Laplacian(gray, cv2.CV_64F).var()
        
        # Simple heuristic:
        # - If image is too dark (avg < 50) or too bright (avg > 200) -> MAINTENANCE
        # - If image is blurry (variance < 100) -> MAINTENANCE
        # - Otherwise -> READY
        
        if avg_brightness < 50 or avg_brightness > 200:
            return "MAINTENANCE"
        
        if variance < 100:
            return "MAINTENANCE"
        
        # Image looks good
        return "READY"
        
    except ImportError:
        # OpenCV not available, use PIL as fallback
        try:
            from PIL import Image
            import statistics
            
            img = Image.open(image_path)
            
            # Convert to grayscale
            gray = img.convert('L')
            
            # Get pixel values
            pixels = list(gray.getdata())
            
            # Calculate average brightness
            avg_brightness = statistics.mean(pixels)
            
            # Simple check
            if avg_brightness < 50 or avg_brightness > 200:
                return "MAINTENANCE"
            
            return "READY"
            
        except:
            # No image library available, use filename heuristic
            filename = os.path.basename(image_path).lower()
            
            # Check filename for hints
            if any(word in filename for word in ["clean", "ok", "good", "ready"]):
                return "READY"
            elif any(word in filename for word in ["broken", "bad", "maintenance", "problem"]):
                return "MAINTENANCE"
            else:
                # Default: assume READY for any valid image file
                return "READY"

def main():
    if len(sys.argv) < 2:
        print("MAINTENANCE")
        sys.exit(0)
    
    image_path = sys.argv[1]
    
    # Validate image path
    if not os.path.exists(image_path):
        print("MAINTENANCE")
        sys.exit(0)
    
    # Analyze image
    result = analyze_image_simple(image_path)
    print(result)
    sys.exit(0)

if __name__ == "__main__":
    main()
