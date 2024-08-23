const ClasseIntegracao = require('./integracao_dtef');
const WebSocket = require('ws');

// Define the RequestType enum
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
});

class WebSocketServer {
    constructor(port = 8080, host = '0.0.0.0') {
        this.clients = [];
        this.wss = new WebSocket.Server({ port, host });
        this.integration = new ClasseIntegracao();
        this.setupIntegration();
        this.setupWebSocketServer();
    }

    setupIntegration() {
        this.integration.carregaDll();

        this.integration.setCallBackDisplayTerminal(this.handleDisplayTerminal.bind(this));
        this.integration.setCallBackDisplayErro(this.handleDisplayError.bind(this));
        this.integration.setCallBackDisplayMensagem(this.handleDisplayMessage.bind(this));
        this.integration.setCallBackBeep(this.handleBeep.bind(this));

        const initResult = this.integration.InicializaDPOS();
        if (initResult !== 0) {
            console.error('Failed to initialize DPOS, result code:', initResult);
        }
    }

    setupWebSocketServer() {
        this.wss.on('connection', this.handleConnection.bind(this));
        console.log('WebSocket server is running on ws://localhost:8080');
    }

    handleConnection(ws) {
        console.log('New client connected!');
        this.clients.push(ws);

        ws.on('message', (message) => this.handleMessage(ws, message));
        ws.on('close', () => this.handleClose(ws));
    }

    handleMessage(ws, message) {
        console.log(`Received message => ${message}`);
        let obj;
        try {
            obj = JSON.parse(message);
        } catch (e) {
            ws.send(JSON.stringify({ error: 'Invalid JSON format' }));
            return;
        }

        if (obj && obj.requestType) {
            try {
                this.processRequest(ws, obj);
            } catch (e) {
                console.error('Error processing request:', e);
                ws.send(JSON.stringify({ error: 'Internal server error' }));
            }
        } else {
            ws.send(JSON.stringify({ error: 'Invalid request' }));
        }
    }

    processRequest(ws, obj) {
        const requestHandlers = {
            [RequestType.INICIALIZA]: () => this.integration.InicializaDPOS(),
            [RequestType.FINALIZA]: () => this.integration.FinalizaDPOS(),
            [RequestType.VERSAO]: () => this.integration.VersaoDPOS(),
            [RequestType.PROCURA]: () => this.integration.ProcuraPinPad(),
            [RequestType.TRANSACAO_DEBITO]: () => this.integration.TransacaoCartaoDebito(obj.valor.toString(), obj.cupom.toString()),
            [RequestType.TRANSACAO_CREDITO]: () => this.integration.TransacaoCartaoCredito(obj.valor.toString(), obj.cupom.toString()),
            [RequestType.TRANSACAO_VOUCHER]: () => this.integration.TransacaoCartaoVoucher(obj.valor.toString(), obj.cupom.toString()),
            [RequestType.CONFIRMA]: () => this.integration.ConfirmaCartao(obj.numeroControle.toString()),
            [RequestType.CONSULTA_PDV]: () => this.integration.ConsultaCadastroPDV(),
        };

        const handler = requestHandlers[obj.requestType];
        if (handler) {
            const result = handler();
            ws.send(JSON.stringify({ type: obj.requestType, result }));
        } else {
            ws.send(JSON.stringify({ error: 'Unknown request type' }));
        }
    }

    handleClose(ws) {
        console.log('Client has disconnected!');
        this.clients = this.clients.filter(client => client !== ws);
    }

    handleDisplayTerminal(message) {
        this.broadcast({ type: 'display', message });
    }

    handleDisplayError(message) {
        this.broadcast({ type: 'error', message });
    }

    handleDisplayMessage(message) {
        this.broadcast({ type: 'message', message });
    }

    handleBeep() {
        this.broadcast({ type: 'beep' });
    }

    broadcast(data) {
        this.clients.forEach(client => client.send(JSON.stringify(data)));
    }
}

new WebSocketServer();
