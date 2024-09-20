const WebSocket = require('ws');
const readline = require('readline');

const rl = readline.createInterface({ input: process.stdin, output: process.stdout });

// Define the RequestType enum (this should match the server-side enum)
const RequestType = Object.freeze({
    INICIALIZA: 'inicializa',
    FINALIZA: 'finaliza',
    VERSAO: 'versao',
    PROCURA: 'procura',
    TRANSACAO_DEBITO: 'transacaoDebito',
    TRANSACAO_CREDITO: 'transacaoCredito',
    TRANSACAO_VOUCHER: 'transacaoVoucher',
    CONFIRMA: 'confirma',
    DESFAZ: 'desfaz',
    CONSULTA_PDV: 'consultaPDV',
    DISPLAY: 'display',
    MENSAGEM_TERMINAL: 'terminal',
    ERROR: 'error',
    MESSAGE: 'message',
    BEEP: 'beep',
    SELECIONA_OP: 'seleciona_op',
    OBTEM_COMPROVANTE: 'obtemComprovante',
    CANCELA_OPERACAO: 'cancelaOperacao',
    MENSAGEM_ALERTA: 'mensagemAlerta',
    MENSAGEM_ADICIONAL: 'mensagemAdicional',
    PREVIEW_COMPROVANTE: 'previewComprovante',
    CANCELAMENTO_PAGAMENTO: 'cancelamentoPagamento',
    SOLICITA_CONFIRMACAO: 'solicitaConfirmacao',
});

class WebSocketClient {
    constructor(url = 'ws://localhost:8080') {
        this.url = url;
        this.ws = new WebSocket(url);
        this.setupWebSocket();
    }

    setupWebSocket() {
        this.ws.on('open', () => {
            console.log('Connected to WebSocket server');
            // Example: Automatically send a version request on connection
            this.sendRequest(RequestType.VERSAO);
        });

        this.ws.on('message', (data) => {
            console.log('Received message:', data);
            const response = JSON.parse(data);
            this.handleResponse(response);
        });

        this.ws.on('close', () => {
            console.log('Disconnected from WebSocket server');
        });

        this.ws.on('error', (error) => {
            console.error('WebSocket error:', error);
        });
    }

    sendRequest(requestType, payload = {}) {
        const request = { requestType, ...payload };

        request.message_ident = Math.floor(Math.random() * 1000);

        console.log('Sending request:', request);
        this.ws.send(JSON.stringify(request));
    }

    handleResponse(response) {
        console.log('Handling response:', response);
        switch (response.requestType) {
            case RequestType.INICIALIZA:
                console.log('Initialization result:', response.retn);
                break;
            case RequestType.FINALIZA:
                console.log('Finalization result:', response.retn);
                break;
            case RequestType.VERSAO:
                console.log('DPOS Version:', response);
                break;
            case RequestType.PROCURA:
                console.log('PinPad Search Result:', response);
                break;
            case RequestType.TRANSACAO_DEBITO:
            case RequestType.TRANSACAO_CREDITO:
            case RequestType.TRANSACAO_VOUCHER:
                console.log('Transaction Result:', response);


                this.sendRequest(RequestType.OBTEM_COMPROVANTE, { numeroControle: response.numeroControle });

                // Example: Automatically confirm the transaction
                this.sendRequest(RequestType.CONFIRMA, { numeroControle: response.numeroControle });


                setTimeout(() => {
                    this.cancelamentoPagamento(response.numeroControle);
                }, 5000);
                break;
            case RequestType.CONFIRMA:
                console.log('Confirm Result:', response.retn);
                break;
            case RequestType.CONSULTA_PDV:
                console.log('PDV Search Result:', response);
                break;
            case RequestType.DISPLAY:
                console.log('Display Message:', response);
                break;
            case RequestType.ERROR:
                console.error('Error:', response);
                break;
            case RequestType.MESSAGE:
                console.log('Message:', response);
                break;
            case RequestType.MENSAGEM_TERMINAL:
                console.log('Terminal:', response);
                break;
            case RequestType.MENSAGEM_ALERTA:
                console.log('Alerta:', response);
                break;
            case RequestType.BEEP:
                console.log('Beep!');
                break;
            case RequestType.SELECIONA_OP:
                console.log('Message:', response);

                rl.question('Digite a opção desejada: ' + response.opcoes, (answer) => {
                    this.sendRequest(RequestType.SELECIONA_OP, { op: answer });
                });

                break;

            case RequestType.OBTEM_COMPROVANTE:
                console.log('Comprovante:', response);
                break;
            case RequestType.SOLICITA_CONFIRMACAO:
                console.log('Message:', response);

                rl.question('Digite a resposta: ' + response.mensagem + ' resposta com código 0 ou 1:', (answer) => {
                    this.sendRequest(RequestType.SOLICITA_CONFIRMACAO, { op: answer });
                });

                break;
            default:
                console.warn('Unknown response type:', response.requestType);
        }
    }

    cancelamentoPagamento(numeroControle) {
        this.sendRequest(RequestType.CANCELAMENTO_PAGAMENTO, { numeroControle });
    }

    confirma(numeroControle) {
        this.sendRequest(RequestType.CONFIRMA, { numeroControle });
    }

    desfaz(numeroControle) {
        this.sendRequest(RequestType.DESFAZ, { numeroControle });
    }

    finaliza() {
        this.sendRequest(RequestType.FINALIZA);
    }
}

// get env url
const url = process.env.WS_URL || 'ws://localhost:8080';

// Example usage
const client = new WebSocketClient(url);

// type the valor
const valor = process.env.VALOR || '1000';
const cupom = process.env.CUPOM || '123456';

// You can send different types of requests like this:
setTimeout(() => {
    client.sendRequest(RequestType.INICIALIZA);

    setTimeout(() => {
        client.sendRequest(RequestType.PROCURA);
    }, 1000);

    /* setTimeout(() => {
        client.sendRequest(RequestType.CONSULTA_PDV);
    }, 2000);*/

    setTimeout(() => {
        client.sendRequest(RequestType.TRANSACAO_DEBITO, { valor: valor, cupom: cupom });
    }, 5000);
}, 1000);


// handle ctrl c
/*process.on('SIGINT', function () {
    console.log('Caught interrupt signal');
    client.sendRequest(RequestType.CANCELA_OPERACAO);

    setTimeout(() => {
        client.finaliza();
        process.exit();
    }, 5000);
});
*/