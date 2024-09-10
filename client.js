const WebSocket = require('ws');

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
    CONSULTA_PDV: 'consultaPDV',
    DISPLAY: 'display',
    ERROR: 'error',
    MESSAGE: 'message',
    BEEP: 'beep',
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
        console.log('Sending request:', request);
        this.ws.send(JSON.stringify(request));
    }

    handleResponse(response) {
        console.log('Handling response:', response);
        switch (response.type) {
            case RequestType.INICIALIZA:
                console.log('Initialization result:', response.result);
                break;
            case RequestType.FINALIZA:
                console.log('Finalization result:', response.result);
                break;
            case RequestType.VERSAO:
                console.log('DPOS Version:', response.result);
                break;
            case RequestType.PROCURA:
                console.log('PinPad Search Result:', response.result);
                break;
            case RequestType.TRANSACAO_DEBITO:
            case RequestType.TRANSACAO_CREDITO:
            case RequestType.TRANSACAO_VOUCHER:
                console.log('Transaction Result:', response.result);

                // Example: Automatically confirm the transaction
                this.sendRequest(RequestType.CONFIRMA, { numeroControle: response.result.numeroControle });
                break;
            case RequestType.CONFIRMA:
                console.log('Confirm Result:', response.result);
                break;
            case RequestType.CONSULTA_PDV:
                console.log('PDV Search Result:', response.result);
                break;
            case RequestType.DISPLAY:
                console.log('Display Message:', response.result);
                break;
            case RequestType.ERROR:
                console.error('Error:', response.result);
                break;
            case RequestType.MESSAGE:
                console.log('Message:', response.result);
                break;
            case RequestType.BEEP:
                console.log('Beep!');
                break;
            default:
                console.warn('Unknown response type:', response.type);
        }
    }
}

// get env url
const url = process.env.WS_URL || 'ws://localhost:8080';

// Example usage
const client = new WebSocketClient(url);

// type the valor
const valor = process.env.VALOR || '1000';
const cupom = process.env.CUPOM || '12346';

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
