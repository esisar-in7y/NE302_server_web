# NE323_server_web


## 1. Comment utiliser le serveur web

Pour l'exécuter, on a besoin du header zlib.h. 

Après avoir unzip le projet, il faut installer le `post.php` dans le répertoire `/var/www/html/`.

La commande pour run le serveur est `make run`, mais on peut également exécuter le script autoreload.sh pour cela.

## 2. Sites web utilisés pour le test

Nous avons pour cela les sites :

- localhost:8000/www.toto.com/
- localhost:8000/www.fake.com/
- localhost:8000/www.o.com/, avec une vidéo en streaming avec range
- localhost:8000/game/, un site avec un petit jeu installé dessus
- localhost:8000/post.php, un site en php utilisé pour le test de la prise en charge des fichiers php (POST/GET).

## 3. Fonctionnalités implémentées

- [x] Gestion des requêtes GET
- [x] Gestion des requêtes HEAD
- [x] Gestion des requêtes POST (les sockets sont fermées au fur et à mesure, donc il n'y a pas de problèmes de crash avec un trop grand nombre de requêtes)
- [x] Prise en charge des fichiers php
- [x] Prise en charge des fichiers de différents formats
- [x] Prise en charge du streaming vidéo et de fichiers js

## 4. Leaks

Il n'y a pas de leaks connus à ce jour avec Valgrind (testable avec la commande `make leaks`).

## 5. Unbug

La QUERY_STRING peut maitenant avoir plus de 127 caracteres
