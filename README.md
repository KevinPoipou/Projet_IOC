## Getting started 
### Raspberry
First you need to install Raspberry Pi Os on your Raspberry pi 3










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


