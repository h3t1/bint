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
Exemple: ![738....393+1235....238](img/i1.png)
