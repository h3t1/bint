## Implémentation des opérations arithmétiques sur les grands nombres

Les grands nombres sont largement utilisés dans de nombreuses applications:
- Cryptographie : RSA (par exemple)
- Signature des documents
- Fonctions de hachage
- etc

Bien que le type de données «*long long int*» puisse stocker de grands nombres entiers, il ne peut
pas stocker des valeurs extrêmement grandes telles qu’un entier à 200 chiffres par exemple.
Nous souhaitons donc stocker ces grands nombres (techniquement, les nombres plus de 20 chiffres)
dans une structure de données permettant d’effectuer les opérations arithmétiques de base, il
s'agit notamment de munir cette structure par les opérations arithmétiques telles que +, -, ∗, /, %.
```
( n chiffres ) OP ( m chiffres )
```
Exemple: <img src="img/i1.png" alt="738....393+1235....238" height="32px" />

Pour pouvoir manipuler les grands nombres et ses opérations arithmétiques, il faut franchir plusieurs étapes:
1. Lire les deux nombres entrés par l’utilisateur comme une chaîne de caractères.
2. Vérifier lexicalement la chaîne de caractères:
```
BINT "-" ?[0 -9]+
```
3. Déterminer la taille de paquet convenable à l’opération arithmétique choisie :
Un paquet de type *unsigned long long int* peut stocker jusqu’à 20 chiffres
(ULLONG_MAX = 2<sup>64</sup> − 1 = 18446744073709551615)
