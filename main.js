const ClasseIntegracao = require('./integracao_dtef');

const integration = new ClasseIntegracao();

integration.carregaDll();

clients = [];

integration.setCallBackDisplayTerminal((message) => {
    console.log('Display Terminal:', message);
    toSend = {
        type: 'display',
        message: message
    };

    clients.forEach(client => client.send(JSON.stringify(toSend)));
});

integration.setCallBackDisplayErro((message) => {
    console.log('Display Error:', message);
    toSend = {
        type: 'error',
        message: message
    };

    clients.forEach(client => client.send(JSON.stringify(toSend)));
});

integration.setCallBackDisplayMensagem((message) => {
    console.log('Display Message:', message);
    toSend = {
        type: 'message',
        message: message
    };

    clients.forEach(client => client.send(JSON.stringify(toSend)));
});

integration.setCallBackBeep(() => {
    console.log('Beep!');
    toSend = {
        type: 'beep'
    };

    clients.forEach(client => client.send(JSON.stringify(toSend)));
});

integration.InicializaDPOS();

/*const pinpad = integration.ProcuraPinPad();
console.log('PinPad:', pinpad);

const valor = '1000';
const cupom = '20';
const resultado = integration.TransacaoCartaoDebito(valor, cupom);

console.log('Resultado:', resultado);

integration.FinalizaDPOS();*/


// Import the WebSocket package
const WebSocket = require('ws');

// Create a new WebSocket server
const wss = new WebSocket.Server({ port: 8080, host: '0.0.0.0' });

// Set up a connection event listener
wss.on('connection', ws => {
    console.log('New client connected!');

    // Send a welcome message to the client
    ws.send('Hello, Client!');
    clients.push(ws);

    // Listen for messages from the client
    ws.on('message', message => {
        console.log(`Received message => ${message}`);

        obj = JSON.parse(message);

        if (obj) {
            switch (obj.requestType) {
                case 'inicializa':
                    integration.InicializaDPOS();
                    break;

                case 'finaliza':
                    integration.FinalizaDPOS();
                    break;

                case 'versao':
                    const versao = integration.VersaoDPOS();
                    ws.send(versao);
                    break;

                case 'procura':
                    const pinpad = integration.ProcuraPinPad();
                    ws.send(JSON.stringify(pinpad));
                    break;

                case 'transacao':
                    const valor = obj.valor.toString();
                    const cupom = obj.cupom.toString();
                    const resultado = integration.TransacaoCartaoDebito(valor, cupom);
                    ws.send(JSON.stringify(resultado));
                    break;

                case 'confirma':
                    const numeroControle = obj.numeroControle.toString();

                    const resultadoConfirma = integration.ConfirmaCartao(numeroControle);
                    ws.send(JSON.stringify(resultadoConfirma));
                    break;
            }
        } else {
            ws.send('Invalid request');
        }
    });

    // Handle the connection close event
    ws.on('close', () => {
        console.log('Client has disconnected!');
        // Remove the client from the clients array
        clients = clients.filter(client => client !== ws);
    });
});

console.log('WebSocket server is running on ws://localhost:8080');

