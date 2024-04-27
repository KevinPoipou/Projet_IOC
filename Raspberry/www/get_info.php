<?php
	// Informations de connexion à la base de données
	$servername = "localhost"; // Adresse du serveur MySQL
	$username = "root"; // Nom d'utilisateur MySQL
	$password = "Aze123Eza*"; // Mot de passe MySQL
	$dbname = "DB_ESP32"; // Nom de la base de données

	// Création de la connexion à la base de données
	$conn = new mysqli($servername, $username, $password, $dbname);

	// Vérification de la connexion
	if ($conn->connect_error) {
		die("Échec de la connexion à la base de données : " . $conn->connect_error);
	}

	// Requête pour récupérer les adresses MAC et les noms associés depuis la table ESP32
	$sql = "SELECT mac_address, name, last_connection, data FROM ESP32";
	$result = $conn->query($sql);

	if ($result->num_rows > 0) {
		// Tableau pour stocker les données
		$data = array();

		// Parcourir les résultats de la requête
    while ($row = $result->fetch_assoc()) {
        // Décodage de la chaîne JSON
        $dataJson = json_decode($row['data'], true);

        // Extraction de la valeur de la luminosité
        $luminosity = $dataJson['luminosity'];
		$motion = $dataJson['motion'];
		$button = $dataJson['button'];


        // Ajouter les données au tableau
        $entry = array(
            'mac_address' => $row['mac_address'],
            'name' => $row['name'],
            'last_connection' => $row['last_connection'],
            'luminosity' => $luminosity,
			'button' => $button,
			'motion' => $motion
        );
        $data[] = $entry;
        
    }

		// Renvoyer les données au format JSON
		echo json_encode($data);
	} else {
		echo "Aucune donnée trouvée.";
	}

	// Fermer la connexion à la base de données
	$conn->close();
?>
