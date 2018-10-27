# Style de code

### Principes de base:
  - Uniformité: Vaut mieux guarder le même style que de changer à l'intérieur d'un même fichier.
  - Lisibilité: Favoriser une meilleur lisibilité à une syntaxe plus compacte.
  - Aération: On laisse plus d'espace que pas assez entre les déclarations.

### Indentation:
On utilise une indetation de **4 espaces de large**. On  favorise l'utilisation d'espaces au lieu de caractères de `TAB`. On essai de troujours d'identer les élements imbriqués. Tous les éléments ayant le même niveau d'imbriquation doivent compter la même identation.

### Espaces
On ajoute toujours un espace en entre un opérateur et les opérands si celui-ci a plus d'un opérand.
```
x = y + z;
x = 1 * (4 + 5);
++x;
```

### Nomenclature
On utilise des mots ou certains acronymes pour composer le nom des variables. Toute variable ou fonctions devrait être auto-descriptive et se distinguer des autres noms présent dans la base de code.
##### Constantes
Les contantes s'écrivent toutes en majuscule avec des soulignés pour séparer les mots.
```
NOMBRE_DE_PIEDS
JAUNE
PORT_TX
```
##### Variables
Les noms de variable sont écrit en *camel* case.
```
nombreDePieds
jaune
portTx
```
##### Fonctions
Les noms de fonction sont étrit en *camel* case avec la première lettre en majuscule. On essai d'utiliser un verbe d'action pour décrire l'effet de la fonction.
```
ComptePieds
SelctionCouleur
EnvoiTx
```

