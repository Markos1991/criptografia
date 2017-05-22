#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define MAX_CLAVES      50
#define MAX_MENSAJE     100

typedef struct Clave_publica {
    unsigned long int n;
    unsigned long int e;
} Clave_publica;

typedef struct Clave_privada {
    unsigned long int n;
    unsigned long int d;
} Clave_privada;

typedef struct Claves {
    Clave_publica publica;
    Clave_privada privada;
} Claves;

Claves genera_claves();
int es_primo(unsigned long int num);
unsigned long int encuentra_d(unsigned long int e, unsigned long int phi);
void salida(char * mensaje);
unsigned long int mcd(unsigned long int a, unsigned long int b);
long int * cifra(long int * mensaje, Clave_publica clave);
long int * descifra(long int * mensaje, Clave_privada clave);
long int exponente_modular(long int base, long int exp, long int mod);

/** Funcion mcd: máximo común divisor
 *  Entradas: a y b, dos números enteros
 *  Salida: mcd(a,b).
 */
unsigned long int mcd(unsigned long int a, unsigned long int b) {
    while(a!=b) {
        if(a > b)
            a-=b;
        else
            b-=a;
    }
    return a;
}

/** Funcion exponente_modular: potencia de un número en un anillo
 *  Entradas: base, exponente y módulo
 *  Salida: (base elevado a exp) módulo mod
 */
long int exponente_modular(long int base, long int exp, long int mod) {
    int k=0;
    int ret=1;
    for(;k<exp;k++) {
        ret*=base;
        ret = ret%mod;
    }

    return ret;
}


/** Función salida:
 *  Entrada: Cadena mensaje a imprimir
 *  Salida: El mensaje impreso y fin de la 
 *  ejecución del programa con código de 
 *  salida 1.
 */
void salida(char * mensaje) {
    fprintf(stderr, "%s\n", mensaje);
    exit(1);
}

int es_primo(unsigned long int num) {
    double max=sqrt(num);
    if(num%2==0)
        return num==2;
    for(int k=3; k<=max; k+=2) {
        if(num%k==0)
            return 0;
    }
    return 1;
}

unsigned long int encuentra_d(unsigned long int e, unsigned long int phi) {
    
    // Encuentra d tal que (d * e) % phi(n) = 1
    unsigned long int d=1;
    while(1) {
        if(((d*e) % phi) == 1)
            return d;
        d++;
    }

}


Claves genera_claves() {
    unsigned long int p, q, n, phi;
    Claves ret[MAX_CLAVES];
    char respuesta;
    int indice=0;

    printf("Necesitamos dos números primos:\n");
    scanf("%ld", &p);
    if(!es_primo(p))
        salida("Entrada incorrecta. P no es primo");
    scanf("%ld", &q);
    if(!es_primo(q))
        salida("Entrada incorrecta. Q no es primo");
    if(p==q)
        salida("Entrada incorrecta. P es igual a Q");

    n = p * q; 
    printf("N = %ld, p = %ld, q = %ld\n", n, p, q);
    phi = (p-1) * (q-1); 
    printf("Phi(n) = %ld\n", phi);

    /* Elegir e tal que 1 < e < phi(n), y tal que
     * e y n sean coprimos. */
    long int i=2,j=0;
    for(; i<phi && j<MAX_CLAVES; i++) {
        if((mcd(i,n)==1) && (mcd(i,phi)==1)){ // e y n son coprimos, y e y phi(n) también
            ret[j].publica.e=i;
            ret[j].publica.n=n;
            ret[j].privada.n=n;
            ret[j].privada.d=encuentra_d(i,phi);
            j++;
        }
    }

    printf("Claves generadas.\n");
    for(i=0; i<j; i++) {
        printf("La clave pública número %ld es (e,n) = (%ld,%ld)\n", i, ret[i].publica.e, ret[i].publica.n);
        printf("La clave privada número %ld es (d,n) = (%ld,%ld)\n", i, ret[i].privada.d, ret[i].privada.n);
    }

    while(respuesta!='s' && respuesta!='n') {
        printf("\nPor defecto escogeremos la primera (0).\n¿Quiere cambiar (s/n)?\n");
        getchar();
        respuesta = getchar();
    }

    if(respuesta=='s') {
        getchar();
        while(indice<1 || indice>i) {
            printf("Por favor escoja un número de clave válido: 1,...,%ld:\n", i-1);
            scanf("%i", &indice);
            printf("Elegido: %i\n", indice);
        }
        Claves tmp = ret[indice];
        ret[indice] = ret[0];
        ret[0] = tmp;
    }
    printf("\n\nClave elegida:\n\n\tkp = ( %ld, %ld )\n\n\tks = ( %ld, %ld )\n\n", ret[0].publica.e, ret[0].publica.n, ret[0].privada.d, ret[0].privada.n );

    return ret[0];
}

long int * cifra(long int * mensaje, Clave_publica clave) {
    int k=0;
    long int *texto_cifrado = malloc(sizeof(long int) * MAX_MENSAJE);

    for(; mensaje[k] != -1; k++)
       texto_cifrado[k] = exponente_modular(mensaje[k], clave.e, clave.n);

    texto_cifrado[k] = -1;

    return texto_cifrado;
}

long int * descifra(long int * mensaje, Clave_privada clave) {
    int k=0;
    long int *texto_plano = malloc(sizeof(long int) * MAX_MENSAJE);
    
    for(; mensaje[k] != -1; k++)
        texto_plano[k] = exponente_modular(mensaje[k], clave.d, clave.n);

    texto_plano[k] = -1;

    return texto_plano;
}

int main() {
   Claves mis_claves = genera_claves();
   char mensaje[MAX_MENSAJE];
   long int *cifrado, *descifrado;
   long int *texto = malloc(sizeof(long int) * MAX_MENSAJE);
   int k=0;
   printf("Escriba su mensaje:\n");
   fflush(stdin);
   getchar();
   fgets(mensaje, MAX_MENSAJE, stdin);
   while(mensaje[k] != NULL) {
       texto[k] = mensaje[k];
       k++;
   }
   texto[k] = -1;
   cifrado=cifra(texto, mis_claves.publica);
   printf("\n\nEl mensaje cifrado es:\n\n\t");
   for(k=0; cifrado[k] != -1; k++)
       printf("%c", (int) cifrado[k]);
   printf("\n");
   descifrado= descifra(cifrado, mis_claves.privada);
   printf("\n\nEl mensaje descifrado es:\n\n\t");
   for(k=0; descifrado[k] != -1; k++)
       printf("%c", (int) descifrado[k]);
   printf("\n\n");
   free(texto);
   free(cifrado);
   free(descifrado);
   return 0;
}
