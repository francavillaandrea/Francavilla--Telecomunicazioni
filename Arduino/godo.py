import tkinter as tk
import pyautogui
import time


# Funzione per spammare la stringa e premere invio ogni volta
def ripeti_texto():
    # Ottieni la stringa e il numero dal form
    stringa = entry_stringa.get()
    try:
        numero = int(entry_numero.get())
    except ValueError:
        label_status.config(text="Errore: Inserisci un numero valido.", fg="red")
        return
    
    # Cambia il testo della label per indicare che l'operazione è in corso
    label_status.config(text="Scrivendo, preparati...", fg="orange")
    
    # Pausa per consentire all'utente di posizionare il cursore nella finestra corretta
    time.sleep(2)
    
    # Per ogni ripetizione, scrivi la stringa e premi invio
    for _ in range(numero):
        # Simula la digitazione della stringa
        pyautogui.write(stringa)
        
        # Simula la pressione del tasto 'Invio' (Enter)
        pyautogui.press('enter')
        
    # Cambia il testo della label per indicare che l'operazione è completata
    label_status.config(text="Testo 'spammato' e Invio premuto per ogni ripetizione!", fg="green")


# Creazione della finestra principale
root = tk.Tk()
root.title("Ripetitore di Testo")

# Modifica delle dimensioni della finestra (larghezza 600px, altezza 400px)
root.geometry("600x400")
root.configure(bg="#f2f2f2")

# Titolo
title_label = tk.Label(root, text="Ripetitore di Testo", font=("Helvetica", 16, "bold"), bg="#f2f2f2", fg="#4CAF50")
title_label.pack(pady=20)

# Frame principale per gli input
frame = tk.Frame(root, bg="#f2f2f2")
frame.pack(pady=10)

# Etichetta e campo di input per la stringa
label_stringa = tk.Label(frame, text="Inserisci la stringa:", font=("Helvetica", 12), bg="#f2f2f2", anchor="w")
label_stringa.grid(row=0, column=0, padx=10, pady=5, sticky="w")
entry_stringa = tk.Entry(frame, font=("Helvetica", 12), width=30)
entry_stringa.grid(row=0, column=1, padx=10, pady=5)

# Etichetta e campo di input per il numero
label_numero = tk.Label(frame, text="Numero di ripetizioni:", font=("Helvetica", 12), bg="#f2f2f2", anchor="w")
label_numero.grid(row=1, column=0, padx=10, pady=5, sticky="w")
entry_numero = tk.Entry(frame, font=("Helvetica", 12), width=30)
entry_numero.grid(row=1, column=1, padx=10, pady=5)

# Etichetta per visualizzare lo stato
label_status = tk.Label(root, text="", font=("Helvetica", 12), bg="#f2f2f2", fg="#4CAF50")
label_status.pack(pady=10)

# Bottone per eseguire l'azione
button_ripeti = tk.Button(root, text="Ripeti e Spamma", font=("Helvetica", 14), bg="#4CAF50", fg="white", command=ripeti_texto, relief="flat", width=20)
button_ripeti.pack(pady=20)

# Avvia la GUI
root.mainloop()
