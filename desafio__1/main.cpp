#include "Funciones.h"
#include <cstdio> // snprintf

int main() {
    cout << "Cuantos archivos desea evaluar? ";
    int n;
    cin >> n;

    if (n <= 0) {
        cout << "Numero invalido. Debe ser mayor que 0.\n";
        return 0;
    }

    for (int i = 1; i <= n; i++) {
        if (!existeEncriptado(i) || !existePista(i)) {
            cout << "Caso " << i << ": archivos no encontrados.\n";
            continue;
        }

        char nombreEnc[64];
        char nombrePista[64];
        snprintf(nombreEnc, sizeof(nombreEnc), "Encriptado%d.txt", i);
        snprintf(nombrePista, sizeof(nombrePista), "pista%d.txt", i);

        unsigned char* datosEnc = nullptr;
        char* datosPista = nullptr;
        size_t lenEnc = 0, lenPista = 0;

        int r1 = leerArchivoBinario(nombreEnc, datosEnc, lenEnc);
        int r2 = leerArchivoTexto(nombrePista, datosPista, lenPista);

        if (r1 != OK || r2 != OK) {
            cout << "Caso " << i << ": error al leer.\n";
            delete[] datosEnc;
            delete[] datosPista;
            continue;
        }

        cout << "\nCaso " << i << ":\n";
        cout << " Archivo encriptado: '" << nombreEnc << "' (" << lenEnc << " bytes)\n";
        cout << " Archivo pista: '" << nombrePista << "' (" << lenPista << " bytes)\n";

        cout << " Pista (texto): '" << datosPista << "'\n";

        // Comprimir pista en RLE y LZ78
        size_t lenRLE = 0, lenLZ = 0;
        unsigned char* pistaRLE = comprimirRLE(datosPista, lenPista, lenRLE);
        unsigned char* pistaLZ  = comprimirLZ78(datosPista, lenPista, lenLZ);

        cout << " Pista comprimida en RLE (hex): ";
        mostrarHex(pistaRLE, lenRLE);
        cout << " Pista comprimida en LZ78 (hex): ";
        mostrarHex(pistaLZ, lenLZ);

        int n_val = -1, k_val = -1;
        // Probar primero RLE
        bool okRLE = encontrarClaveConPistaRLE(datosEnc, lenEnc, pistaRLE, lenRLE, n_val, k_val);
        if (okRLE) {
            cout << "Metodo identificado: RLE\n";
            cout << "Combinacion encontrada: n=" << n_val
                 << ", k=0x" << hex << uppercase << k_val << dec << "\n";

            unsigned char* datosDescifrados = desencriptar(datosEnc, lenEnc, n_val, (unsigned char)k_val);
            size_t lenTexto = 0;
            unsigned char* textoPlano = descomprimirRLE(datosDescifrados, lenEnc, lenTexto);

            cout << "Texto descomprimido:\n";
            for (size_t j = 0; j < lenTexto; j++) cout << (char)textoPlano[j];
            cout << "\n";

            delete[] datosDescifrados;
            delete[] textoPlano;
        } else {
            // Probar LZ78
            bool okLZ = encontrarClaveConPistaLZ78(datosEnc, lenEnc, pistaLZ, lenLZ, n_val, k_val);
            if (okLZ) {
                cout << "Metodo identificado: LZ78\n";
                cout << "Combinacion encontrada: n=" << n_val
                     << ", k=0x" << hex << uppercase << k_val << dec << "\n";

                unsigned char* datosDescifrados = desencriptar(datosEnc, lenEnc, n_val, (unsigned char)k_val);
                size_t lenTexto = 0;
                char* textoPlano = descomprimirLZ78(datosDescifrados, lenEnc, lenTexto);

                cout << "Texto descomprimido:\n";
                for (size_t j = 0; j < lenTexto; j++) cout << textoPlano[j];
                cout << "\n";

                delete[] datosDescifrados;
                delete[] textoPlano;
            } else {
                cout << "No se pudo identificar el metodo de compresion.\n";
            }
        }

        delete[] datosEnc;
        delete[] datosPista;
        delete[] pistaRLE;
        delete[] pistaLZ;
    }

    cout << "\nProcesamiento finalizado.\n";
    return 0;
}

