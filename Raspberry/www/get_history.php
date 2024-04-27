<?php


// Se connecter à la base de données 
$servername = "localhost"; // Adresse du serveur MySQL
$username = "root"; // Nom d'utilisateur MySQL
$password = "Trou2balle"; // Mot de passe MySQL
$dbname = "DB_ESP32"; // Nom de la base de données

// Créer une connexion
$conn = new mysqli($servername, $username, $password, $dbname);

$selectedMac = $_GET['mac'];

$decode = urldecode($selectedMac);

// Construire la requête SQL avec la MAC sélectionnée
$sql = "SELECT timestamp, ESP32_SensorData.data FROM ESP32_SensorData join ESP32 on ESP32.id = ESP32_SensorData.fk_mac where ESP32.mac_address = '$decode'"; //pour selectionner la donnée qu'on veut affiche

//echo $decode;

// Exécuter la requête SQL
$result = $conn->query($sql);

// Créer un tableau pour stocker les données de luminosité
$data = array();

// Parcourir les résultats de la requête et ajouter les données au tableau
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $data[] = array(
            'timestamp' => $row['timestamp'],
            'data' => json_decode($row['data'], true)
        );
    }
}

// Fermer la connexion à la base de données
$conn->close();

// Renvoyer les données de luminosité au format JSON
echo json_encode($data);
?>
