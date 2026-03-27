import sys
import time
import argparse
from playwright.sync_api import sync_playwright

def log(msg):
    print(f"[{time.strftime('%H:%M:%S')}] {msg}")
    sys.stdout.flush()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--prompt", required=True, help="Le texte de prompt a generer")
    parser.add_argument("--groq_key", required=False, help="Cle API GROQ pour analyser le texte de la page gratuitement")
    args = parser.parse_args()

    log("Démarrage du navigateur automatisé...")
    
    with sync_playwright() as p:
        user_data_dir = "./invideo_chrome_profile"
        log(f"Lancement du navigateur MS Edge avec profil persistant : {user_data_dir}")
        
        # Configuration Anti-Bot et persistance de session
        context = p.chromium.launch_persistent_context(
            user_data_dir,
            headless=False,
            channel="msedge", # Utilise l'Edge installé sur Windows
            viewport={'width': 1280, 'height': 800},
            user_agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 Edg/120.0.0.0",
            args=[
                "--disable-blink-features=AutomationControlled", # Cache que c'est un bot
                "--start-maximized"
            ]
        )
        
        # Sur un contexte persistant, une page par défaut est parfois déjà ouverte
        page = context.pages[0] if context.pages else context.new_page()

        try:
            log("Navigation vers InVideo AI...")
            page.goto("https://ai.invideo.io/", timeout=60000)
            
            log("Attente de l'interface principale (connectez-vous si nécessaire)...")
            # On donne 5 minutes (300 000 ms) pour que la zone de texte apparaisse
            # Cela vous laisse amplement le temps de vous connecter manuellement
            prompt_selector = "textarea" 
            page.wait_for_selector(prompt_selector, timeout=300000)
            
            log("Saisie du prompt IA...")
            prompt_selector = "textarea" 
            page.wait_for_selector(prompt_selector, timeout=300000)
            page.fill(prompt_selector, args.prompt)
            
            time.sleep(2)
            log("Validation (Appui sur la touche Entrée)...")
            # InVideo réagit souvent à la touche Entrée plutôt qu'à un clic fragile
            page.keyboard.press("Enter")

            # Gérer le popup de configuration d'InVideo (Audience, Look and Feel, etc.)
            time.sleep(5)
            try:
                if args.groq_key:
                    log("Utilisation de l'IA (Groq / Llama-3) pour trouver le bouton de validation du popup...")
                    try:
                        from openai import OpenAI
                        # Groq est compatible avec le SDK OpenAI Python, hyper rapide et a un Free Tier
                        client = OpenAI(api_key=args.groq_key, base_url="https://api.groq.com/openai/v1")
                        
                        # Extract the visible text from the page instead of taking a screenshot
                        page_text = page.locator("body").inner_text()
                        
                        log("Analyse du texte de la page en cours par Llama-3.3...")
                        response = client.chat.completions.create(
                            model="llama-3.3-70b-versatile",
                            messages=[
                                {
                                    "role": "user",
                                    "content": f"I am automating InVideo AI to generate a video. I am looking at a popup. I need to click the final confirm button to start generating the video (usually 'Continue' or 'Generate video'). Here is the text visible on the screen: \n\n{page_text}\n\nWhat is the EXACT TEXT of the button I should click based on this text? Output ONLY the text of the button, nothing else (no quotes). If there is no such button, output 'NONE'."
                                }
                            ]
                        )
                        btn_text = response.choices[0].message.content.strip()
                        log(f"L'IA Vision recommande de cliquer sur : '{btn_text}'")
                        
                        if btn_text != "NONE" and btn_text != "":
                            page.click(f"button:has-text('{btn_text}'), :has-text('{btn_text}')", timeout=5000)
                        else:
                            log("L'IA n'a pas vu de bouton clair. Fallback: appui sur Entrée.")
                            page.keyboard.press("Enter")
                    except Exception as ev:
                        log("Erreur de l'API Vision, fallback standard : " + str(ev))
                        confirm_btn = "button:has-text('Continue'), button:has-text('Generate video')"
                        page.click(confirm_btn)
                else:
                    log("Recherche du bouton de confirmation final par défaut (Continue/Generate)...")
                    # Chercher un bouton générique de validation
                    confirm_btn = "button:has-text('Continue'), button:has-text('Generate video')"
                    page.wait_for_selector(confirm_btn, timeout=15000)
                    page.click(confirm_btn)
            except Exception as e:
                log("Aucun popup de confirmation détecté ou erreur : " + str(e))

            log("En attente de connexion ou génération (Cloudflare / Auth possible)...")
            log("ACTION REQUISE : Si InVideo demande de vous connecter, veuillez le faire dans la fenêtre ouverte.")
            
            # A partir de là, invideo prend beaucoup de temps et demande sûrement un login si pas connecté
            # On attend un très long sélecteur "Export" ou "Download" qui indique que la vidéo est finie
            download_btn = "button:has-text('Export')"
            log("Le script attend la fin de la génération (Temps illimité)...")
            
            # Attente INIFINIE pour que la vidéo se termine (timeout=0)
            page.wait_for_selector(download_btn, timeout=0)
            log("Vidéo terminée ! Le bouton d'Export est visible.")
            time.sleep(2)
            
            log("SUCCESS")
            
        except Exception as e:
            log(f"Erreur d'automatisation : {str(e)}")
            log("FAILED")
        
        finally:
            log("Fermeture du navigateur (contexte sauvegardé) dans 10 secondes...")
            time.sleep(10)
            context.close()

if __name__ == "__main__":
    main()
