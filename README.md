## Getting started 
### Setting up the Raspberry Pi
1. First you need to install Raspberry Pi Os on your Raspberry pi 3 then update the environment :

```bash
sudo apt-get update
sudo apt-get upgrade
```
2. Python installation :
```bash
python --version
sudo apt install python3
```
3. MariaDB installation and configuration :
```bash
sudo apt-get install mariadb-server
sudo mysql\_secure\_installation
```
 Enter a password then for the following question :
```bash
Switch to unix_socket authentification? => n
Change the root password? => n
Remove anonymous users? => n
Disallow root login remotely? => n
Remove test database and access to it? => n
Reload privilege tables now? => n
```
4. Apache2, php installation :
```bash
sudo apt install apache2
sudo apt install php
sudo apt install phpmyadmin
```
 And choose apache2 for phpmyadmin configuration then answer no to "Configure database for phpmyadmin with dbconfig-common"

5. Again MariaDB/mysql configuration :

   Dont forget to replace username and password don't remove the ' characters
```bash
sudo mysql -u root -p
GRANT ALL PRIVILEGES ON *.* TO ’username’@’localhost’ IDENTIFIED BY ’password’ WITH GRANT OPTION;
quit
```
6. Apache2 configuration :
```bash
sudo nano /etc/apache2/apache2.conf
```
Then add these line :
```bash
Include /etc/phpmyadmin/apache.conf
```
 Save and close the editor then restart apache2 :
```bash
sudo service apache2 restart
```
After 
```bash
sudo ln -s /etc/phpmyadmin/ /var/www/html/phpmyadmin
```

 Now we can reach phpmyadmin on the following link : http ://localhost/phpmyadmin
 If it doesn't work you need to reconfigure phpmyadmin

6. MQTT installation and configuration
 ```bash
sudo apt install mosquitto
sudo nano /etc/mosquitto/mosquitto.conf
```
 Replace by :
 ```bash
allow_anonymous true
pid_file /run/mosquitto/mosquitto.pid
persistence true
persistence_location /var/lib/mosquitto/
listener 1883
listener 9001
protocol websockets
log_dest file /var/log/mosquitto/mosquitto.log
include_dir /etc/mosquitto/conf.d
```
 Save and exit then run these command :
 ```bash
pip install paho-mqtt
pip install mysql-connector
```


Now it is ready to run the project. Don't forget to modify the following files :
| File Path                                      | Description                                               |
|------------------------------------------------|-----------------------------------------------------------|
| Arduino/Projet_IOC.ino                         | Change the PINs, IP address, and listening TOPICs.      |
| Arduino/song.h                                 | Change the PINs if needed.                               |
| Raspberry/server/start_subscriber.py           | Update the listening channel and db_config.              |
| Raspberry/server/www/get_history.php           | Change the credentials.                                   |
| Raspberry/server/www/get_info.php              | Change the credentials.                                   |
| Raspberry/server/www/mqtt.js                   | Update with the correct IP address.                      |


You will need the Arduino IDE with the following package to upload the code to your ESP32 :
 ```bash
https://dl.espressif.com/dl/package_esp32_index.json
```

And all the librairy used with Arduino IDE are located at : ./Arduino/library 
To install them just go to Sketch -> Include Librairy -> ADD ZIP Librairy on your arduino app

### Setting up the database :

You need to created the database by running theses following line on : http ://localhost/phpmyadmin/index.php
```bash
CREATE DATABASE DB_ESP32;
```
```bash
CREATE TABLE ‘ESP32‘ (
‘id‘ int(11) NOT NULL AUTO_INCREMENT,
‘name‘ varchar(24) NOT NULL,
‘mac_address‘ varchar(17) NOT NULL,
‘data‘ text DEFAULT NULL,
‘last_connection‘ text DEFAULT NULL,
PRIMARY KEY (‘id‘),
UNIQUE KEY ‘mac_address‘ (‘mac_address‘)
) ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
```
```bash
CREATE TABLE ‘ESP32_SensorData‘ (
‘id‘ int(11) NOT NULL AUTO_INCREMENT,
‘data‘ text NOT NULL,
‘timestamp‘ timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE
current_timestamp(),
‘fk_mac‘ int(11) NOT NULL,
PRIMARY KEY (‘id‘),
KEY ‘fk_mac_constraint‘ (‘fk_mac‘),
CONSTRAINT ‘fk_mac_constraint‘ FOREIGN KEY (‘fk_mac‘) REFERENCES ‘ESP32‘ (‘id‘)
) ENGINE=InnoDB AUTO_INCREMENT=119223 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
```

### Redirecting apache2 to our www directory :
```bash
sudo nano /etc/apache2/apache2.conf
```
```bash
Find the following lines :

<Directory /var/www/>
Options Indexes FollowSymLinks
AllowOverride None
Require all granted
Allow from all
</Directory>

Replace by

<Directory /home/votre_nom_d’utilisateur/Project_location/Projet_IOC/server/www/>
Options Indexes FollowSymLinks
AllowOverride None
Require all granted
Allow from all
</Directory>
```
Then :
```bash
sudo nano /etc/apache2/sites-available/000-default.conf
```
```bash
Find the following lines :

DocumentRoot /var/www/html

Replace by

DocumentRoot /home/votre_nom_d/Desktop/Projet_IOC/server/www/
```
Then restart apache2
```bash
sudo service apache2 restart
```

Now if everything was done correctly you may run the website and communicate with your ESP32.
![plot](./images/site_web.png?raw=true "site_web")

Short video in french with a demonstration of the project : https://www.youtube.com/watch?v=u35QerKMySA

<!--

Projet réalisé durant le second semestre du master 1 SESI.
Il s'agit de realiser une communication sans fil entre plusieurs ESP32 ainsi qu'une raspberry pi3 et d'y afficher les informations sur un site web.
Il faut aussi réussir a faire communiquer le site web avec les ESP32.

Pour faire fonctionner ce projet, il faut lire le compte rendu et suivre les étapes de la mise en place.

Il y aura des informations à réadapter selon votre configuration, qui sont aussi detaillé dans le compte rendu.

Dans le repertoire arduino, on va y retrouver le code pour l'ESP32 ainsi que les bibliotèques utilisé.

Dans le repertoire Rapsberry, on y retrouvera le code du site web, le serveur d'ecoute MQTT, ainsi que les scripts pour convertir des musiques .midi en note de musique.


Une fois la mise en place faite, il suffit de placer les fichier qui sont dans le repertoire "Raspberry" sur le bureau de la rapsberry par exemple.
Puis il faudra lancer le serveur d'ecoute en tapant : python start_subscriber.py

Une fois le serveur d'écoute lancer, il faudra y récupéré l'adresse ip afficher, changer dans le repertoire www le fichier mqtt.js en mettant l'adresse ip obtenu.

Ensuite il suffira d'ouvrir le logiciel Arduino IDE, d'y ouvrir le projet qui est situé dans le repertoire "Arduino", changez les informations comme décrit dans l'entete et téléversé le code sur l'ESP32.

Vidéo démonstration du projet : https://www.youtube.com/watch?v=u35QerKMySA

![plot](./images/site_web.png?raw=true "site_web")
-->

