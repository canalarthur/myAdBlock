# myAdBlock
Projet de RSA 2016-2017, par Arthur Canal et Jérôme Gauzins.

## Pour compiler : 
Utilisez `make` ou `gcc src/serverTCP.c -pthread -I include/ -o proxy`
 
## Pour executer :
Utilisez `./proxy portNumber` (par exemple `./proxy 8080`)

### Modifications :
Il est possible de modifier le nombre maximal de threads utilisés en changeant la valeur de la constante `MAX_NUMBER_OF_THREADS` définie par : `#define MAX_NUMBER_OF_THREADS XX`
