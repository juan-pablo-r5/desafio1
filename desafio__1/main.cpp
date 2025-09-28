#include"Funciones.h"

#include <cstdio> // para snprintf

int main() {
    cout << "Cuantos archivos desea evaluar? ";
    int n;
    cin >> n;

    if ( n <= 0 ) {
        cout << "Numero invalido. Debe ser mayor que 0.\n";
        return 0;
    }

    for ( int i = 1; i <= n; i++ ) {
        if ( !existeEncriptado(i) || !existePista(i) ) {
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

        if ( r1 != OK || r2 != OK ) {
            cout << "Caso " << i << ": error al leer.\n";
            delete[] datosEnc;
            delete[] datosPista;
            continue;
        }

        cout << "\nCaso " << i << ":\n";
        cout << " Archivo encriptado: '" << nombreEnc << "' (" << lenEnc << " bytes)\n";
        cout << " Archivo pista: '" << nombrePista << "' (" << lenPista << " bytes)\n";

        size_t preview = lenEnc < 32 ? lenEnc : 32;
        cout << " Vista previa (hex) primeros " << preview << " bytes: ";
        mostrarHex(datosEnc, preview);

        cout << " Pista (texto): '" << datosPista << "'\n";

        // Comprimir pista en RLE (formato ternas)
        size_t lenRLE = 0;
        unsigned char* pistaRLE = comprimirRLE(datosPista, lenPista, lenRLE);

        int n_val = -1, k_val = -1;
        bool ok = encontrarClaveConPista(datosEnc, lenEnc, pistaRLE, lenRLE, n_val, k_val);

        if (ok) {
            cout << "Combinacion encontrada: n=" << n_val
                 << ", k=0x" << hex << uppercase << k_val << dec << "\n";

        } else {
            cout << " No se encontro ninguna combinacion con RLE.\n";
        }

        delete[] datosEnc;
        delete[] datosPista;
        delete[] pistaRLE;
    }

    cout << "\nProcesamiento finalizado.\n";
    return 0;
}

