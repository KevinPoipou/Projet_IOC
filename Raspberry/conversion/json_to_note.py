import json

# Chemin vers le fichier JSON d'entrée
input_file_path = "./tmp.txt"
# Chemin vers le fichier de sortie TXT
output_file_path  = "./fichier.txt"

# Définition d'une fonction pour convertir les noms de notes
def convert_note_name(note_name):
    if "#" in note_name:
        return note_name.replace("#", "S")
    else:
        return note_name

# Charger les données JSON à partir du fichier d'entrée
with open(input_file_path, "r") as input_file:
    data = input_file.read()

# Charger le JSON
json_data = json.loads(data)

# Extraire les valeurs "name", "durationTicks" et "time", et les formater selon les spécifications
notes = json_data["notes"]
extracted_data = []
previous_time = 0
cpt = 0

for index, note in enumerate(notes):
    name = convert_note_name(note["name"])
    duration_ticks = int(note["durationTicks"])
    current_time = note["time"]

    if index > 0:  # Vérifiez si ce n'est pas la première note
        time_difference = (current_time - previous_time) * 200
        time_difference = round(time_difference)
        extracted_data[index-1].append(time_difference)  # Ajoutez time_difference à la note précédente

    extracted_data.append(["NOTE_" + name, round(duration_ticks * 1)])
    previous_time = current_time

# Ajoutez time_difference pour la dernière note
if len(notes) > 0:
    extracted_data[-1].append(0)


# Écrire les données extraites dans le fichier de sortie
with open(output_file_path, "w") as output_file:
    for item in extracted_data:
        output_file.write(",".join(str(i) for i in item) + ",\n")

print("Extraction terminée. Les données ont été écrites dans le fichier :", output_file_path)