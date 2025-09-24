#include "funciones.h"

bool existeEncriptado(int indice) {
    string nombre = "Encriptado" + to_string(indice) + ".txt";
    ifstream archivo;
    archivo.open(nombre, ios::binary);
    return archivo.is_open();
}

bool existePista(int indice) {
    string nombre = "pista" + to_string(indice) + ".txt";
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

    datos[tam] = '\0'; // Para trabajar como string
    longitud = static_cast<size_t>(tam);
    return OK;
}

bool verificarLongitud(size_t longitud, size_t minimo) {
    return longitud >= minimo;
}
