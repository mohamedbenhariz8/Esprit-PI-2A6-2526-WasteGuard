import sys
import time
import argparse
import os

def log(msg):
    print(f"[{time.strftime('%H:%M:%S')}] {msg}")
    sys.stdout.flush()

def ask_groq(client, page_html, question):
    """Use Groq to analyze the page HTML/text and answer a specific question."""
    try:
        response = client.chat.completions.create(
            model="llama-3.3-70b-versatile",
            messages=[
                {
                    "role": "system",
                    "content": "You are a web automation assistant. Your job is to analyze page source code or text and answer precise questions about UI elements. Always respond concisely with only the requested information, no extra text."
                },
                {
                    "role": "user",
                    "content": f"{question}\n\nPage content:\n{page_html[:8000]}"
                }
            ],
            max_tokens=200
        )
        return response.choices[0].message.content.strip()
    except Exception as e:
        log(f"Groq error: {e}")
        return None

def smart_click(page, groq_client, page_html, action_description, fallback_selectors=None):
    """Ask Groq to find the right element to click for a given action."""
    log(f"Groq AI analyse la page pour : {action_description}")
    
    answer = ask_groq(
        groq_client, page_html,
        f"I need to {action_description}. Look at this page HTML/text. Give me a CSS selector OR the exact visible text of the element I need to click. Output ONLY one of these two things: either a CSS selector (e.g.: button.create-btn) OR the exact button text (e.g.: Generate). Nothing else."
    )
    
    if answer:
        log(f"Groq recommande : '{answer}'")
        try:
            # Try using it as a CSS selector first
            if answer.startswith('.') or answer.startswith('#') or answer.startswith('button') or answer.startswith('div') or answer.startswith('['):
                page.click(answer, timeout=5000)
            else:
                # Try as text content
                page.click(f":has-text('{answer}')", timeout=5000)
            log(f"Clic réussi pour : {action_description}")
            return True
        except Exception as e:
            log(f"Tentative Groq échouée : {e}, passage au fallback...")
    
    # Fallback to pre-defined selectors
    if fallback_selectors:
        for sel in fallback_selectors:
            try:
                page.click(sel, timeout=3000)
                log(f"Fallback réussi avec : {sel}")
                return True
            except:
                pass
    
    log(f"ATTENTION: Impossible de cliquer pour '{action_description}'")
    return False

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--prompt", required=True, help="Le texte du prompt vidéo")
    parser.add_argument("--image_path", required=False, help="Chemin absolu vers l'image du produit à uploader")
    parser.add_argument("--groq_key", required=False, help="Clé API Groq (gratuit) pour l'analyse intelligente des pages")
    parser.add_argument("--aspect_ratio", required=False, default="16:9", help="Format vidéo: 16:9, 9:16, 1:1")
    parser.add_argument("--duration", required=False, default="5s", help="Durée: 5s ou 10s")
    args = parser.parse_args()

    from playwright.sync_api import sync_playwright
    
    groq_client = None
    if args.groq_key:
        try:
            from openai import OpenAI
            groq_client = OpenAI(api_key=args.groq_key, base_url="https://api.groq.com/openai/v1")
            log("Client Groq AI initialisé avec succès.")
        except Exception as e:
            log(f"Erreur d'initialisation Groq: {e}. Le bot continuera sans IA.")

    log("Démarrage du navigateur (MS Edge avec profil persistant)...")
    
    with sync_playwright() as p:
        user_data_dir = "./lumalabs_edge_profile"
        
        context = p.chromium.launch_persistent_context(
            user_data_dir,
            headless=False,
            channel="msedge",
            viewport={'width': 1440, 'height': 900},
            user_agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36 Edg/122.0.0.0",
            args=[
                "--disable-blink-features=AutomationControlled",
                "--start-maximized"
            ]
        )
        
        page = context.pages[0] if context.pages else context.new_page()
        
        try:
            log("Navigation vers Luma Dream Machine...")
            page.goto("https://dream-machine.lumalabs.ai/", timeout=60000)
            
            log("Attente de l'interface principale (connectez-vous si demandé)...")
            log("ASTUCE: Si vous voyez un bouton de connexion Google, cliquez dessus. Le bot reprendra ensuite.")
            
            # Wait for the main create interface (prompt textarea)
            # Dream Machine has a textarea or contenteditable div for the prompt
            prompt_selectors = [
                "textarea",
                "[placeholder*='prompt']",
                "[placeholder*='dream']",
                "[data-testid='prompt-input']",
                ".prompt-input",
                "[contenteditable='true']"
            ]
            
            prompt_element = None
            for sel in prompt_selectors:
                try:
                    page.wait_for_selector(sel, timeout=300000)  # 5 min to login
                    prompt_element = sel
                    log(f"Interface trouvée ! Sélecteur: {sel}")
                    break
                except:
                    continue
            
            if not prompt_element:
                raise Exception("Impossible de trouver l'interface de création même après 5 minutes.")
            
            time.sleep(2)
            
            # ========================
            # STEP 1: Upload Image
            # ========================
            if args.image_path and os.path.exists(args.image_path):
                log(f"Upload de l'image: {args.image_path}")
                
                # Look for the file input element
                file_input_found = False
                try:
                    # Try clicking an upload button first to trigger file input
                    upload_btn_selectors = [
                        "input[type='file']",
                        "button[aria-label*='upload']",
                        "button[aria-label*='image']",
                        ".upload-btn",
                        "[class*='upload']",
                        "button:has-text('Image')",
                        "label[for*='file']",
                    ]
                    
                    # First, ask Groq for the image upload button if available
                    if groq_client:
                        page_text = page.locator("body").inner_text()
                        upload_answer = ask_groq(
                            groq_client, page_text,
                            "I need to upload an image to this video generation form. What CSS selector or text label should I use to find the image upload button? It might be an icon, camera symbol '+' or 'Add image' button."
                        )
                        if upload_answer:
                            log(f"Groq suggère le sélecteur upload: {upload_answer}")
                    
                    # Use Playwright's file upload via direct input
                    file_input = page.locator("input[type='file']")
                    if file_input.count() > 0:
                        file_input.first.set_input_files(args.image_path)
                        log("Image uploadée avec succès via input[type='file'] !")
                        file_input_found = True
                    else:
                        # Try clicking the upload area button
                        for sel in upload_btn_selectors[1:]:
                            try:
                                btn = page.locator(sel)
                                if btn.count() > 0:
                                    btn.first.click()
                                    time.sleep(1)
                                    # After click, try to set files
                                    fi = page.locator("input[type='file']")
                                    if fi.count() > 0:
                                        fi.first.set_input_files(args.image_path)
                                        log(f"Image uploadée via: {sel}")
                                        file_input_found = True
                                        break
                            except:
                                continue
                    
                    if not file_input_found:
                        log("ATTENTION: L'upload automatique d'image a échoué. Continuons sans image.")
                    
                    time.sleep(3)
                except Exception as e:
                    log(f"Erreur lors de l'upload: {e}. Continuation sans image.")
            elif args.image_path:
                log(f"ATTENTION: Image introuvable à: {args.image_path}")
            
            # ========================
            # STEP 2: Select Aspect Ratio (Format)
            # ========================
            log(f"Sélection du format vidéo: {args.aspect_ratio}")
            aspect_ratio_map = {
                "16:9": ["16:9", "Landscape", "Horizontal"],
                "9:16": ["9:16", "Portrait", "Vertical"],
                "1:1": ["1:1", "Square"]
            }
            
            ratio_texts = aspect_ratio_map.get(args.aspect_ratio, ["16:9"])
            ratio_clicked = False
            
            if groq_client:
                page_text = page.locator("body").inner_text()
                ratio_answer = ask_groq(
                    groq_client, page_text,
                    f"I need to select the video aspect ratio '{args.aspect_ratio}'. What is the exact button text or CSS selector to click? The options are usually 16:9, 9:16, 1:1 or Landscape/Portrait/Square."
                )
                if ratio_answer and ratio_answer != "NONE":
                    for attempt_sel in [ratio_answer, f":has-text('{ratio_answer}')", f"button:has-text('{ratio_answer}')"]:
                        try:
                            page.click(attempt_sel, timeout=3000)
                            log(f"Format sélectionné via Groq: {ratio_answer}")
                            ratio_clicked = True
                            break
                        except:
                            pass
            
            if not ratio_clicked:
                for text in ratio_texts:
                    for sel in [f"button:has-text('{text}')", f":has-text('{text}')"]:
                        try:
                            page.click(sel, timeout=2000)
                            log(f"Format sélectionné: {text}")
                            ratio_clicked = True
                            break
                        except:
                            pass
                    if ratio_clicked:
                        break
            
            if not ratio_clicked:
                log("Format non modifié (utilisation du format par défaut).")
            
            time.sleep(1)
            
            # ========================
            # STEP 3: Select Duration
            # ========================
            log(f"Sélection de la durée: {args.duration}")
            duration_seconds = args.duration.replace("s", "")
            
            duration_clicked = False
            if groq_client:
                page_text = page.locator("body").inner_text()
                dur_answer = ask_groq(
                    groq_client, page_text,
                    f"I need to select video duration of {args.duration}. What exact button text or selector do I click for the duration/length setting? Options are typically 5s or 10s."
                )
                if dur_answer and dur_answer != "NONE":
                    try:
                        page.click(f":has-text('{dur_answer}')", timeout=3000)
                        log(f"Durée sélectionnée via Groq: {dur_answer}")
                        duration_clicked = True
                    except:
                        pass
            
            if not duration_clicked:
                for text in [args.duration, duration_seconds, f"{duration_seconds} sec", f"{duration_seconds} seconds"]:
                    try:
                        page.click(f"button:has-text('{text}')", timeout=2000)
                        log(f"Durée sélectionnée: {text}")
                        duration_clicked = True
                        break
                    except:
                        pass
            
            if not duration_clicked:
                log("Durée non modifiée (utilisation de la valeur par défaut).")
            
            time.sleep(1)
            
            # ========================
            # STEP 4: Write the Prompt
            # ========================
            log("Écriture du prompt vidéo...")
            try:
                page.click(prompt_element, timeout=5000)
                time.sleep(0.5)
                page.keyboard.press("Control+a")
                page.keyboard.press("Delete")
                page.type(prompt_element, args.prompt, delay=30)
                log(f"Prompt saisi: {args.prompt[:80]}...")
            except Exception as e:
                log(f"Erreur de saisie du prompt: {e}")
            
            time.sleep(2)
            
            # ========================
            # STEP 5: Click Generate
            # ========================
            log("Clic sur le bouton Générer...")
            generate_clicked = False
            
            if groq_client:
                page_text = page.locator("body").inner_text()
                gen_answer = ask_groq(
                    groq_client, page_text,
                    "I need to click the final Generate/Create/Submit button to start the video generation. What is the exact text or CSS selector of this button? It could be an arrow button or icon too."
                )
                if gen_answer and gen_answer != "NONE":
                    try:
                        if gen_answer.startswith('.') or gen_answer.startswith('#'):
                            page.click(gen_answer, timeout=5000)
                        else:
                            page.click(f"button:has-text('{gen_answer}')", timeout=5000)
                        log(f"Génération lancée via Groq: '{gen_answer}'")
                        generate_clicked = True
                    except:
                        pass
            
            if not generate_clicked:
                # Try pressing Enter (common on these platforms)
                page.keyboard.press("Enter")
                log("Génération lancée via touche Entrée.")
                generate_clicked = True
            
            # ========================
            # STEP 6: Wait for completion
            # ========================
            log("En attente de la fin de la génération (peut prendre plusieurs minutes)...")
            log("Le navigateur Edge restera ouvert jusqu'à la fin.")
            
            # Look for completion indicators - Luma shows a download button or the video appears
            completion_selectors = [
                "button:has-text('Download')",
                "a:has-text('Download')",
                "button[aria-label*='download']",
                "[class*='download']",
                "video",  # The generated video finally appearing
            ]
            
            done = False
            start_time = time.time()
            check_interval = 15  # Check every 15 seconds
            
            while not done:
                elapsed = int(time.time() - start_time)
                log(f"Génération en cours... ({elapsed}s écoulées)")
                
                for sel in completion_selectors:
                    try:
                        element = page.locator(sel)
                        if element.count() > 0:
                            log(f"Vidéo générée ! Élément de téléchargement détecté: {sel}")
                            done = True
                            break
                    except:
                        pass
                
                if not done:
                    if groq_client and elapsed % 60 == 0 and elapsed > 0:
                        # Every minute, ask Groq if the video is ready
                        page_text = page.locator("body").inner_text()
                        status = ask_groq(
                            groq_client, page_text,
                            "Is the video generation complete? Look for a 'Download' button, a percentage bar at 100%, or the word 'Done'. Reply: YES or NO."
                        )
                        log(f"Groq status check: {status}")
                        if status and "YES" in status.upper():
                            done = True
                    
                    time.sleep(check_interval)
            
            log("SUCCESS: La vidéo est prête !")
            log("Vous pouvez la télécharger depuis la fenêtre du navigateur.")
            time.sleep(15)
            
        except Exception as e:
            log(f"Erreur d'automatisation: {str(e)}")
            log("FAILED")
        
        finally:
            log("Fermeture du navigateur dans 15 secondes...")
            time.sleep(15)
            context.close()

if __name__ == "__main__":
    main()
