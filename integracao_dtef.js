const ffi = require('ffi-napi');
const ref = require('ref-napi');

// Define the callback function types using ffi.Function
const ptrCallBackDisplayTerminal = ffi.Function('void', ['string']);
const ptrCallBackDisplayErro = ffi.Function('void', ['string']);
const ptrCallBackMensagem = ffi.Function('void', ['string']);
const ptrCallBackBeep = ffi.Function('void', []);
const charPtr = ref.refType('string');

// Load the shared library
const libDPOSDRV = ffi.Library('libDPOSDRV.so', {
    'RegPDVDisplayTerminal': ['void', [ptrCallBackDisplayTerminal]],
    'RegPDVDisplayErro': ['void', [ptrCallBackDisplayErro]],
    'RegPDVMensagem': ['void', [ptrCallBackMensagem]],
    'RegPDVBeep': ['void', [ptrCallBackBeep]],
    'InicializaDPOS': ['int', []],
    'FinalizaDPOS': ['int', []],
    'VersaoDPOS': ['int', [charPtr]],
    'ProcuraPinPad': ['int', [charPtr]],
    'TransacaoCartaoDebito': ['int', ['string', 'string', charPtr]],
    'ConfirmaCartao': ['int', ['string']],
});

class ClasseIntegracao {
    constructor() {
        this.handle = null;
    }

    carregaDll() {
        if (this.handle === null) {
            this.handle = ffi.DynamicLibrary('libDPOSDRV.so');
        }
    }

    setCallBackDisplayTerminal(cbDisplayTerminal) {
        const RegPDVDisplayTerminalPtr = libDPOSDRV.RegPDVDisplayTerminal;
        if (RegPDVDisplayTerminalPtr) {
            RegPDVDisplayTerminalPtr(cbDisplayTerminal);
        }
    }

    setCallBackDisplayErro(cbCallBackDisplayErro) {
        const RegPDVDisplayErroPtr = libDPOSDRV.RegPDVDisplayErro;
        if (RegPDVDisplayErroPtr) {
            RegPDVDisplayErroPtr(cbCallBackDisplayErro);
        }
    }

    setCallBackDisplayMensagem(cbDisplayMensagem) {
        const RegPDVMensagemPtr = libDPOSDRV.RegPDVMensagem;
        if (RegPDVMensagemPtr) {
            RegPDVMensagemPtr(cbDisplayMensagem);
        }
    }

    setCallBackBeep(cbBeep) {
        const RegPDVBeepPtr = libDPOSDRV.RegPDVBeep;
        if (RegPDVBeepPtr) {
            RegPDVBeepPtr(cbBeep);
        }
    }

    InicializaDPOS() {
        let resultado = 11;

        if (this.handle !== null) {
            const InicializaDPOSPtr = libDPOSDRV.InicializaDPOS;
            if (InicializaDPOSPtr) {
                resultado = InicializaDPOSPtr();
            }
        }

        return resultado;
    }

    FinalizaDPOS() {
        let resultado = 11;

        if (this.handle !== null) {
            const FinalizaDPOSPtr = libDPOSDRV.FinalizaDPOS;
            if (FinalizaDPOSPtr) {
                resultado = FinalizaDPOSPtr();
            }
        }

        return resultado;
    }

    VersaoDPOS() {
        // Create a buffer to hold the output data
        const pDadosBuffer = Buffer.alloc(256);

        let resultado = 11;

        if (this.handle !== null) {
            const VersaoDPOSPtr = libDPOSDRV.VersaoDPOS;
            if (VersaoDPOSPtr) {
                resultado = VersaoDPOSPtr(pDadosBuffer);
                // Retrieve the string from the buffer
                const pDados = pDadosBuffer.toString('utf8').replace(/\0/g, '');  // Handle null-termination
                return { resultado, pDados };
            }
        }

        return { resultado, pDados: '' };
    }

    ProcuraPinPad() {
        const pDadosBuffer = Buffer.alloc(1024);
        var pDados = '';

        let resultado = 11;

        const ProcuraPinPadPtr = libDPOSDRV.ProcuraPinPad;
        if (ProcuraPinPadPtr) {
            resultado = ProcuraPinPadPtr(pDadosBuffer);
            // Retrieve the string from the buffer
            pDados = pDadosBuffer.toString('utf8').replace(/\0/g, '');  // Handle null-termination
        }

        return { resultado, pDados };
    }

    TransacaoCartaoDebito(valor, cupom) {
        const pNumeroControle = Buffer.alloc(1024, 0);
        const pValor = Buffer.from(valor + '\0', 'utf8');
        const pCupom = Buffer.from(cupom + '\0', 'utf8');
        var numeroControle = '';

        let resultado = 11;

        if (libDPOSDRV.TransacaoCartaoDebito) {
            resultado = libDPOSDRV.TransacaoCartaoDebito(valor, cupom, pNumeroControle);
            // Retrieve the string from the buffer
            numeroControle = pNumeroControle.toString('utf8').replace(/\0/g, '');  // Handle null-termination
        }

        return { resultado, numeroControle };
    }

    ConfirmaCartao(numeroControle) {
        let resultado = 11;

        if (libDPOSDRV.ConfirmaCartao) {
            resultado = libDPOSDRV.ConfirmaCartao(numeroControle);
        }

        return resultado;
    }
}

module.exports = ClasseIntegracao;
