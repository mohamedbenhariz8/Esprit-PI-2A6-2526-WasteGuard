import sys
import time
import argparse

def log(msg):
    timestamp = time.strftime('%H:%M:%S')
    try:
        print(f"[{timestamp}] {msg}")
    except Exception:
        # Fallback for Windows console not supporting emojis like 🔥
        safe_msg = msg.encode('ascii', 'replace').decode('ascii')
        print(f"[{timestamp}] {safe_msg}")
    sys.stdout.flush()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--prompt", required=True, help="Video prompt")
    parser.add_argument("--google_key", required=True, help="Google Gemini/Vertex AI Key")
    args = parser.parse_args()

    log("Démarrage de l'intégration Google Veo avec le SDK Officiel (google-genai)...")
    log("Clé API Google reconnue.")

    try:
        from google import genai
        from google.genai import types
        
        client = genai.Client(api_key=args.google_key)
        
        log("Recherche des modèles vidéo (Veo) autorisés pour cette clé API...")
        available_models = list(client.models.list())
        veo_models = [m.name for m in available_models if "veo" in m.name.lower()]
        
        if not veo_models:
            log("❌ ERREUR CRITIQUE : Votre clé API n'a accès à AUCUN modèle Veo.")
            log("Modèles disponibles (échantillon) : " + ", ".join([m.name for m in available_models[:5]]))
            log("Pour utiliser Veo, vous devez être explicitement sur liste d'attente / autorisé par Google (Private Preview).")
            log("Veuillez utiliser un modèle autorisé ou demander l'accès à Google.")
            log("FAILED")
            return
            
        log(f"✅ Modèles Veo trouvés pour votre compte : {veo_models}")
        
        for target_model in veo_models:
            log(f"\nTentative de génération avec le modèle : {target_model}...")
            try:
                # Lancement de l'opération asynchrone (Long-Running Operation) sur Veo
                operation = client.models.generate_videos(
                    model=target_model,
                    prompt=args.prompt,
                )
                log(f"✅ Vidéo générée avec succès par {target_model} !")
                log("SUCCESS")
                return # Stop after first success
            except Exception as e:
                err_msg = str(e)
                log(f"❌ Échec avec {target_model} : {err_msg}")
                if "billing" in err_msg.lower():
                    log("   -> Ce modèle nécessite l'activation de la facturation Google Cloud (carte bleue).")
        
        log("\n❌ Tous les modèles Veo disponibles ont échoué. Veuillez vérifier vos accès et la facturation.")
        log("FAILED")
            
    except Exception as e:
        log("Erreur de connexion globale API Google Veo : " + str(e))
        log("FAILED")

if __name__ == "__main__":
    main()
