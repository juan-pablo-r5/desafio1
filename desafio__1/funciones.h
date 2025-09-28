#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
#include <fstream>
#include <cstddef>
using namespace std;

// Códigos de error
#define OK 0
#define ERR_ARCHIVO_NO_ENCONTRADO -1
#define ERR_MEMORIA -2
#define ERR_IO -3

// Comprobación de existencia
bool existeEncriptado(int indice);
bool existePista(int indice);

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

// Mostrar bytes en hex
void mostrarHex(const unsigned char* datos, size_t longitud);

// Comprimir un texto con RLE (pista en texto claro)
// Devuelve salida en formato de ternas: (basura, conteo, caracter)
unsigned char* comprimirRLE(const char* texto, size_t longitud, size_t& out_len);

// Buscar valores n y K probando pista comprimida
bool encontrarClaveConPista(const unsigned char* datosEnc, size_t lenEnc,
                            const unsigned char* pistaComp, size_t lenPista,
                            int& n_encontrado, int& k_encontrado);

#endif

