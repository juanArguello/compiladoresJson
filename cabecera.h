// Librerias utilizadas 
# include<stdio.h>
# include<string.h>
# include<stdlib.h>
# include<ctype.h>



// Definicion de tokens con valores numericos simbolicos
#define L_CORCHETE			256
#define R_CORCHETE			257
#define L_LLAVE				258
#define R_LLAVE				259
#define COMA				260
#define DOS_PUNTOS			261
#define LITERAL_CADENA		262
#define LITERAL_NUM			263
#define PR_TRUE				264
#define PR_FALSE			265
#define PR_NULL				266


// constantes de tamanhos
#define TAMANHOBUFFER 		5		// tamanho del buffer
#define TAMANHOLEXICO 		50		// tamanho del lexema
#define TAMANHOHASH 		101		// tamanho de la tabla hash


// estructura de entrada
typedef struct entrada{
	int componenteLexico;			// componente lexico
	char lexema[TAMANHOLEXICO];		// lexema
	struct entrada *tipoDato; 		// null puede representar variable no declarada
}entrada;

// estructura de tokens
typedef struct {
	int componenteLexico;			// componente lexico
	entrada *pe;
} token;

// prototipos de funcion y procedimientos
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void getToken();
