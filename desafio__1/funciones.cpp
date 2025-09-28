#include "funciones.h"
#include <cstdio>   // para snprintf
#include <cstring>  // para memcmp

bool existeEncriptado(int indice) {
    char nombre[64];
    snprintf(nombre, sizeof(nombre), "Encriptado%d.txt", indice);
    ifstream archivo;
    archivo.open(nombre, ios::binary);
    return archivo.is_open();
}

bool existePista(int indice) {
    char nombre[64];
    snprintf(nombre, sizeof(nombre), "pista%d.txt", indice);
    ifstream archivo;
    archivo.open(nombre);
    return archivo.is_open();
}

int leerArchivoBinario(const char* nombre, unsigned char*& datos, size_t& longitud) {
    ifstream archivo(nombre, ios::binary | ios::ate);
    if ( !archivo.is_open() ) return ERR_ARCHIVO_NO_ENCONTRADO;

    streamsize tam = archivo.tellg();
    if ( tam < 0 ) return ERR_IO;
    archivo.seekg(0, ios::beg);

    datos = new(nothrow) unsigned char[tam];
    if ( !datos ) return ERR_MEMORIA;

    if ( !archivo.read(reinterpret_cast<char*>(datos), tam) ) {
        delete[] datos;
        return ERR_IO;
    }

    longitud = static_cast<size_t>(tam);
    return OK;
}

int leerArchivoTexto(const char* nombre, char*& datos, size_t& longitud) {
    ifstream archivo(nombre, ios::ate);
    if ( !archivo.is_open() ) return ERR_ARCHIVO_NO_ENCONTRADO;

    streamsize tam = archivo.tellg();
    if ( tam < 0 ) return ERR_IO;
    archivo.seekg(0, ios::beg);

    datos = new(nothrow) char[tam + 1];
    if ( !datos ) return ERR_MEMORIA;

    if ( !archivo.read(datos, tam) ) {
        delete[] datos;
        return ERR_IO;
    }

    datos[tam] = '\0'; // Para trabajar como string C
    longitud = static_cast<size_t>(tam);
    return OK;
}

bool verificarLongitud(size_t longitud, size_t minimo) {
    return longitud >= minimo;
}

unsigned char rotarDerecha(unsigned char valor, int n) {
    return (valor >> n) | (valor << (8 - n));
}

unsigned char* desencriptar(const unsigned char* datosEnc, size_t longitud, int n, unsigned char K) {
    unsigned char* salida = new unsigned char[longitud];
    for (size_t i = 0; i < longitud; i++) {
        unsigned char temp = datosEnc[i] ^ K;        // paso 1: XOR
        salida[i] = rotarDerecha(temp, n);           // paso 2: rotación inversa
    }
    return salida; // el llamador debe liberar con delete[]
}

void mostrarHex(const unsigned char* datos, size_t longitud) {
    for (size_t i = 0; i < longitud; i++) {
        cout << hex << (static_cast<int>(datos[i]) & 0xFF) << " ";
    }
    cout << dec << "\n";
}

unsigned char* comprimirRLE(const char* texto, size_t longitud, size_t& out_len) {
    if (!texto || longitud == 0) {
        out_len = 0;
        return nullptr;
    }

    // Peor caso: cada caracter distinto → 3 bytes (basura, conteo, caracter)
    unsigned char* salida = new unsigned char[longitud * 3];
    size_t j = 0;

    for (size_t i = 0; i < longitud; ) {
        char actual = texto[i];
        size_t conteo = 1;

        // contar repeticiones
        while (i + conteo < longitud && texto[i + conteo] == actual && conteo < 255) {
            conteo++;
        }

        salida[j++] = 0x00;                  // byte basura
        salida[j++] = (unsigned char)conteo; // conteo
        salida[j++] = (unsigned char)actual; // caracter

        i += conteo;
    }

    out_len = j;
    return salida;
}

bool encontrarClaveConPista(const unsigned char* datosEnc, size_t lenEnc,
                            const unsigned char* pistaComp, size_t lenPista,
                            int& n_encontrado, int& k_encontrado) {
    for (int n = 1; n < 8; n++) {               // ✅ rango válido 1..7
        for (int K = 0; K < 256; K++) {
            unsigned char* resultado = desencriptar(datosEnc, lenEnc, n, (unsigned char)K);

            bool contiene = false;
            for (size_t i = 0; i + lenPista <= lenEnc; i++) {
                if (memcmp(resultado + i, pistaComp, lenPista) == 0) {
                    contiene = true;
                    break;
                }
            }

            if (contiene) {
                n_encontrado = n;
                k_encontrado = K;
                delete[] resultado;
                return true;
            }

            delete[] resultado;
        }
    }
    return false;
}

