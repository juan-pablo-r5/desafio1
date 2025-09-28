#include "funciones.h"
#include <cstdio>   // snprintf
#include <cstring>  // memcmp
#include <new>


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

    datos[tam] = '\0';
    longitud = static_cast<size_t>(tam);
    return OK;
}

bool verificarLongitud(size_t longitud, size_t minimo) {
    return longitud >= minimo;
}

unsigned char rotarDerecha(unsigned char valor, int n) {
    return (unsigned char)((valor >> n) | (valor << (8 - n)));
}

unsigned char* desencriptar(const unsigned char* datosEnc, size_t longitud, int n, unsigned char K) {
    unsigned char* salida = new unsigned char[longitud];
    for (size_t i = 0; i < longitud; i++) {
        unsigned char temp = datosEnc[i] ^ K;        // XOR
        salida[i] = rotarDerecha(temp, n);           // rotación inversa
    }
    return salida;
}

void mostrarHex(const unsigned char* datos, size_t longitud) {
    for (size_t i = 0; i < longitud; i++) {
        cout << hex << (static_cast<int>(datos[i]) & 0xFF) << " ";
    }
    cout << dec << "\n";
}

/* =====================
   RLE - Ternas (basura, conteo, caracter)
   ===================== */
unsigned char* comprimirRLE(const char* texto, size_t longitud, size_t& out_len) {
    if (!texto || longitud == 0) {
        out_len = 0;
        return nullptr;
    }

    unsigned char* salida = new unsigned char[longitud * 3];
    size_t j = 0;

    for (size_t i = 0; i < longitud; ) {
        char actual = texto[i];
        size_t conteo = 1;

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
    if (lenPista > lenEnc) return false;

    for (int n = 1; n < 8; n++) {
        for (int K = 0; K < 256; K++) {
            unsigned char* resultado = desencriptar(datosEnc, lenEnc, n, (unsigned char)K);

            bool contiene = false;
            // recorrer byte a byte en resultado
            for (size_t i = 0; i + lenPista <= lenEnc; i++) {
                if (memcmp(resultado + i, pistaComp, lenPista) == 0) {
                    contiene = true;
                    break;
                }
            }

            delete[] resultado;

            if (contiene) {
                n_encontrado = n;
                k_encontrado = K;
                return true;
            }
        }
    }
    return false;
}




unsigned char* descomprimirRLE(const unsigned char* datos, size_t longitud, size_t& out_len) {
    if (!datos || longitud == 0 || longitud % 3 != 0) {
        out_len = 0;
        return nullptr;
    }

    size_t capacidad = longitud * 255 / 3 + 1;
    unsigned char* salida = new unsigned char[capacidad];
    size_t j = 0;

    for (size_t i = 0; i + 2 < longitud; i += 3) {
        unsigned char conteo   = datos[i + 1]; // segundo byte = repeticiones
        unsigned char caracter = datos[i + 2]; // tercer byte = carácter

        for (int k = 0; k < conteo; k++) {
            salida[j++] = caracter;
        }
    }

    out_len = j;
    return salida;
}

/* =====================
   LZ78 (ternas: [índice alto, índice bajo, caracter])
   ===================== */
char* descomprimirLZ78(const unsigned char* datos, size_t lenDatos, size_t& lenSalida) {
    if (lenDatos % 3 != 0) {
        lenSalida = 0;
        return nullptr; // formato inválido
    }

    size_t capacidad = 1024;
    size_t numEntradas = 1;
    char** diccionario = new char*[capacidad];
    diccionario[0] = new char[1];
    diccionario[0][0] = '\0';

    // Buffer dinámico para la salida
    size_t capacidadOut = lenDatos * 4; // tamaño estimado
    char* salida = new char[capacidadOut];
    lenSalida = 0;

    for (size_t i = 0; i < lenDatos; i += 3) {
        unsigned short indice = (datos[i] << 8) | datos[i+1];
        char nuevoChar = (char)datos[i+2];

        // Construir nueva entrada: diccionario[indice] + nuevoChar
        size_t lenPrefijo = strlen(diccionario[indice]);
        size_t lenNueva = lenPrefijo + 1;

        char* nuevaEntrada = new char[lenNueva + 1];
        strcpy(nuevaEntrada, diccionario[indice]);
        nuevaEntrada[lenPrefijo] = nuevoChar;
        nuevaEntrada[lenNueva] = '\0';

        // Asegurar espacio en salida
        if (lenSalida + lenNueva >= capacidadOut) {
            capacidadOut *= 2;
            char* nuevaSalida = new char[capacidadOut];
            memcpy(nuevaSalida, salida, lenSalida);
            delete[] salida;
            salida = nuevaSalida;
        }

        // Copiar nueva entrada al texto descomprimido
        memcpy(salida + lenSalida, nuevaEntrada, lenNueva);
        lenSalida += lenNueva;

        // Insertar en diccionario
        if (numEntradas >= capacidad) {
            capacidad *= 2;
            char** nuevoDic = new char*[capacidad];
            for (size_t k = 0; k < numEntradas; k++) nuevoDic[k] = diccionario[k];
            delete[] diccionario;
            diccionario = nuevoDic;
        }
        diccionario[numEntradas++] = nuevaEntrada;
    }

    // Liberar memoria del diccionario
    for (size_t k = 0; k < numEntradas; k++) delete[] diccionario[k];
    delete[] diccionario;

    // Agregar terminador
    salida[lenSalida] = '\0';
    return salida;
}

unsigned char* comprimirLZ78(const char* texto, size_t lenTexto, size_t& lenSalida) {
    // Diccionario dinámico
    size_t capacidad = 1024;
    size_t numEntradas = 1; // índice 0 = ""
    char** diccionario = new char*[capacidad];
    diccionario[0] = new char[1];
    diccionario[0][0] = '\0';

    // Buffer de salida (máx: 3 bytes por caracter)
    unsigned char* salida = new unsigned char[lenTexto * 3];
    size_t posSalida = 0;

    size_t i = 0;
    while (i < lenTexto) {
        int mejorIndice = 0;
        size_t mejorLong = 0;

        // Buscar el prefijo más largo
        for (size_t idx = 0; idx < numEntradas; idx++) {
            size_t longPrefijo = strlen(diccionario[idx]);
            if (longPrefijo == 0) continue;
            if (i + longPrefijo > lenTexto) continue;
            if (strncmp(&texto[i], diccionario[idx], longPrefijo) == 0) {
                if (longPrefijo > mejorLong) {
                    mejorLong = longPrefijo;
                    mejorIndice = (int)idx;
                }
            }
        }

        // Si no queda caracter para "nuevoChar", paramos
        if (i + mejorLong >= lenTexto) break;

        char nuevoChar = texto[i + mejorLong];

        // Guardar salida: índice (2 bytes) + nuevoChar
        salida[posSalida++] = (mejorIndice >> 8) & 0xFF;
        salida[posSalida++] = mejorIndice & 0xFF;
        salida[posSalida++] = (unsigned char)nuevoChar;

        // Construir nueva entrada
        size_t lenNueva = mejorLong + 1;
        char* nuevaEntrada = new char[lenNueva + 1];

        if (mejorIndice == 0) {
            strncpy(nuevaEntrada, &texto[i], lenNueva);
        } else {
            strcpy(nuevaEntrada, diccionario[mejorIndice]);
            nuevaEntrada[mejorLong] = nuevoChar;
        }
        nuevaEntrada[lenNueva] = '\0';

        // Insertar en diccionario
        if (numEntradas >= capacidad) {
            capacidad *= 2;
            char** nuevoDic = new char*[capacidad];
            for (size_t k = 0; k < numEntradas; k++) nuevoDic[k] = diccionario[k];
            delete[] diccionario;
            diccionario = nuevoDic;
        }
        diccionario[numEntradas++] = nuevaEntrada;

        // Avanzar
        i += mejorLong + 1;
    }

    lenSalida = posSalida;

    // Liberar diccionario
    for (size_t k = 0; k < numEntradas; k++) delete[] diccionario[k];
    delete[] diccionario;

    return salida;
}
