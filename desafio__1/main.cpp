#include"Funciones.h"


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

        string nombreEnc = "Encriptado" + to_string(i) + ".txt";
        string nombrePista = "pista" + to_string(i)  + ".txt";

        unsigned char* datosEnc = nullptr;
        char* datosPista = nullptr;
        size_t lenEnc = 0, lenPista = 0;

        int r1 = leerArchivoBinario(nombreEnc.c_str(), datosEnc, lenEnc);
        int r2 = leerArchivoTexto(nombrePista.c_str(), datosPista, lenPista);

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
        for ( size_t j = 0; j < preview; j++ ) {
            cout << hex << (static_cast<int>(datosEnc[j]) & 0xFF) << " ";
        }
        cout << dec << "\n";

        cout << " Pista (texto): '" << datosPista << "'\n";

        // Supongamos que ya leímos datosEnc y lenEnc
        for (int n = 0; n < 8; n++) {
            for (int K = 0; K < 256; K++) {
                unsigned char* resultado = desencriptar(datosEnc, lenEnc, n, (unsigned char)K);

                // Aquí podrías comparar con la pista o imprimir parte del texto desencriptado
                cout << "n=" << n << ", K=" << K << " -> "
                     << "primeros 8 chars: ";
                for (int j = 0; j < 8 && j < lenEnc; j++) {
                    cout << (char)resultado[j];
                }
                cout << "\n";

                delete[] resultado;
            }
        }


        delete[] datosEnc;
        delete[] datosPista;
    }

    cout << "\nProcesamiento finalizado.\n";
    return 0;
}
