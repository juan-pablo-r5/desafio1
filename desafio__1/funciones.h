#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
#include <fstream>
#include <cstddef>
#include <new>
using namespace std;

// Codigos de error
#define OK 0
#define ERR_ARCHIVO_NO_ENCONTRADO -1
#define ERR_MEMORIA -2
#define ERR_IO -3

bool existeEncriptado(int indice);
bool existePista(int indice);

// (binario)
int leerArchivoBinario(const char* nombre, unsigned char*& datos, size_t& longitud);
// (texto)
int leerArchivoTexto(const char* nombre, char*& datos, size_t& longitud);

// Verificacion de longitud minima
bool verificarLongitud(size_t longitud, size_t minimo);
unsigned char rotarDerecha(unsigned char valor, int n);

// Intentar desencriptar con una combinación (n, K)
unsigned char* desencriptar(const unsigned char* datosEnc, size_t longitud, int n, unsigned char K);

// Mostrar bytes en hex
void mostrarHex(const unsigned char* datos, size_t longitud);

// Comprimir un texto con RLE (pista en texto claro)
// Devuelve salida en formato de ternas: (basura, conteo, caracter)
unsigned char* comprimirRLE(const char* texto, size_t longitud, size_t& out_len);
// Nueva función: comprimir pista con LZ78 (formato ternas)
unsigned char* comprimirLZ78(const char* texto, size_t longitud, size_t& out_len);


// Buscar valores n y K probando pista comprimida
bool encontrarClaveConPista(const unsigned char* datosEnc, size_t lenEnc,
                                 const unsigned char* pistaComp, size_t lenPista,
                                 int& n_encontrado, int& k_encontrado);


unsigned char* descomprimirRLE(const unsigned char* datos, size_t longitud, size_t& out_len);
char* descomprimirLZ78(const unsigned char* datos, size_t lenDatos, size_t& lenSalida);


#endif

