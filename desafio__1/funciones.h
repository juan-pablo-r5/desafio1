#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
#include <fstream>
using namespace std;

// Códigos de error
#define OK 0
#define ERR_ARCHIVO_NO_ENCONTRADO -1
#define ERR_MEMORIA -2
#define ERR_IO -3

// Comprobación de existencia
bool existeEncriptado(int indice);
bool existePista(int indice);
// Comprimir un texto con RLE (pista en texto claro)
unsigned char* comprimirRLE(const char* texto, size_t longitud, size_t& out_len);

// Mostrar en hex el resultado de compresión (para depuración)
void mostrarHex(const unsigned char* datos, size_t longitud);

// Lectura de encriptado (binario)
int leerArchivoBinario(const char* nombre, unsigned char*& datos, size_t& longitud);

// Lectura de pista (texto)
int leerArchivoTexto(const char* nombre, char*& datos, size_t& longitud);

// Verificación de longitud mínima
bool verificarLongitud(size_t longitud, size_t minimo);

// Rotación a la derecha de un byte
unsigned char rotarDerecha(unsigned char valor, int n);

// Intentar desencriptar con una combinación (n, K)
unsigned char* desencriptar(const unsigned char* datosEnc, size_t longitud, int n, unsigned char K);


#endif

