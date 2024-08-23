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
    'TransacaoCartaoCredito': ['int', ['string', 'string', charPtr]],
    'TransacaoCartaoVoucher': ['int', ['string', 'string', charPtr]],
    'ConfirmaCartao': ['int', ['string']],
    'DesfazCartao': ['int', ['string']],
    'FinalizaTransacao': ['int', []],
    'ConsultaCadastroPDV': ['int', [charPtr]],
});

class ClasseIntegracao {
    constructor() {
        this.handle = null;
    }

    bufferParser(buffer) {
        const result = buffer.toString('utf8').replace(/\0/g, '');  // Handle null-termination

        // if result has invalid characters, try using windows-1252 encoding
        if (result.includes('�')) {
            const decoder = new TextDecoder('windows-1252');
            return decoder.decode(buffer).replace(/\0/g, '');
        }

        return result;
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
                const pDados = this.bufferParser(pDadosBuffer);  // Handle null-termination
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
            pDados = this.bufferParser(pDadosBuffer);  // Handle null-termination
        }

        return { resultado, pDados };
    }

    TransacaoCartaoDebito(valor, cupom) {
        const pNumeroControle = Buffer.alloc(1024, 0);
        var numeroControle = '';

        let resultado = 11;

        if (libDPOSDRV.TransacaoCartaoDebito) {
            resultado = libDPOSDRV.TransacaoCartaoDebito(valor, cupom, pNumeroControle);
            // Retrieve the string from the buffer
            numeroControle = this.bufferParser(pNumeroControle);  // Handle null-termination
        }

        return { resultado, numeroControle };
    }

    TransacaoCartaoCredito(valor, cupom) {
        const pNumeroControle = Buffer.alloc(1024, 0);
        var numeroControle = '';

        let resultado = 11;

        if (libDPOSDRV.TransacaoCartaoCredito) {
            resultado = libDPOSDRV.TransacaoCartaoCredito(valor, cupom, pNumeroControle);
            // Retrieve the string from the buffer
            numeroControle = this.bufferParser(pNumeroControle);  // Handle null-termination
        }

        return { resultado, numeroControle };
    }

    TransacaoCartaoVoucher(valor, cupom) {
        const pNumeroControle = Buffer.alloc(1024, 0);
        var numeroControle = '';

        let resultado = 11;

        if (libDPOSDRV.TransacaoCartaoCredito) {
            resultado = libDPOSDRV.TransacaoCartaoVoucher(valor, cupom, pNumeroControle);
            // Retrieve the string from the buffer
            numeroControle = this.bufferParser(pNumeroControle);  // Handle null-termination
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

    DesfazCartao(numeroControle) {
        let resultado = 11;

        if (libDPOSDRV.DesfazCartao) {
            resultado = libDPOSDRV.DesfazCartao(numeroControle);
        }

        return resultado;
    }

    FinalizaTransacao() {
        let resultado = 11;

        if (libDPOSDRV.FinalizaTransacao) {
            resultado = libDPOSDRV.FinalizaTransacao();
        }

        return resultado;
    }

    ConsultaCadastroPDV() {
        let resultado = 11;
        const pMensagem = Buffer.alloc(1024, 0);

        if (libDPOSDRV.ConsultaCadastroPDV) {
            resultado = libDPOSDRV.ConsultaCadastroPDV(pMensagem);
            // Retrieve the string from the buffer
            const mensagem = this.bufferParser(pMensagem);  // Handle null-termination
            return { resultado, mensagem };
        }

        return { resultado, mensagem: '' };
    }
}

module.exports = ClasseIntegracao;
