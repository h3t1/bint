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
(ULLONG_MAX = 2<sup>64</sup> − 1 = 18446744073709551615)</br></br>
Nous voulons garantir que:</br>
∀*p1, p2* deux paquets de taille *n* chiffres, ∀α ∈ {+, −, ∗, /, %}</br>
*p1αp2 = p3* avec *p3* est un paquet de taille toujours inférieure ou égale à *n*.</br>
   1. La formule pour calculer la taille maximale des paquets pour les opérations +, -, /, % :</br>
![equ](https://render.githubusercontent.com/render/math?math=\text{max\_p}%20=%20\text{nombre\_chiffres(ULLONG\_MAX)}-2)
En langage C: 
   ```c
   max_p = snprintf(NULL , 0 , "%llu", ULLONG_MAX) - 2;
   ```
   2. La formule pour calculer la taille de paquet pour l’opération de multiplication ∗ est:</br>
![equ](https://render.githubusercontent.com/render/math?math=\text{max\_p}%20=%20\frac{\text{nombre\_chiffres(ULLONG\_MAX)}}{2}-1)</br>
En langage C:
   ```c
   max_p = snprintf(NULL , 0 , "%llu", ULLONG_MAX)/2 - 1;
   ```
4. Déterminer le nombre de paquets à insérer pour une chaîne donnée:
   - Le nombre de paquets:

