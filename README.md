# Projet 3D : Terrain Authoring

## Présentation du projet
De nos jours, les terrains sont beaucoup utilisés dans de nombreux domaines, comme la simulation ou les jeux vidéo, il est donc nécessaire de trouver une solution pour les stocker, car les modèles 3D deviennent très lourd selon la taille des terrains. Une solution, qui est la nôtre, est d'avoir un terrain à partir de différentes cartes. Celles-ci décriront différents aspects du terrain et pourront, lors de la génération, être utilisées pour générer le terrain final en n'ayant stocké seulement quatre images. 


 De plus, avec l'utilisation de cartes, il est possible de non seulement générer, mais aussi de modifier le terrain comme bon nous semble.
C'est dans ce cadre que s'inscrit notre projet. Il aura la forme d'une application développée en Qt C++, utilisant le système de cartes énoncé et permettant la visualisation du terrain ainsi que la modification de celui-ci en temps réel et en 3D. Les changements peuvent être appliqués aux cartes afin de pouvoir les sauvegarder  et les utiliser dans divers contextes et applications, prenant en charge la génération de terrain par cartes.

## Installation sur Linux
Ouvrir un terminal

Cloner le projet dans le répertoire souhaité
```bash
  git clone https://github.com/fcaball/Terrain.git
```
Déplacez-vous dans le répertoire de l'application
```bash
  cd App
```
Installez assimp si ce n'est pas fait
```bash
  sudo apt-get update
  sudo apt-get -y install assimp-utils
```

ou 

(/!\ Cette méthode requiert cmake)
```bash
  git clone https://github.com/assimp/assimp.git
  cd assimp
  cmake CMakeLists.txt
  cmake --build .
  make
```
Si vous utilisez cette méthode il faut ajouter au fichier App.pro 
les lignes suivantes :
```bash
LIBS+= -L/chemin/absolu/vers/assimp/bin -lassimp

INCLUDEPATH += assimp/include
  ```
Ensuite une fois revenu dans le dossier App (cd ..), si vous n'y êtes pas déja, il vous reste plus qu'à faire :
```bash
  make clean
  qmake
  make
  ./App
```

  
