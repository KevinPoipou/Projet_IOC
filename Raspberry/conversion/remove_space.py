input_file = "fichier.txt"  # Chemin vers le fichier d'entrée
output_file = "fichier_json.txt"  # Chemin vers le fichier de sortie

with open(input_file, "r") as file:
    content = file.read()  # Lire le contenu du fichier en une seule chaîne

# Supprimer les sauts de ligne du contenu
content_without_newlines = content.replace("\n", "")

with open(output_file, "w") as file:
    file.write(content_without_newlines)  # Écrire le contenu sans sauts de ligne dans le fichier de sortie