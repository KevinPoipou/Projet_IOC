import mysql.connector
import paho.mqtt.client as mqtt
import json
import socket

canal = "Pi"

db_config = {
    'user' : 'root',
    'password' : 'Aze123Eza*',
    'host' : 'localhost',
    'database' : 'DB_ESP32'
}

def get_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80)) #connecte au serveur de google pour recup l'addresse ip local
        ip_address = s.getsockname()[0]
        s.close
        return ip_address
    except Exception as e:
        print("Erreur lors de la recuperation de l'adresse IP :",e)
        return None
        
ip_address = get_ip()
if ip_address:
    print("Adresse IP :",ip_address)
else:
    print("Impossible de recup l'adresse IP")

def on_connect(client, userdata, flags, rc): # Callback connexion broker
    print("Broker connected on chanel : " +canal)
    client.subscribe(canal)

def on_message(client, userdata, msg): # Callback subscriber
    try:
        message = json.loads(msg.payload.decode("utf-8"))
        print("Message reçu sur le canal : " + msg.topic + " - Contenu : " + str(message)) 
        
        name = message["name"]
        mac_address = message["mac_address"]
        data = message["data"]
        
        json_data = json.dumps(data)
        
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()

        # Requête SQL 
        select_query = f"SELECT ID FROM `ESP32` WHERE `mac_address` = '{mac_address}'"
        cursor.execute(select_query)
        result = cursor.fetchone()

        if result: #si le mac_address existe deja on update la table sinon on ajoute a la table
            update_query = f"UPDATE `ESP32` SET `last_connection` = CURRENT_TIMESTAMP, `name` = '{name}', `data` = '{json_data}' WHERE `mac_address` = '{mac_address}'"
            cursor.execute(update_query)
            conn.commit()
            # print("La DB a été mise à jour")
        else:
            insert_query = f"INSERT INTO `ESP32` (`name`, `mac_address`, `data`,`last_connection`) VALUES ('{name}', '{mac_address}', '{json_data}', CURRENT_TIMESTAMP)"
            cursor.execute(insert_query)
            select_query = f"SELECT ID FROM `ESP32` WHERE `mac_address` = '{mac_address}'"
            cursor.execute(select_query)
            result = cursor.fetchone()
            conn.commit()
            # print("Une nouvelle ligne a été insérée")
        # print("Result : ", result[0])    
        #j'insere les data dans une autre table pour evité de spam la table principale
        insert_query = f"INSERT INTO `ESP32_SensorData` (`fk_mac`, `data`,`timestamp`) VALUES ('{result[0]}', '{json_data}', CURRENT_TIMESTAMP)"
        cursor.execute(insert_query)
        conn.commit()
        
        delete_query = f"DELETE FROM ESP32_SensorData WHERE timestamp < (NOW() - INTERVAL 10 MINUTE)"
        cursor.execute(delete_query)
        conn.commit()

        cursor.close()
        conn.close()
        # print("Message publié sur MySQL")

    except Exception as e:
        print("Une erreur s'est produite :", str(e))




client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1) # Création d'une instance client
client.on_connect = on_connect # Assignation des callbacks
client.on_message = on_message
client.connect(ip_address, 1883, 300) # Connexion au broker


client.loop_forever() # pour attendre les messages

