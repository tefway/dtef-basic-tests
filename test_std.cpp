#include "ClasseIntegracao.h"

void *CALLING_COV display_error(char *msg) {
    std::cout << __func__ << ": " << msg << std::endl;
    return msg;
}

void *CALLING_COV display_msg(char *msg) {
    std::cout << __func__ << ": " << msg << std::endl;
    return msg;
}

void *CALLING_COV display_terminal(char *msg) {
    std::cout << __func__ << ": " << msg << std::endl;
    return msg;
}

int main() {
    ClasseIntegracao integ;

    integ.carregaDll();

    char versao[255]{};
    integ.InicializaDPOS();
    integ.VersaoDPOS(versao);

    std::cout << versao << std::endl;

    integ.setCallBackDisplayErro(display_error);
    integ.setCallBackDisplayMensagem(display_msg);
    integ.setCallBackDisplayTerminal(display_terminal);

    char DadosPin[1024]{};
    integ.ProcuraPinPad(DadosPin);

    std::cout << "Dados PinPad: " << DadosPin << std::endl;

    std::cout << "Iniciar transação de débito" << std::endl;
    char valorTx[255] = "1000";
    char cupom[255] = "17";

    std::cout << "Valor da transação: ";
    std::cin >> valorTx;

    std::cout << "Cupom: ";
    std::cin >> cupom;

    char controle[255] = "000000";
    integ.TransacaoCartaoCredito(valorTx, cupom, controle);

    std::cout << "Controle: " << controle << std::endl;

    std::cout << "ConfirmaCartao " << integ.ConfirmaCartao(controle)
              << std::endl;

    integ.FinalizaDPOS();
}
