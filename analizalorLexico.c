// Libreria utilizadas 
#include "tablaSimbolos.c"


// Variables globales 
int consumir;			
char cadena[5*TAMANHOLEXICO];// string utilizado para cargar mensajes de error
token t;// token global

// variables para el analizador lexico
FILE *archivo;				// Fuente txt en formato json
FILE *salida;				// Salida del analisis lexico
char buffer[2*TAMANHOBUFFER];	// Buffer para lectura de archivo fuente
char id[TAMANHOLEXICO];		// Utilizado por el analizador lexico
int delantero=-1;			// Utilizado por el analizador lexico
int fin=0;					// Utilizado por el analizador lexico
int numeroLinea=1;			// Numero de Linea


// Funciones y procedimientos

void error(const char* mensaje)
{
	printf("Linea %d: Error Lexico. %s.\n",numeroLinea,mensaje);	
}

void getToken(){
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF){
		
		if (c==' '){
			 do{
				fputs(" ",salida);
                c=fgetc(archivo); 
            }while(c ==' ');
            c=ungetc(c,archivo);
		}else if(c=='\t'){
			 while(c=='\t'){
				fputs("\t",salida);
	        	c=fgetc(archivo);
            }
		}else if(c=='\n'){
			//incrementar el numero de linea
			numeroLinea++;
			fputs("\n",salida);
			continue;
		}else if (isalpha(c)){
			//palabra reservada true false null TRUE FALSE NULL
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMANHOLEXICO)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			if(strcmp(id,"true")==0){
				t.componenteLexico=PR_TRUE;
				t.pe=buscar("true");
				fputs("PR_TRUE",salida);
				fputs(" ",salida);
			}else if(strcmp(id,"TRUE")==0){
				t.componenteLexico=PR_TRUE;
				t.pe=buscar("TRUE");
				fputs("PR_TRUE",salida);
				fputs(" ",salida);
			}else if(strcmp(id,"false")==0){
				t.componenteLexico=PR_FALSE;
				t.pe=buscar("false");
				fputs("PR_FALSE",salida);
				fputs(" ",salida);
			}else if(strcmp(id,"FALSE")==0){
				t.componenteLexico=PR_FALSE;
				t.pe=buscar("FALSE");
				fputs("PR_FALSE",salida);
				fputs(" ",salida);
			}else if(strcmp(id,"null")==0){
				t.componenteLexico=PR_NULL;
				t.pe=buscar("null");
				fputs("PR_NULL",salida);
				fputs(" ",salida);
			}else if(strcmp(id,"NULL")==0){
				t.componenteLexico=PR_NULL;
				t.pe=buscar("NULL");
				fputs("PR_NULL",salida);
				fputs(" ",salida);
			}else{
				error("Error lexico");
				fputs("Error lexico",salida);
				while(c!='\n')
					c=fgetc(archivo);
				ungetc(c,archivo);
			}
			break;
		}else if (isdigit(c)){
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto){
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c)){
							id[++i]=c;
							estado=0;
						}else if(c=='.'){
							id[++i]=c;
							estado=1;
						}else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c)){
							id[++i]=c;
							estado=2;
						}else if(c=='.'){
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}else{
							sprintf(msg,"No se esperaba '%c'",c);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c)){
							id[++i]=c;
							estado=2;
						}else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-'){
							id[++i]=c;
							estado=4;
						}else if(isdigit(c)){
							id[++i]=c;
							estado=5;
						}else{
							sprintf(msg,"No se esperaba '%c'",c);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c)){
							id[++i]=c;
							estado=5;
						}else{
							sprintf(msg,"No se esperaba '%c'",c);
							fputs("Error lexico",salida);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c)){
							id[++i]=c;
							estado=5;
						}else{
							estado=6;
						}
						break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->componenteLexico==-1){
							strcpy(e.lexema,id);
							e.componenteLexico=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.componenteLexico=LITERAL_NUM;
						fputs("NUMBER",salida);
						fputs(" ",salida);
						break;
					case -1:
						if (c==EOF){
							error("No se esperaba el fin de archivo");
							fputs("Error lexico",salida);
						}else
							error(msg);
						exit(1);
					}
				}
			break;
		}else if (c==':'){
			t.componenteLexico=DOS_PUNTOS;
			t.pe=buscar(":");
			fputs("DOS_PUNTOS",salida);
			fputs(" ",salida);
			break;
		}else if (c==','){
			t.componenteLexico=COMA;
			t.pe=buscar(",");
			fputs("COMA",salida);
			fputs(" ",salida);
			break;
		}else if (c=='{'){
			t.componenteLexico=L_LLAVE;
			t.pe=buscar("{");
			fputs("L_LLAVE",salida);
			fputs(" ",salida);
			break;
		}else if (c=='}'){
			t.componenteLexico=R_LLAVE;
			t.pe=buscar("}");
			fputs("R_LLAVE",salida);
			fputs(" ",salida);
			break;
		}else if (c=='['){
			t.componenteLexico=L_CORCHETE;
			t.pe=buscar("[");
			fputs("L_CORCHETE",salida);
			fputs(" ",salida);
			break;
		}else if (c==']'){
			t.componenteLexico=R_CORCHETE;
			t.pe=buscar("]");
			fputs("R_CORCHETE",salida);
			fputs(" ",salida);
			break;
		}else if ( c == '\"'){
			//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\"'){
					c=fgetc(archivo);
					if (c=='\"'){
						id[i]=c;
						i++;
						id[i]=c;
						i++;
					}else{
						id[i]='\"';
						i++;
						break;
					}
				}else if(c==EOF || c==',' || c=='\n' || c==':'){
					sprintf(msg,"Se esperaba que finalice el literal");
					error(msg);
					fputs("Error lexico",salida);
                    
                    while(c!='\n')
                        c=fgetc(archivo);

                    ungetc(c,archivo);
                    break; 
				}else if(!isgraph(c)){
					switch(c){
						case 'ñ':
							id[i]='\244';
							i++;
							break;
						case 'Ñ':
							id[i]='\245';
							i++;
							break;
						case 'á':
							id[i]='\240';
							i++;
							break;
						case 'í':
							id[i]='\241';
							i++;
							break;
						case 'ó':
							id[i]='\242';
							i++;
							break;
						case 'ú':
							id[i]='\243';
							i++;
							break;
						case 'é':
							id[i]='\202';
							i++;
							break;
						default:
							break;
					}
				}else{
					id[i]=c;
					i++;
				}
			}while(isascii(c) || !isgraph(c) );
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			strcpy(e.lexema,id);
			e.componenteLexico=LITERAL_CADENA;
			insertar(e);
			t.pe=buscar(id);
			t.componenteLexico=LITERAL_CADENA;
			fputs("STRING",salida);
			fputs(" ",salida);
			break;
		}else if (c!=EOF){
			sprintf(msg,"%c no esperado",c);
			error(msg);
			fputs("Error lexico",salida);
            while(c!='\n')
                c=fgetc(archivo);
            ungetc(c,archivo);
		}
	}
	if (c==EOF){
		t.componenteLexico=EOF;
		// strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}	
}

int main(int argc,char* args[]){
	// inicializar analizador lexico
	initTabla();
	initTablaSimbolos();
	
	if(argc > 1){
		if (!(archivo=fopen(args[1],"rt"))){
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		salida = fopen("output.txt","w");
		while (t.componenteLexico!=EOF){
			getToken();
		}
		fclose(archivo);
		fclose(salida);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}
