## Getting started 
### Raspberry
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


