#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#define CALLING_COV __stdcall
#else
#define CALLING_COV
#endif

class ClasseIntegracao {
  private:
    bool semTelas{false};
    typedef void *(CALLING_COV *ptrCallBackDisplayTerminal)(char *pMensagem);
    typedef void *(CALLING_COV *ptrCallBackDisplayErro)(char *pMensagem);
    typedef void *(CALLING_COV *ptrCallBackMensagem)(char *pMensagem);
    typedef void *(CALLING_COV *ptrCallBackBeep)(void);
    typedef int *(CALLING_COV *ptrCallBackSolicitaConfirmacao)(char *pMensagem);
    typedef int *(CALLING_COV *ptrCallBackEntraCartao)(char *pLabel,
                                                       char *pCartao);
    typedef int *(CALLING_COV *ptrCallBackEntraDataValidade)(
        char *pLabel, char *pDataValidade);
    typedef int *(CALLING_COV *ptrCallBackEntraData)(char *pLabel,
                                                     char *pDataValidade);
    typedef int *(CALLING_COV *ptrCallBackEntraCodigoSeguranca)(
        char *pLabel, char *pEntraCodigoSeguranca, int iTamanhoMax);
    typedef int(CALLING_COV *ptrCallBackSelecionaOpcao)(char *pLabel,
                                                        char *pOpcoes,
                                                        int *iOpcaoSelecionada);
    typedef int *(CALLING_COV *ptrCallBackEntraValor)(char *pLabel,
                                                      char *pValor,
                                                      char *pValorMinimo,
                                                      char *pValorMaximo);
    typedef int *(CALLING_COV *ptrCallBackEntraValorEspecial)(
        char *pLabel, char *pValor, char *pParametros);
    typedef int *(CALLING_COV *ptrCallBackEntraNumero)(
        char *pLabel, char *pNumero, char *pNumeroMinimo, char *pNumeroMaximo,
        int iMininoDigitos, int iMaximoDigitos, int iDigitosExatos);
    typedef int *(CALLING_COV *ptrCallBackOperacaoCancelada)(void);
    typedef int *(CALLING_COV *ptrCallBackSetaOperacaoCancelada)(
        bool bCancelada);
    typedef void *(CALLING_COV *ptrCallBackProcessaMensagens)(void);
    typedef int *(CALLING_COV *ptrCallBackEntraString)(char *pLabel,
                                                       char *pString,
                                                       char *pTamanhoMaximo);
    typedef int *(CALLING_COV *ptrCallBackConsultaAVS)(
        char *cEndereco, char *cNumero, char *cApto, char *cBloco, char *cCEP,
        char *cBairro, char *cCPF);
    typedef int *(CALLING_COV *ptrCallBackMensagemAdicional)(
        char *pMensagemAdicional);
    typedef int *(CALLING_COV *ptrCallBackImagemAdicional)(int *pIndiceImagem);
    typedef int *(CALLING_COV *ptrCallBackEntraCodigoBarras)(char *Label,
                                                             char *Campo);
    typedef int *(CALLING_COV *ptrCallBackEntraCodigoBarrasLido)(char *Label,
                                                                 char *Campo);
    typedef void *(CALLING_COV *ptrCallBackMensagemAlerta)(
        char *pMensagemAlerta);
    typedef void *(CALLING_COV *ptrCallBackPreviewComprovante)(
        char *cComprovante);
    typedef int *(CALLING_COV *ptrCallBackSelecionaPlanos)(
        int iCodigoRede, int iCodigoTransacao, int iTipoFinanciamento,
        int iMaximoParcelas, char *pValorMinimoParcela, int iMaxDiasPreDatado,
        char *pNumeroParcelas, char *pValorTransacao, char *pValorParcela,
        char *pValorEntrada, char *pData);
    typedef int *(CALLING_COV *ptrCallBackSelecionaPlanosEx)(char *sSolicitacao,
                                                             char *sRetorno);

    typedef void(CALLING_COV *pCallBackDisplayTerminal)(char *pMensagem);
    typedef void(CALLING_COV *pCallBackDisplayErro)(char *pMensagem);
    typedef void(CALLING_COV *pCallBackMensagem)(char *pMensagem);
    typedef void(CALLING_COV *pCallBackBeep)(void);
    typedef int(CALLING_COV *pCallBackSolicitaConfirmacao)(char *pMensagem);
    typedef int(CALLING_COV *pCallBackEntraCartao)(char *pLabel, char *pCartao);
    typedef int(CALLING_COV *pCallBackEntraDataValidade)(char *pLabel,
                                                         char *pDataValidade);
    typedef int(CALLING_COV *pCallBackEntraData)(char *pLabel,
                                                 char *pDataValidade);
    typedef int(CALLING_COV *pCallBackEntraCodigoSeguranca)(
        char *pLabel, char *pEntraCodigoSeguranca, int iTamanhoMax);
    typedef int(CALLING_COV *pCallBackSelecionaOpcao)(char *pLabel,
                                                      char *pOpcoes,
                                                      int *iOpcaoSelecionada);
    typedef int(CALLING_COV *pCallBackEntraValor)(char *pLabel, char *pValor,
                                                  char *pValorMinimo,
                                                  char *pValorMaximo);
    typedef int(CALLING_COV *pCallBackEntraValorEspecial)(char *pLabel,
                                                          char *pValor,
                                                          char *pParametros);
    typedef int(CALLING_COV *pCallBackEntraNumero)(
        char *pLabel, char *pNumero, char *pNumeroMinimo, char *pNumeroMaximo,
        int iMininoDigitos, int iMaximoDigitos, int iDigitosExatos);
    typedef int(CALLING_COV *pCallBackOperacaoCancelada)(void);
    typedef int(CALLING_COV *pCallBackSetaOperacaoCancelada)(bool bCancelada);
    typedef void(CALLING_COV *pCallBackProcessaMensagens)(void);
    typedef int(CALLING_COV *pCallBackEntraString)(char *pLabel, char *pString,
                                                   char *pTamanhoMaximo);
    typedef int(CALLING_COV *pCallBackConsultaAVS)(char *cEndereco,
                                                   char *cNumero, char *cApto,
                                                   char *cBloco, char *cCEP,
                                                   char *cBairro, char *cCPF);
    typedef int(CALLING_COV *pCallBackMensagemAdicional)(
        char *pMensagemAdicional);
    typedef int(CALLING_COV *pCallBackImagemAdicional)(int *iIndiceImagem);
    typedef int(CALLING_COV *pCallBackEntraCodigoBarras)(char *Label,
                                                         char *Campo);
    typedef int(CALLING_COV *pCallBackEntraCodigoBarrasLido)(char *Label,
                                                             char *Campo);
    typedef void(CALLING_COV *pCallBackMensagemAlerta)(char *pMensagemAlerta);
    typedef void(CALLING_COV *pCallBackPreviewComprovante)(char *cComprovante);
    typedef int(CALLING_COV *pCallBackSelecionaPlanos)(
        int iCodigoRede, int iCodigoTransacao, int iTipoFinanciamento,
        int iMaximoParcelas, char *pValorMinimoParcela, int iMaxDiasPreDatado,
        char *pNumeroParcelas, char *pValorTransacao, char *pValorParcela,
        char *pValorEntrada, char *pData);
    typedef int(CALLING_COV *pCallBackSelecionaPlanosEx)(char *sSolicitacao,
                                                         char *sRetorno);

    typedef void(CALLING_COV *RegPDVDisplayTerminal)(pCallBackDisplayTerminal);
    typedef void(CALLING_COV *RegPDVDisplayErro)(pCallBackDisplayErro);
    typedef void(CALLING_COV *RegPDVMensagem)(pCallBackMensagem);
    typedef void(CALLING_COV *RegPDVBeep)(pCallBackBeep);
    typedef void(CALLING_COV *RegPDVSolicitaConfirmacao)(
        pCallBackSolicitaConfirmacao);
    typedef void(CALLING_COV *RegPDVEntraCartao)(pCallBackEntraCartao);
    typedef void(CALLING_COV *RegPDVEntraDataValidade)(
        pCallBackEntraDataValidade);
    typedef void(CALLING_COV *RegPDVEntraData)(pCallBackEntraData);
    typedef void(CALLING_COV *RegPDVEntraCodigoSeguranca)(
        pCallBackEntraCodigoSeguranca);
    typedef void(CALLING_COV *RegPDVSelecionaOpcao)(pCallBackSelecionaOpcao);
    typedef void(CALLING_COV *RegPDVEntraValor)(pCallBackEntraValor);
    typedef void(CALLING_COV *RegPDVEntraValorEspecial)(
        pCallBackEntraValorEspecial);
    typedef void(CALLING_COV *RegPDVEntraNumero)(pCallBackEntraNumero);
    typedef void(CALLING_COV *RegPDVOperacaoCancelada)(
        pCallBackOperacaoCancelada);
    typedef void(CALLING_COV *RegPDVSetaOperacaoCancelada)(
        pCallBackSetaOperacaoCancelada);
    typedef void(CALLING_COV *RegPDVProcessaMensagens)(
        pCallBackProcessaMensagens);
    typedef void(CALLING_COV *RegPDVEntraString)(pCallBackEntraString);
    typedef void(CALLING_COV *RegPDVConsultaAVS)(pCallBackConsultaAVS);
    typedef void(CALLING_COV *RegPDVImagemAdicional)(pCallBackImagemAdicional);
    typedef void(CALLING_COV *RegPDVMensagemAdicional)(
        pCallBackMensagemAdicional);
    typedef void(CALLING_COV *RegPDVEntraCodigoBarras)(
        pCallBackEntraCodigoBarras);
    typedef void(CALLING_COV *RegPDVEntraCodigoBarrasLido)(
        pCallBackEntraCodigoBarrasLido);
    typedef void(CALLING_COV *RegPDVMensagemAlerta)(pCallBackMensagemAlerta);
    typedef void(CALLING_COV *RegPDVPreviewComprovante)(
        pCallBackPreviewComprovante);
    typedef void(CALLING_COV *RegPDVSelecionaPlanos)(pCallBackSelecionaPlanos);
    typedef void(CALLING_COV *RegPDVSelecionaPlanosEx)(
        pCallBackSelecionaPlanosEx);

    typedef int(CALLING_COV *FTransacaoCartaoCredito)(char *valor,
                                                      char *numeroCupom,
                                                      char *numeroControle);
    typedef int(CALLING_COV *FTransacaoCartaoDebito)(char *valor,
                                                     char *numeroCupom,
                                                     char *numeroControle);
    typedef void(CALLING_COV *FVersaoDPOS)(char *pVersao);
    typedef int(CALLING_COV *FTransacaoCartaoFrota)(char *pValorTransacao,
                                                    char *pNumeroCupom,
                                                    char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaCartaoFrota)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCartaoFrotaCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoProduto, char *pCodigoVeiculo, char *pCodigoCondutor,
        char *pKilometragem, char *pDadosServicos, char *pPermiteAlteracao,
        char *pReservado);
    typedef int(CALLING_COV *FImprimeCupomTEF)(char *pPathArquivoCupomTEF,
                                               char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoCartaoParceleMais)(char *pValorTransacao,
                                                          char *pNumeroCupom,
                                                          char *pNumeroControle,
                                                          char *pCodigoTabela);
    typedef int(CALLING_COV *FTransacaoCartaoParceleMaisCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pCodigoTabela, char *pNumeroParcelas, char *pValorParcela,
        char *pVencimentoPrimeiraParcela, char *pPermiteAlteracao);
    typedef int(CALLING_COV *FTransacaoCartaoVoucher)(char *pValorTransacao,
                                                      char *pNumeroCupom,
                                                      char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCheque)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pQuantidadeCheques, char *pPeriodicidadeCheques,
        char *pDataPrimeiroCheque, char *pCarenciaPrimeiroCheque);
    typedef int(CALLING_COV *FTransacaoCelular)(char *pValorTransacao,
                                                char *pNumeroCupom,
                                                char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCancelamentoCelular)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCancelamentoGarantiaCheque)(
        char *pValorTransacao, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCancelamentoPagamento)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FEstornoPreAutorizacaoRedecard)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConfirmacaoPreAutorizacao)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCancelamentoPagamentoCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FConfirmaCartaoCredito)(char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaCartaoDebito)(char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaCartaoVoucher)(char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaDebitoBeneficiario)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaCartao)(char *pNumeroControle);
    typedef int(CALLING_COV *FDesfazCartao)(char *pNumeroControle);
    typedef int(CALLING_COV *FFinalizaTransacao)();
    typedef int(CALLING_COV *FTransacaoConsultaParcelas)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaParcelasCredito)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaParcelasCelular)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPreAutorizacao)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPreAutorizacaoCartaoCredito)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPreAutorizacaoCartaoFrota)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoFuncaoEspecial)();
    typedef int(CALLING_COV *FTransacaoResumoVendas)(char *pNumeroControle);
    typedef void(CALLING_COV *FDadosUltimaTransacao)(
        char *pDadosUltimaTransacao);
    typedef void(CALLING_COV *FObtemCodigoRetorno)(int iCodigoRetorno,
                                                   char *pCodigoRetorno);
    typedef void(CALLING_COV *FObtemLogUltimaTransacao)(
        char *oLogUltimaTransacao);
    typedef void(CALLING_COV *FObtemLogUltimaTransacaoTeleMarketing)(
        char *pNumeroPDV, char *oLogUltimaTransacao);
    typedef void(CALLING_COV *FObtemLogUltimaTransacaoTeleMarketingMultiLoja)(
        char *cNumeroEmpresa, char *cNumeroLoja, char *pNumeroPDV,
        char *oLogUltimaTransacao);
    typedef void(CALLING_COV *FObtemComprovanteTransacao)(
        char *pNumeroControle, char *pComprovante, char *pComprovanteReduzido);
    typedef void(CALLING_COV *FDadosUltimaTransacaoDiscada)(
        char *oDadosUltimaTransacaoDiscada);
    typedef void(CALLING_COV *FDadosUltimaTransacaoCB)(char *pLogTitulo);
    typedef int(CALLING_COV *FTransacaoCartaoCreditoCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorParcela,
        char *pValorTaxaServico, char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCartaoDebitoCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pNumeroParcelas, char *pSequenciaParcela,
        char *pDataDebito, char *pValorParcela, char *pValorTaxaServico,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCartaoVoucherCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pReservado);
    typedef int(CALLING_COV *FTransacaoChequeCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pQuantidadeCheques, char *pPeriodicidadeCheques,
        char *pDataPrimeiroCheque, char *pCarenciaPrimeiroCheque,
        char *pTipoDocumento, char *pNumeroDocumento,
        char *pQuantidadeChequesEx, char *pSequenciaCheque,
        char *pCamaraCompensacao, char *pNumeroBanco, char *pNumeroAgencia,
        char *pNumeroContaCorrente, char *pNumeroCheque, char *pDataDeposito,
        char *pValorCheque, char *pDataNascimentoCliente,
        char *pTelefoneCliente, char *pCMC7, char *pPermiteAlteracao,
        char *pReservado);
    typedef int(CALLING_COV *FTransacaoCartaoPrivateLabelCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorEntrada,
        char *pValorTaxaServico, char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoSimulacaoPrivateLabel)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaPrivateLabel)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPagamentoPrivateLabel)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoDebitoBeneficiario)(
        char *pValorTransacao, char *pTipoBeneficio, char *pNumeroBeneficio,
        char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FInfAdicionaisCartaoDebito)(char *pDadosParcelas);
    typedef int(CALLING_COV *FObtemInfTransacaoDebitoParcelado)(
        char *pDadosParcelas);
    typedef int(CALLING_COV *FConsultaParametrosCB)(char *pParametros);
    typedef int(CALLING_COV *FConsultaPagamentoCB)(
        char *pTipoConta, char *pCodigoBarrasDigitado, char *pCodigoBarrasLido,
        char *pValorTitulo, char *pValorDesconto, char *pValorAcrescimo,
        char *pDataVencimento, char *pFormaPagamento, char *pTrilhaCartao,
        char *pTipoSenha, char *pSenha, char *pNSUCartao, char *pTipoCMC7,
        char *pCMC7, char *pNumeroControle, char *pMensagemTEF,
        char *pImprimeComprovante, char *pParametros, char *pParametros2);
    typedef int(CALLING_COV *FTransacaoPagamentoCB)(
        char *pTipoConta, char *pCodigoBarrasDigitado, char *pCodigoBarrasLido,
        char *pValorTitulo, char *pValorDesconto, char *pValorAcrescimo,
        char *pDataVencimento, char *pFormaPagamento, char *pTrilhaCartao,
        char *pTipoSenha, char *pSenha, char *pNSUCartao, char *pTipoCMC7,
        char *pCMC7, char *pParametros2, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCancelamentoCB)(
        char *pTipoConta, char *pCodigoBarrasDigitado, char *pCodigoBarrasLido,
        char *pValorTitulo, char *pNSUCartao, char *pNumeroControle);
    typedef int(CALLING_COV *FInicializaSessaoCB)(char *pNumeroControle);
    typedef int(CALLING_COV *FFinalizaSessaoCB)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoResumoRecebimentosCB)(
        char *pTipoRecebimento, char *pNumeroControle);
    typedef int(CALLING_COV *FUltimaMensagemTEF)(char *pMensagem);
    typedef int(CALLING_COV *FTransacaoReimpressaoCupom)();
    typedef int(CALLING_COV *FConfiguraDPOS)();
    typedef int(CALLING_COV *FInicializaDPOS)();
    typedef int(CALLING_COV *FFinalizaDPOS)();
    typedef int(CALLING_COV *FTransacaoFechamento)(
        char *pDataMovimento, char *pQuantidadeVendas, char *pValorTotalVendas,
        char *pQuantidadeVendasCanceladas, char *pValorTotalVendasCanceladas,
        char *pReservado, char *pNumeroControle, char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoFechamentoPOS)(
        char *pTipoRelatorio, char *pParametrosRelatorio, char *pReservado,
        char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoAtivacaoPOS)(char *pCNPJ,
                                                    char *pNumeroSerie,
                                                    char *pCodAtivacao,
                                                    char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoInicializacaoAreaPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoLojaPOS)(char *pCNPJ, char *pNumeroSerie,
                                                char *pCodigoEmpresa,
                                                char *pCodigoLoja,
                                                char *pCodigoPDV,
                                                char *pNumeroControle);
    typedef void(CALLING_COV *FObtemUltimoErro)(char *pErro);
    typedef void(CALLING_COV *FDadosUltimaTransacaoNaoAprovada)(
        char *pDadosTransacaoNaoAprovada);
    typedef int(CALLING_COV *FTransacaoColeta)(int nTipoCartao,
                                               char *pValorTransacao,
                                               char *pTrilha1, char *pTrilha2,
                                               char *pTrilha3,
                                               char *pParametros);
    typedef int(CALLING_COV *FTransacaoColetaCartao)(int nTipoCartao,
                                                     char *pTrilha1,
                                                     char *pTrilha2,
                                                     char *pTrilha3);
    typedef int(CALLING_COV *FTransacaoColetaSenha)(char *pValor, char *pSenha);
    typedef int(CALLING_COV *FTransacaoColetaSenhaCartao)(int nTipoCartao,
                                                          char *pValor,
                                                          char *pTrilha2,
                                                          char *pReservado,
                                                          char *pSenha);
    typedef int(CALLING_COV *FColetaPlanoPagamento)(
        char *pDescricaoFormaPagamento, char *pValorTransacao,
        char *pNumeroCupom, char *pTipoFormaPagamento, char *pNumeroControle,
        char *pNumeroParcelas, char *pValorAcrescimo, char *pValorEntrada,
        char *pValorTotal, char *pCodigoPlano, char *pPlano, char *pParcelas);
    typedef int(CALLING_COV *FTransacaoAdministrativaTEFDiscado)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FSelecionaTEF)();
    typedef int(CALLING_COV *FExportaPlanos)();
    typedef int(CALLING_COV *FTransacaoQuantidade)(char *pValorTransacao,
                                                   char *pValorTransacaoMaximo,
                                                   char *pNumeroCupom);
    typedef int(CALLING_COV *FTransacaoValor)(char *pValorTransacao,
                                              char *pValorTransacaoMaximo,
                                              char *pNumeroCupom);
    typedef int(CALLING_COV *FTransacaoCartaoCreditoConfirmada)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartao, char *pDataVencimento, char *pCVV2,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorTaxaServico,
        char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCancelamentoConfirmada)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroControle, char *pNumeroCartao,
        char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FPreAutorizacaoCreditoConfirmada)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartao, char *pDataVencimento, char *pCVV2,
        char *pValorTaxaServico, char *pNumeroAutorizacao, char *pMensagemTEF,
        char *pReservado);
    typedef int(CALLING_COV *FConfPreAutorizacaoCreditoConfirmada)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pDataPreAutor, char *pNumeroCartao, char *pDataVencimento,
        char *pCVV2, char *pTipoOperacao, char *pNumeroParcelas,
        char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FTransacaoManualPOS)(char *pValorTransacao);
    typedef int(CALLING_COV *FTransacaoManualPOSCompleta)(
        char *pValorTransacao, char *pCodigoEstabelecimento, char *pData,
        char *pHora, char *pNumeroAutorizadora, char *pNumeroCartao,
        char *pTipoOperacao, char *pNumeroParcelas, char *pDataPreDatado,
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCartaoFidelidade)(
        char *pValorTransacao, char *pNumeroCupom, char *pCompraPontos,
        char *pQuantidadeItensTaxaVariavel, char *pItensTaxaVariavel,
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoDebitoFidelidade)(char *pNumeroCupom,
                                                         char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaCartaoFidelidade)(char *pNumeroControle);
    typedef int(CALLING_COV *FExtratoCartaoAutorizadorDirecao)(
        char *pNumeroCartao, char *pNumeroControle);
    typedef int(CALLING_COV *FRecebimentoAutorizadorDirecao)(
        char *pValorTransacao, char *pNumeroCartao, char *pNumeroControle);
    typedef int(CALLING_COV *FEstornoRecebimentoAutorizadorDirecao)(
        char *pValorTransacao, char *pNumeroCartao, char *pNumeroControle);
    typedef int(CALLING_COV *FRecebimentoCarneAutorizadorDirecao)(
        char *pValorTransacao, char *pNumeroCartao, char *pDataVencimento,
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPagamentoConvenio)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FConfirmaConvenio)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoSaqueRedecard)(char *pValorTransacao,
                                                      char *pNumeroCupomFiscal,
                                                      char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoSaque)(char *pValorTransacao,
                                              char *pNumeroCupomFiscal,
                                              char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCDC1MIN)(char *pValorTransacao,
                                                char *pNumeroCupomFiscal,
                                                char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoSIMparcelado)(char *pValorTransacao,
                                                     char *pNumeroCupomFiscal,
                                                     char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCartaoCreditoIATA)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCartaoCreditoIATAConfirmada)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartao, char *pDataVencimento, char *pCVV2,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorTaxaServico,
        char *pValorEntrada, char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCartaoPrivateLabel)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef void(CALLING_COV *FBeepErro)();
    typedef int(CALLING_COV *FTransacaoConsultaSaldo)(char *pNumeroCupom,
                                                      char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaSaldoCelular)(
        char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaSaldoCompleta)(
        char *pNumeroCupom, char *pNumeroControle, char *pTipoCartao,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FColetaCartaoDebito)(char *pValorTransacao,
                                                  char *pNumeroCupom,
                                                  char *pParametrosTEF,
                                                  char *pMensagemOperador);
    typedef int(CALLING_COV *FConsisteDadosCartaoDebito)(
        char *pUltimosDigitos, char *pCodigoSeguranca, char *pMensagemOperador);
    typedef int(CALLING_COV *FColetaSenhaCartaoDebito)(char *pNumeroControle,
                                                       char *pMensagemOperador);
    typedef int(CALLING_COV *FConsultaParametrosPBM)(char *pNumeroCupom,
                                                     char *pQuantidadeRedes,
                                                     char *pDadosRedes,
                                                     char *pMensagemOperador);
    typedef int(CALLING_COV *FConsultaProdutosPBM)(
        char *pNumeroCupom, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pAutorizacao, char *pDadosCRM, char *pTipoVenda,
        char *pValorTotalMedicamentos, char *pValorVista, char *pValorCartao,
        char *pCodigoCredenciado, char *pQuantidadeMedicamentos,
        char *pMedicamentos, char *pReservado, char *pNumeroControle,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoVendaPBM)(
        char *pValorTotalMedicamentos, char *pNumeroCupom, char *pRedePBM,
        char *pTrilha1, char *pTrilha2, char *pDigitado, char *pAutorizacao,
        char *pNSUConsulta, char *pDadosCRM, char *pTipoVenda,
        char *pValorVista, char *pValorCartao, char *pCodigoCredenciado,
        char *pRegraNegocio, char *pQuantidadeMedicamentos, char *pMedicamentos,
        char *pReservado, char *pNumeroControle, char *pNumeroControleRede,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FConfirmaVendaPBM)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoReimpressaoVendaPBM)();
    typedef int(CALLING_COV *FTransacaoCancelamentoVendaPBM)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCancelamentoPreAutorizacaoPBM)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoElegibilidadePBM)(
        char *pNumeroCupom, char *pRedePBM, char *pMatricula,
        char *pDadosElegibilidade, char *pNumeroControle,
        char *pNumeroControleRede, char *pNomeCliente, char *pNomeMedico,
        char *pInformaDependente, char *pListaDependentes, char *pReservado,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoPreAutorizacaoPBM)(
        char *pNumeroCupom, char *pRedePBM, char *pAutorizacaoElegibilidade,
        char *pDadosFarmaceutico, char *pQuantidadeMedicamentos,
        char *pMedicamentos, char *pNumeroControle, char *pNumeroControleRede,
        char *pReservado, char *pMensagemOperador);
    typedef int(
        CALLING_COV *FTransacaoCancelamentoPreAutorizacaoPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pMatricula, char *pReservado,
        char *pNumeroControle, char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoElegibilidadePBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pRedePBM, char *pMatricula,
        char *pDadosElegibilidade, char *pNumeroControle,
        char *pNumeroControleRede, char *pNomeCliente, char *pNomeMedico,
        char *pInformaDependente, char *pListaDependentes, char *pReservado,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoPreAutorizacaoPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pRedePBM,
        char *pAutorizacaoElegibilidade, char *pDadosFarmaceutico,
        char *pQuantidadeMedicamentos, char *pMedicamentos,
        char *pNumeroControle, char *pNumeroControleRede, char *pReservado,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FConsultaParametrosPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pQuantidadeRedes,
        char *pDadosRedes, char *pMensagemOperador);
    typedef int(CALLING_COV *FConsultaProdutosPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pRedePBM, char *pTrilha1,
        char *pTrilha2, char *pDigitado, char *pAutorizacao, char *pDadosCRM,
        char *pTipoVenda, char *pValorTotalMedicamentos, char *pValorVista,
        char *pValorCartao, char *pCodigoCredenciado,
        char *pQuantidadeMedicamentos, char *pMedicamentos, char *pReservado,
        char *pNumeroControle, char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoVendaPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pValorTotalMedicamentos, char *pNumeroCupom,
        char *pRedePBM, char *pTrilha1, char *pTrilha2, char *pDigitado,
        char *pAutorizacao, char *pNSUConsulta, char *pDadosCRM,
        char *pTipoVenda, char *pValorVista, char *pValorCartao,
        char *pCodigoCredenciado, char *pRegraNegocio,
        char *pQuantidadeMedicamentos, char *pMedicamentos, char *pReservado,
        char *pNumeroControle, char *pNumeroControleRede,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoCancelamentoVendaPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pTrilha1, char *pTrilha2, char *pDigitado,
        char *pReservado, char *pNumeroControle, char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoAberturaVendaPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pReservado, char *pPedirDadosComplementares,
        char *pDadosComplementares, char *pInfoDadosComplementares,
        char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoVendaProdutoPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pMedicamento, char *pReservado,
        char *pPedirDadosComplementares, char *pDadosComplementares,
        char *pInfoDadosComplementares, char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoFechamentoVendaPBM_TeleMarketing)(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pConfirmacao, char *pReservado,
        char *pAutorizacao, char *pNumeroControle, char *pMensagemOperador);
    typedef void(CALLING_COV *FInformaDadosMedicamentos)(
        char *pIndicativoReceita, char *pRegistroProfissional,
        char *pListaMedicamentos);
    typedef int(CALLING_COV *FTransacaoSaqueSaldo)(char *pValorTransacao,
                                                   char *pNumeroCupom,
                                                   char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoSaqueExtrato)(char *pValorTransacao,
                                                     char *pNumeroCupom,
                                                     char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaAVS)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaAVSConfirmada)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pNumeroCupom, char *pNumeroControle, char *pNumeroCartao,
        char *pDataVencimento, char *pCVV2, char *pEndereco, char *pNumero,
        char *pApto, char *pBloco, char *pCEP, char *pBairro, char *pCPF,
        char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FTransacaoPagamentoContasVisanet)(
        char *pTipoConta, char *pCodigoBarras, char *pFormaPagamento,
        char *pTrilhaCartao, char *pValorTransacao, char *pNumeroCupom,
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoEspecial)(int iCodigoTransacao,
                                                 char *pDados);
    typedef int(CALLING_COV *FConsultaValoresPrePago)(char *pNumeroControle,
                                                      char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoRecargaCelular)(char *pCodigoArea,
                                                       char *pNumeroTelefone,
                                                       char *pNumeroControle);
    typedef int(CALLING_COV *FConsultaParametrosTeledata)(
        char *pDadosTeledata, char *pMensagemOperador);
    typedef int(CALLING_COV *FConsultaChequesGarantidos)(
        char *pTipoDocumento, char *pNumeroDocumento,
        char *pDataInicialConsulta, char *pDataFinalConsulta,
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoSaqueCompleta)(
        char *pValorTransacao, char *pNumeroCupomFiscal, char *pNumeroControle,
        char *pTipoTransacao, char *pPlanoPagamento, char *pNumeroParcelas,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCartaoCompraSaque)(
        char *pValorCompra, char *pValorSaque, char *pNumeroCupom,
        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCartaoCompraSaqueCompleta)(
        char *pValorCompra, char *pValorSaque, char *pNumeroCupom,
        char *pNumeroControle, char *pPermiteAlteracao);
    typedef int(CALLING_COV *FTesteRedeAtiva)(
        char *pNumeroEmpresa, char *pNumeroLoja, char *pNumeroPDV,
        char *pCodigoRede, char *pStatusRede, char *pMensagemOperador);
    typedef int(CALLING_COV *FTransacaoCadastraSenha)(char *pNumeroControle);
    typedef void(CALLING_COV *FConfiguraFluxoExternoDTEF5)();
    typedef int(CALLING_COV *FTrataDesfazimento)(int iTratar);
    typedef int(CALLING_COV *FConsultaTransacao)(
        char *pNumeroEmpresa, char *pNumeroLoja, char *pNumeroPDV,
        char *pSolicitacao, char *pResposta, char *pMensagemErro);
    typedef int(CALLING_COV *FTransacaoeValeGas)(char *pValorTransacao,
                                                 char *pNumeroCupom,
                                                 char *pNumeroControle,
                                                 char *pNumeroValeGas);
    typedef int(CALLING_COV *FTransacaoConsultaeValeGas)(char *pValorTransacao,
                                                         char *pNumeroCupom,
                                                         char *pNumeroControle,
                                                         char *pNumeroValeGas,
                                                         char *pDadosRetorno);
    typedef int(CALLING_COV *FTransacaoCancelamentoPadrao)(
        char *pNumeroControle);
    typedef int(CALLING_COV *FConfiguraModoTeleMarketing)(int iModo);
    typedef int(CALLING_COV *FConfirmaCartaoTeleMarketing)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pNumeroControle, char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FDesfazCartaoTeleMarketing)(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pNumeroControle, char *pMensagemTEF, char *pReservado);
    typedef int(CALLING_COV *FTransacaoEstatistica)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoInjecaoChaves)(char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoTrocoSurpresa)(char *pValorTransacao,
                                                      char *pNumeroCupom,
                                                      char *pNumeroControle,
                                                      char *pMensagemTEF,
                                                      char *pReservado);
    typedef int(CALLING_COV *FIdentificacaoAutomacaoComercial)(
        char *pNomeAutomacao, char *pVersaoAutomacao, char *pReservado);
    typedef int(CALLING_COV *FConsultaTabelasValeGas)(char *pNumeroControle,
                                                      char *pMensagemOperador);
    typedef int(CALLING_COV *FDefineBandeiraTransacao)(char *pCodigoBandeira);
    typedef int(CALLING_COV *FDefineRedeTransacao)(char *pCodigoRede);
    typedef int(CALLING_COV *FConverteCodigoRede)(char *pCodigoRede,
                                                  char *pCodigoRedeDTEF5,
                                                  char *pCodigoRedeDTEF8);
    typedef int(CALLING_COV *FTransacaoAbreLote)(char *pNumeroCupom,
                                                 char *pNumeroControle,
                                                 char *pMensagemTEF,
                                                 char *pReservado);
    typedef int(CALLING_COV *FTransacaoFechaLote)(char *pNumeroCupom,
                                                  char *pNumeroControle,
                                                  char *pMensagemTEF,
                                                  char *pReservado);
    typedef void(CALLING_COV *FConfiguraEmpresaLojaPDV)(char *pNumeroEmpresa,
                                                        char *pNumeroLoja,
                                                        char *pNumeroPDV);
    typedef int(CALLING_COV *FTransacaoResgatePremio)(char *pNumeroCupom,
                                                      char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoVendaPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pCodigoFrentista, char *pCPFCNPJ,
        char *pPagamentoTEF, char *pCartaoProprio, char *pValor,
        char *pTelefoneCliente, char *pCodigoOrigem, char *pSaldoPontos,
        char *pCodigoPesquisa, char *pReservado, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoEstornoVendaPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pCodigoFrentista, char *pCPFCNPJ,
        char *pDataEstorno, char *pNSUEstorno, char *pAutorizacaoEstorno,
        char *pSaldoPontos, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoResgatePontosPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pCodigoFrentista, char *pCPFCNPJ,
        char *pQuantidadeProdutos, char *pListaProdutos, char *pSaldoPontos,
        char *pReservado, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaPontosPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pCodigoFrentista, char *pCPFCNPJ,
        char *pSaldoPontos, char *pReservado, char *pNumeroControle);
    typedef int(CALLING_COV *FAtualizacaoFrentistasPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pNumeroFrentistas,
        char *pListaFrentistas, char *pReservado, char *pNumeroControle);
    typedef int(CALLING_COV *FBuscaTabelaDTEF)(int iTipoTabela,
                                               char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoContratacao)(char *pNumeroCupom,
                                                    char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPrimeiraCompra)(char *pValorTransacao,
                                                       char *pNumeroCupom,
                                                       char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoCadastro)(
        char *pCNPJ, char *pNumeroSerie, char *pCodigoFrentista, char *pCPFCNPJ,
        char *pTelefoneCliente, char *pReservado, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoDesativacaoPOS)(
        char *pCNPJ, char *pNumeroSerie, char *pNumeroSerieDesativacao,
        char *pFrentista, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaPOSAtivos)(
        char *pCNPJ, char *pNumeroSerie, char *pNumeroPOSAtivos,
        char *pListaPOSAtivos, char *pNumeroControle);
    typedef int(CALLING_COV *FProcuraPinPad)(char *pDados);
    typedef int(CALLING_COV *FApagaTabelasPinPad)(char *pDados);
    typedef int(CALLING_COV *FTransacaoFuncoesAdministrativas)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pReservado);
    typedef int(CALLING_COV *FTransacaoQuitacaoCartaFrete)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoQuitacaoCartaFreteCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartaFrete, char *pQuantidadeChegada,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCartaoCrediario)(char *pValorTransacao,
                                                        char *pNumeroCupom,
                                                        char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoSimulacaoCrediario)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoPagamentoPrivateLabelCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pIndicadorDigitacao, char *pCodigoBarras, char *pNumeroCartao,
        char *pCodigoRede, char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoCargaCartao)(char *pValorTransacao,
                                                    char *pNumeroCupom,
                                                    char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoRecargaCartao)(char *pValorTransacao,
                                                      char *pNumeroCupom,
                                                      char *pNumeroControle);
    typedef int(CALLING_COV *FCadastraPDV)();
    typedef int(CALLING_COV *FCadastraPDVTelemarketing)(char *pNumeroEmpresa,
                                                        char *pNumeroLoja,
                                                        char *pNumeroPDV,
                                                        char *pMensagemTEF);
    typedef int(CALLING_COV *FTransacaoCartaoCrediarioCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroParcelas, char *pDataParcela1, char *pValorParcela1,
        char *pValorEntrada, char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoConsultaPlanos)(char *pValorTransacao,
                                                       char *pNumeroControle,
                                                       char *pReservado,
                                                       char *pPlanos);
    typedef int(CALLING_COV *FConsultaCadastroPDV)(char *pMensagem);
    typedef int(CALLING_COV *FConsultaCadastroPDVTelemarketing)(
        char *pNumeroEmpresa, char *pNumeroLoja, char *pNumeroPDV,
        char *pMensagemTEF);
    typedef int(CALLING_COV *FTransacaoSimulacaoSaque)(char *pValorTransacao,
                                                       char *pNumeroCupom,
                                                       char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoBloqueioCartao)(char *pNumeroCupom,
                                                       char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoDesbloqueioCartao)(
        char *pNumeroCupom, char *pNumeroControle);
    typedef int(CALLING_COV *FTransacaoConsultaCadastroCPFCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pDocEmissor, char *pPermiteAlteracao,
        char *pReservado);
    typedef int(CALLING_COV *FTransacaoDepositoCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pDocEmissor, char *pDocDestinatario,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoSaqueDomesticoCompleta)(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pCodigoPIN, char *pDocDestinatario,
        char *pPermiteAlteracao, char *pReservado);
    typedef int(CALLING_COV *FTransacaoAlteraSenhaUsuarioDTEF)(
        char *pNumeroCupom, char *pNumeroControle);

    typedef int(CALLING_COV *FDefineParametroTransacao)(int iCodigoParametro,
                                                        char *pValorParametro,
                                                        int iTamanhoParametro);
    typedef int(CALLING_COV *FDefineParametroConfirmacao)(
        int iCodigoParametro, char *pValorParamtero, char *pTamanhoParametro);

  public:
    int TransacaoCartaoCredito(char *valor, char *numeroCupom,
                               char *numeroControle);
    void carregaDll(void);
    void VersaoDPOS(char *pVersao);
    int TransacaoCartaoFrota(char *pValorTransacao, char *pNumeroCupom,
                             char *pNumeroControle);
    int ConfirmaCartaoFrota(char *pNumeroControle);
    int TransacaoCartaoFrotaCompleta(char *pValorTransacao, char *pNumeroCupom,
                                     char *pNumeroControle, char *pTipoProduto,
                                     char *pCodigoVeiculo,
                                     char *pCodigoCondutor, char *pKilometragem,
                                     char *pDadosServicos,
                                     char *pPermiteAlteracao, char *pReservado);
    int ImprimeCupomTEF(char *pPathArquivoCupomTEF, char *pMensagemOperador);
    int TransacaoCartaoDebito(char *pValorTransacao, char *pNumeroCupom,
                              char *pNumeroControle);
    int TransacaoCartaoParceleMais(char *pValorTransacao, char *pNumeroCupom,
                                   char *pNumeroControle, char *pCodigoTabela);
    int TransacaoCartaoParceleMaisCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pCodigoTabela, char *pNumeroParcelas, char *pValorParcela,
        char *pVencimentoPrimeiraParcela, char *pPermiteAlteracao);
    int TransacaoCartaoVoucher(char *pValorTransacao, char *pNumeroCupom,
                               char *pNumeroControle);
    int TransacaoCheque(char *pValorTransacao, char *pNumeroCupom,
                        char *pNumeroControle, char *pQuantidadeCheques,
                        char *pPeriodicidadeCheques, char *pDataPrimeiroCheque,
                        char *pCarenciaPrimeiroCheque);
    int TransacaoCelular(char *pValorTransacao, char *pNumeroCupom,
                         char *pNumeroControle);
    int TransacaoCancelamentoCelular(char *pNumeroControle);
    int TransacaoCancelamentoGarantiaCheque(char *pValorTransacao,
                                            char *pNumeroControle);
    int TransacaoCancelamentoPagamento(char *pNumeroControle);
    int EstornoPreAutorizacaoRedecard(char *pNumeroControle);
    int TransacaoConfirmacaoPreAutorizacao(char *pNumeroControle);
    int TransacaoCancelamentoPagamentoCompleta(char *pValorTransacao,
                                               char *pNumeroCupom,
                                               char *pNumeroControle,
                                               char *pPermiteAlteracao,
                                               char *pReservado);
    int ConfirmaCartaoCredito(char *pNumeroControle);
    int ConfirmaCartaoDebito(char *pNumeroControle);
    int ConfirmaCartaoVoucher(char *pNumeroControle);
    int ConfirmaDebitoBeneficiario(char *pNumeroControle);
    int ConfirmaCartao(char *pNumeroControle);
    int DesfazCartao(char *pNumeroControle);
    int FinalizaTransacao();
    int TransacaoConsultaParcelas(char *pNumeroControle);
    int TransacaoConsultaParcelasCredito(char *pNumeroControle);
    int TransacaoConsultaParcelasCelular(char *pNumeroControle);
    int TransacaoPreAutorizacao(char *pNumeroControle);
    int TransacaoPreAutorizacaoCartaoCredito(char *pNumeroControle);
    int TransacaoPreAutorizacaoCartaoFrota(char *pNumeroControle);
    int TransacaoFuncaoEspecial();
    int TransacaoResumoVendas(char *pNumeroControle);
    void DadosUltimaTransacao(char *pDadosUltimaTransacao);
    void ObtemCodigoRetorno(int iCodigoRetorno, char *pCodigoRetorno);
    ;
    void ObtemLogUltimaTransacao(char *oLogUltimaTransacao);
    void ObtemLogUltimaTransacaoTeleMarketing(char *pNumeroPDV,
                                              char *oLogUltimaTransacao);
    void ObtemLogUltimaTransacaoTeleMarketingMultiLoja(
        char *cNumeroEmpresa, char *cNumeroLoja, char *pNumeroPDV,
        char *oLogUltimaTransacao);
    void ObtemComprovanteTransacao(char *pNumeroControle, char *pComprovante,
                                   char *pComprovanteReduzido);
    void DadosUltimaTransacaoDiscada(char *oDadosUltimaTransacaoDiscada);
    void DadosUltimaTransacaoCB(char *pLogTitulo);
    int TransacaoCartaoCreditoCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorParcela,
        char *pValorTaxaServico, char *pPermiteAlteracao, char *pReservado);
    int TransacaoCartaoDebitoCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pNumeroParcelas, char *pSequenciaParcela,
        char *pDataDebito, char *pValorParcela, char *pValorTaxaServico,
        char *pPermiteAlteracao, char *pReservado);
    int TransacaoCartaoVoucherCompleta(char *pValorTransacao,
                                       char *pNumeroCupom,
                                       char *pNumeroControle, char *pReservado);
    int TransacaoChequeCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pQuantidadeCheques, char *pPeriodicidadeCheques,
        char *pDataPrimeiroCheque, char *pCarenciaPrimeiroCheque,
        char *pTipoDocumento, char *pNumeroDocumento,
        char *pQuantidadeChequesEx, char *pSequenciaCheque,
        char *pCamaraCompensacao, char *pNumeroBanco, char *pNumeroAgencia,
        char *pNumeroContaCorrente, char *pNumeroCheque, char *pDataDeposito,
        char *pValorCheque, char *pDataNascimentoCliente,
        char *pTelefoneCliente, char *pCMC7, char *pPermiteAlteracao,
        char *pReservado);
    int TransacaoCartaoPrivateLabelCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorEntrada,
        char *pValorTaxaServico, char *pPermiteAlteracao, char *pReservado);
    int TransacaoSimulacaoPrivateLabel(char *pValorTransacao,
                                       char *pNumeroCupom,
                                       char *pNumeroControle);
    int TransacaoConsultaPrivateLabel(char *pValorTransacao, char *pNumeroCupom,
                                      char *pNumeroControle);
    int TransacaoPagamentoPrivateLabel(char *pValorTransacao,
                                       char *pNumeroCupom,
                                       char *pNumeroControle);
    int TransacaoDebitoBeneficiario(char *pValorTransacao, char *pTipoBeneficio,
                                    char *pNumeroBeneficio, char *pNumeroCupom,
                                    char *pNumeroControle);
    void InfAdicionaisCartaoDebito(char *pDadosParcelas);
    void ObtemInfTransacaoDebitoParcelado(char *pDadosParcelas);
    int ConsultaParametrosCB(char *pParametros);
    int ConsultaPagamentoCB(char *pTipoConta, char *pCodigoBarrasDigitado,
                            char *pCodigoBarrasLido, char *pValorTitulo,
                            char *pValorDesconto, char *pValorAcrescimo,
                            char *pDataVencimento, char *pFormaPagamento,
                            char *pTrilhaCartao, char *pTipoSenha, char *pSenha,
                            char *pNSUCartao, char *pTipoCMC7, char *pCMC7,
                            char *pNumeroControle, char *pMensagemTEF,
                            char *pImprimeComprovante, char *pParametros,
                            char *pParametros2);
    int TransacaoPagamentoCB(char *pTipoConta, char *pCodigoBarrasDigitado,
                             char *pCodigoBarrasLido, char *pValorTitulo,
                             char *pValorDesconto, char *pValorAcrescimo,
                             char *pDataVencimento, char *pFormaPagamento,
                             char *pTrilhaCartao, char *pTipoSenha,
                             char *pSenha, char *pNSUCartao, char *pTipoCMC7,
                             char *pCMC7, char *pParametros2,
                             char *pNumeroControle);
    int TransacaoCancelamentoCB(char *pTipoConta, char *pCodigoBarrasDigitado,
                                char *pCodigoBarrasLido, char *pValorTitulo,
                                char *pNSUCartao, char *pNumeroControle);
    int InicializaSessaoCB(char *pNumeroControle);
    int FinalizaSessaoCB(char *pNumeroControle);
    int TransacaoResumoRecebimentosCB(char *pTipoRecebimento,
                                      char *pNumeroControle);
    int UltimaMensagemTEF(char *pMensagem);
    int TransacaoReimpressaoCupom();
    void ConfiguraDPOS();
    int InicializaDPOS();
    int FinalizaDPOS();
    int TransacaoFechamento(char *pDataMovimento, char *pQuantidadeVendas,
                            char *pValorTotalVendas,
                            char *pQuantidadeVendasCanceladas,
                            char *pValorTotalVendasCanceladas, char *pReservado,
                            char *pNumeroControle, char *pMensagemOperador);
    int TransacaoFechamentoPOS(char *pTipoRelatorio, char *pParametrosRelatorio,
                               char *pReservado, char *pNumeroCupom,
                               char *pNumeroControle);
    int TransacaoAtivacaoPOS(char *pCNPJ, char *pNumeroSerie,
                             char *pCodAtivacao, char *pNumeroControle);
    int TransacaoInicializacaoAreaPOS(char *pCNPJ, char *pNumeroSerie,
                                      char *pNumeroControle);
    int TransacaoLojaPOS(char *pCNPJ, char *pNumeroSerie, char *pCodigoEmpresa,
                         char *pCodigoLoja, char *pCodigoPDV,
                         char *pNumeroControle);
    void ObtemUltimoErro(char *pErro);
    void DadosUltimaTransacaoNaoAprovada(char *pDadosTransacaoNaoAprovada);
    int TransacaoColeta(int nTipoCartao, char *pValorTransacao, char *pTrilha1,
                        char *pTrilha2, char *pTrilha3, char *pParametros);
    int TransacaoColetaCartao(int nTipoCartao, char *pTrilha1, char *pTrilha2,
                              char *pTrilha3);
    int TransacaoColetaSenha(char *pValor, char *pSenha);
    int TransacaoColetaSenhaCartao(int nTipoCartao, char *pValor,
                                   char *pTrilha2, char *pReservado,
                                   char *pSenha);
    int ColetaPlanoPagamento(char *pDescricaoFormaPagamento,
                             char *pValorTransacao, char *pNumeroCupom,
                             char *pTipoFormaPagamento, char *pNumeroControle,
                             char *pNumeroParcelas, char *pValorAcrescimo,
                             char *pValorEntrada, char *pValorTotal,
                             char *pCodigoPlano, char *pPlano, char *pParcelas);
    int TransacaoAdministrativaTEFDiscado(char *pNumeroControle);
    void SelecionaTEF();
    void ExportaPlanos();
    int TransacaoQuantidade(char *pValorTransacao, char *pValorTransacaoMaximo,
                            char *pNumeroCupom);
    int TransacaoValor(char *pValorTransacao, char *pValorTransacaoMaximo,
                       char *pNumeroCupom);
    int TransacaoCartaoCreditoConfirmada(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartao, char *pDataVencimento, char *pCVV2,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorTaxaServico,
        char *pMensagemTEF, char *pReservado);
    int TransacaoCancelamentoConfirmada(char *pMultiLoja, char *pNumeroLoja,
                                        char *pNumeroPDV, char *pValorTransacao,
                                        char *pNumeroControle,
                                        char *pNumeroCartao, char *pMensagemTEF,
                                        char *pReservado);
    int PreAutorizacaoCreditoConfirmada(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartao, char *pDataVencimento, char *pCVV2,
        char *pValorTaxaServico, char *pNumeroAutorizacao, char *pMensagemTEF,
        char *pReservado);
    int ConfPreAutorizacaoCreditoConfirmada(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pDataPreAutor, char *pNumeroCartao, char *pDataVencimento,
        char *pCVV2, char *pTipoOperacao, char *pNumeroParcelas,
        char *pMensagemTEF, char *pReservado);
    int TransacaoManualPOS(char *pValorTransacao);
    int TransacaoManualPOSCompleta(char *pValorTransacao,
                                   char *pCodigoEstabelecimento, char *pData,
                                   char *pHora, char *pNumeroAutorizadora,
                                   char *pNumeroCartao, char *pTipoOperacao,
                                   char *pNumeroParcelas, char *pDataPreDatado,
                                   char *pNumeroControle);
    int TransacaoCartaoFidelidade(char *pValorTransacao, char *pNumeroCupom,
                                  char *pCompraPontos,
                                  char *pQuantidadeItensTaxaVariavel,
                                  char *pItensTaxaVariavel,
                                  char *pNumeroControle);
    int TransacaoDebitoFidelidade(char *pNumeroCupom, char *pNumeroControle);
    int ConfirmaCartaoFidelidade(char *pNumeroControle);
    int ExtratoCartaoAutorizadorDirecao(char *pNumeroCartao,
                                        char *pNumeroControle);
    int RecebimentoAutorizadorDirecao(char *pValorTransacao,
                                      char *pNumeroCartao,
                                      char *pNumeroControle);
    int EstornoRecebimentoAutorizadorDirecao(char *pValorTransacao,
                                             char *pNumeroCartao,
                                             char *pNumeroControle);
    int RecebimentoCarneAutorizadorDirecao(char *pValorTransacao,
                                           char *pNumeroCartao,
                                           char *pDataVencimento,
                                           char *pNumeroControle);
    int TransacaoPagamentoConvenio(char *pValorTransacao, char *pNumeroCupom,
                                   char *pNumeroControle);
    int ConfirmaConvenio(char *pNumeroControle);
    int TransacaoSaqueRedecard(char *pValorTransacao, char *pNumeroCupomFiscal,
                               char *pNumeroControle);
    int TransacaoSaque(char *pValorTransacao, char *pNumeroCupomFiscal,
                       char *pNumeroControle);
    int TransacaoCDC1MIN(char *pValorTransacao, char *pNumeroCupomFiscal,
                         char *pNumeroControle);
    int TransacaoSIMparcelado(char *pValorTransacao, char *pNumeroCupomFiscal,
                              char *pNumeroControle);
    int TransacaoCartaoCreditoIATA(char *pValorTransacao, char *pNumeroCupom,
                                   char *pNumeroControle);
    int TransacaoCartaoCreditoIATAConfirmada(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartao, char *pDataVencimento, char *pCVV2,
        char *pTipoOperacao, char *pNumeroParcelas, char *pValorTaxaServico,
        char *pValorEntrada, char *pMensagemTEF, char *pReservado);
    int TransacaoCartaoPrivateLabel(char *pValorTransacao, char *pNumeroCupom,
                                    char *pNumeroControle);
    void BeepErro();
    int TransacaoConsultaSaldo(char *pNumeroCupom, char *pNumeroControle);
    int TransacaoConsultaSaldoCelular(char *pNumeroCupom,
                                      char *pNumeroControle);
    int TransacaoConsultaSaldoCompleta(char *pNumeroCupom,
                                       char *pNumeroControle, char *pTipoCartao,
                                       char *pPermiteAlteracao,
                                       char *pReservado);
    int ColetaCartaoDebito(char *pValorTransacao, char *pNumeroCupom,
                           char *pParametrosTEF, char *pMensagemOperador);
    int ConsisteDadosCartaoDebito(char *pUltimosDigitos, char *pCodigoSeguranca,
                                  char *pMensagemOperador);
    int ColetaSenhaCartaoDebito(char *pNumeroControle, char *pMensagemOperador);
    int ConsultaParametrosPBM(char *pNumeroCupom, char *pQuantidadeRedes,
                              char *pDadosRedes, char *pMensagemOperador);
    int ConsultaProdutosPBM(char *pNumeroCupom, char *pRedePBM, char *pTrilha1,
                            char *pTrilha2, char *pDigitado, char *pAutorizacao,
                            char *pDadosCRM, char *pTipoVenda,
                            char *pValorTotalMedicamentos, char *pValorVista,
                            char *pValorCartao, char *pCodigoCredenciado,
                            char *pQuantidadeMedicamentos, char *pMedicamentos,
                            char *pReservado, char *pNumeroControle,
                            char *pMensagemOperador);
    int TransacaoVendaPBM(char *pValorTotalMedicamentos, char *pNumeroCupom,
                          char *pRedePBM, char *pTrilha1, char *pTrilha2,
                          char *pDigitado, char *pAutorizacao,
                          char *pNSUConsulta, char *pDadosCRM, char *pTipoVenda,
                          char *pValorVista, char *pValorCartao,
                          char *pCodigoCredenciado, char *pRegraNegocio,
                          char *pQuantidadeMedicamentos, char *pMedicamentos,
                          char *pReservado, char *pNumeroControle,
                          char *pNumeroControleRede, char *pMensagemOperador);
    int ConfirmaVendaPBM(char *pNumeroControle);
    int TransacaoReimpressaoVendaPBM();
    int TransacaoCancelamentoVendaPBM(char *pNumeroControle);
    int TransacaoCancelamentoPreAutorizacaoPBM(char *pNumeroControle);
    int TransacaoElegibilidadePBM(char *pNumeroCupom, char *pRedePBM,
                                  char *pMatricula, char *pDadosElegibilidade,
                                  char *pNumeroControle,
                                  char *pNumeroControleRede, char *pNomeCliente,
                                  char *pNomeMedico, char *pInformaDependente,
                                  char *pListaDependentes, char *pReservado,
                                  char *pMensagemOperador);
    int TransacaoPreAutorizacaoPBM(char *pNumeroCupom, char *pRedePBM,
                                   char *pAutorizacaoElegibilidade,
                                   char *pDadosFarmaceutico,
                                   char *pQuantidadeMedicamentos,
                                   char *pMedicamentos, char *pNumeroControle,
                                   char *pNumeroControleRede, char *pReservado,
                                   char *pMensagemOperador);
    int TransacaoCancelamentoPreAutorizacaoPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pMatricula, char *pReservado,
        char *pNumeroControle, char *pMensagemOperador);
    int TransacaoElegibilidadePBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pRedePBM, char *pMatricula,
        char *pDadosElegibilidade, char *pNumeroControle,
        char *pNumeroControleRede, char *pNomeCliente, char *pNomeMedico,
        char *pInformaDependente, char *pListaDependentes, char *pReservado,
        char *pMensagemOperador);
    int TransacaoPreAutorizacaoPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pRedePBM,
        char *pAutorizacaoElegibilidade, char *pDadosFarmaceutico,
        char *pQuantidadeMedicamentos, char *pMedicamentos,
        char *pNumeroControle, char *pNumeroControleRede, char *pReservado,
        char *pMensagemOperador);
    int ConsultaParametrosPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pQuantidadeRedes,
        char *pDadosRedes, char *pMensagemOperador);
    int ConsultaProdutosPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pNumeroCupom, char *pRedePBM, char *pTrilha1,
        char *pTrilha2, char *pDigitado, char *pAutorizacao, char *pDadosCRM,
        char *pTipoVenda, char *pValorTotalMedicamentos, char *pValorVista,
        char *pValorCartao, char *pCodigoCredenciado,
        char *pQuantidadeMedicamentos, char *pMedicamentos, char *pReservado,
        char *pNumeroControle, char *pMensagemOperador);
    int TransacaoVendaPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pValorTotalMedicamentos, char *pNumeroCupom,
        char *pRedePBM, char *pTrilha1, char *pTrilha2, char *pDigitado,
        char *pAutorizacao, char *pNSUConsulta, char *pDadosCRM,
        char *pTipoVenda, char *pValorVista, char *pValorCartao,
        char *pCodigoCredenciado, char *pRegraNegocio,
        char *pQuantidadeMedicamentos, char *pMedicamentos, char *pReservado,
        char *pNumeroControle, char *pNumeroControleRede,
        char *pMensagemOperador);
    int TransacaoCancelamentoVendaPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pTrilha1, char *pTrilha2, char *pDigitado,
        char *pReservado, char *pNumeroControle, char *pMensagemOperador);
    int TransacaoAberturaVendaPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pReservado, char *pPedirDadosComplementares,
        char *pDadosComplementares, char *pInfoDadosComplementares,
        char *pMensagemOperador);
    int TransacaoVendaProdutoPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pMedicamento, char *pReservado,
        char *pPedirDadosComplementares, char *pDadosComplementares,
        char *pInfoDadosComplementares, char *pMensagemOperador);
    int TransacaoFechamentoVendaPBM_TeleMarketing(
        char *pNumeroEmpresa, char *pMultiLoja, char *pNumeroLoja,
        char *pNumeroPDV, char *pRedePBM, char *pTrilha1, char *pTrilha2,
        char *pDigitado, char *pConfirmacao, char *pReservado,
        char *pAutorizacao, char *pNumeroControle, char *pMensagemOperador);
    void InformaDadosMedicamentos(char *pIndicativoReceita,
                                  char *pRegistroProfissional,
                                  char *pListaMedicamentos);
    int TransacaoSaqueSaldo(char *pValorTransacao, char *pNumeroCupom,
                            char *pNumeroControle);
    int TransacaoSaqueExtrato(char *pValorTransacao, char *pNumeroCupom,
                              char *pNumeroControle);
    int TransacaoConsultaAVS(char *pNumeroControle);
    int TransacaoConsultaAVSConfirmada(
        char *pMultiLoja, char *pNumeroLoja, char *pNumeroPDV,
        char *pNumeroCupom, char *pNumeroControle, char *pNumeroCartao,
        char *pDataVencimento, char *pCVV2, char *pEndereco, char *pNumero,
        char *pApto, char *pBloco, char *pCEP, char *pBairro, char *pCPF,
        char *pMensagemTEF, char *pReservado);
    int TransacaoPagamentoContasVisanet(char *pTipoConta, char *pCodigoBarras,
                                        char *pFormaPagamento,
                                        char *pTrilhaCartao,
                                        char *pValorTransacao,
                                        char *pNumeroCupom,
                                        char *pNumeroControle);
    int TransacaoEspecial(int iCodigoTransacao, char *pDados);
    int ConsultaValoresPrePago(char *pNumeroControle, char *pMensagemOperador);
    int TransacaoRecargaCelular(char *pCodigoArea, char *pNumeroTelefone,
                                char *pNumeroControle);
    int ConsultaParametrosTeledata(char *pDadosTeledata,
                                   char *pMensagemOperador);
    int ConsultaChequesGarantidos(char *pTipoDocumento, char *pNumeroDocumento,
                                  char *pDataInicialConsulta,
                                  char *pDataFinalConsulta,
                                  char *pNumeroControle);
    int TransacaoSaqueCompleta(char *pValorTransacao, char *pNumeroCupomFiscal,
                               char *pNumeroControle, char *pTipoTransacao,
                               char *pPlanoPagamento, char *pNumeroParcelas,
                               char *pPermiteAlteracao, char *pReservado);
    int TransacaoCartaoCompraSaque(char *pValorCompra, char *pValorSaque,
                                   char *pNumeroCupom, char *pNumeroControle);
    int TransacaoCartaoCompraSaqueCompleta(char *pValorCompra,
                                           char *pValorSaque,
                                           char *pNumeroCupom,
                                           char *pNumeroControle,
                                           char *pPermiteAlteracao);
    int TesteRedeAtiva(char *pNumeroEmpresa, char *pNumeroLoja,
                       char *pNumeroPDV, char *pCodigoRede, char *pStatusRede,
                       char *pMensagemOperador);
    int TransacaoCadastraSenha(char *pNumeroControle);
    void ConfiguraFluxoExternoDTEF5();
    int TrataDesfazimento(int iTratar);
    int ConsultaTransacao(char *pNumeroEmpresa, char *pNumeroLoja,
                          char *pNumeroPDV, char *pSolicitacao, char *pResposta,
                          char *pMensagemErro);
    int TransacaoeValeGas(char *pValorTransacao, char *pNumeroCupom,
                          char *pNumeroControle, char *pNumeroValeGas);
    int TransacaoConsultaeValeGas(char *pValorTransacao, char *pNumeroCupom,
                                  char *pNumeroControle, char *pNumeroValeGas,
                                  char *pDadosRetorno);
    int TransacaoCancelamentoPadrao(char *pNumeroControle);
    int ConfiguraModoTeleMarketing(int iModo);
    int ConfirmaCartaoTeleMarketing(char *pMultiLoja, char *pNumeroLoja,
                                    char *pNumeroPDV, char *pNumeroControle,
                                    char *pMensagemTEF, char *pReservado);
    int DesfazCartaoTeleMarketing(char *pMultiLoja, char *pNumeroLoja,
                                  char *pNumeroPDV, char *pNumeroControle,
                                  char *pMensagemTEF, char *pReservado);
    int TransacaoEstatistica(char *pNumeroControle);
    int TransacaoInjecaoChaves(char *pNumeroControle);
    int TransacaoTrocoSurpresa(char *pValorTransacao, char *pNumeroCupom,
                               char *pNumeroControle, char *pMensagemTEF,
                               char *pReservado);
    int IdentificacaoAutomacaoComercial(char *pNomeAutomacao,
                                        char *pVersaoAutomacao,
                                        char *pReservado);
    int ConsultaTabelasValeGas(char *pNumeroControle, char *pMensagemOperador);
    int DefineBandeiraTransacao(char *pCodigoBandeira);
    int DefineRedeTransacao(char *pCodigoRede);
    int ConverteCodigoRede(char *pCodigoRede, char *pCodigoRedeDTEF5,
                           char *pCodigoRedeDTEF8);
    int TransacaoAbreLote(char *pNumeroCupom, char *pNumeroControle,
                          char *pMensagemTEF, char *pReservado);
    int TransacaoFechaLote(char *pNumeroCupom, char *pNumeroControle,
                           char *pMensagemTEF, char *pReservado);
    void ConfiguraEmpresaLojaPDV(char *pNumeroEmpresa, char *pNumeroLoja,
                                 char *pNumeroPDV);
    int TransacaoResgatePremio(char *pNumeroCupom, char *pNumeroControle);
    int TransacaoVendaPOS(char *pCNPJ, char *pNumeroSerie,
                          char *pCodigoFrentista, char *pCPFCNPJ,
                          char *pPagamentoTEF, char *pCartaoProprio,
                          char *pValor, char *pTelefoneCliente,
                          char *pCodigoOrigem, char *pSaldoPontos,
                          char *pCodigoPesquisa, char *pReservado,
                          char *pNumeroControle);
    int TransacaoEstornoVendaPOS(char *pCNPJ, char *pNumeroSerie,
                                 char *pCodigoFrentista, char *pCPFCNPJ,
                                 char *pDataEstorno, char *pNSUEstorno,
                                 char *pAutorizacaoEstorno, char *pSaldoPontos,
                                 char *pNumeroControle);
    int TransacaoResgatePontosPOS(char *pCNPJ, char *pNumeroSerie,
                                  char *pCodigoFrentista, char *pCPFCNPJ,
                                  char *pQuantidadeProdutos,
                                  char *pListaProdutos, char *pSaldoPontos,
                                  char *pReservado, char *pNumeroControle);
    int TransacaoConsultaPontosPOS(char *pCNPJ, char *pNumeroSerie,
                                   char *pCodigoFrentista, char *pCPFCNPJ,
                                   char *pSaldoPontos, char *pReservado,
                                   char *pNumeroControle);
    int AtualizacaoFrentistasPOS(char *pCNPJ, char *pNumeroSerie,
                                 char *pNumeroFrentistas,
                                 char *pListaFrentistas, char *pReservado,
                                 char *pNumeroControle);
    int BuscaTabelaDTEF(int iTipoTabela, char *pNumeroControle);
    int TransacaoContratacao(char *pNumeroCupom, char *pNumeroControle);
    int TransacaoPrimeiraCompra(char *pValorTransacao, char *pNumeroCupom,
                                char *pNumeroControle);
    int TransacaoCadastro(char *pCNPJ, char *pNumeroSerie,
                          char *pCodigoFrentista, char *pCPFCNPJ,
                          char *pTelefoneCliente, char *pReservado,
                          char *pNumeroControle);
    int TransacaoDesativacaoPOS(char *pCNPJ, char *pNumeroSerie,
                                char *pNumeroSerieDesativacao, char *pFrentista,
                                char *pNumeroControle);
    int TransacaoConsultaPOSAtivos(char *pCNPJ, char *pNumeroSerie,
                                   char *pNumeroPOSAtivos,
                                   char *pListaPOSAtivos,
                                   char *pNumeroControle);
    int ProcuraPinPad(char *pDados);
    int ApagaTabelasPinPad(char *pDados);
    int TransacaoFuncoesAdministrativas(char *pValorTransacao,
                                        char *pNumeroCupom,
                                        char *pNumeroControle,
                                        char *pReservado);
    int TransacaoQuitacaoCartaFrete(char *pValorTransacao, char *pNumeroCupom,
                                    char *pNumeroControle);
    int TransacaoQuitacaoCartaFreteCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroCartaFrete, char *pQuantidadeChegada,
        char *pPermiteAlteracao, char *pReservado);
    int TransacaoCartaoCrediario(char *pValorTransacao, char *pNumeroCupom,
                                 char *pNumeroControle);
    int TransacaoSimulacaoCrediario(char *pValorTransacao, char *pNumeroCupom,
                                    char *pNumeroControle);
    int TransacaoPagamentoPrivateLabelCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pIndicadorDigitacao, char *pCodigoBarras, char *pNumeroCartao,
        char *pCodigoRede, char *pPermiteAlteracao, char *pReservado);
    int TransacaoCargaCartao(char *pValorTransacao, char *pNumeroCupom,
                             char *pNumeroControle);
    int TransacaoRecargaCartao(char *pValorTransacao, char *pNumeroCupom,
                               char *pNumeroControle);
    int CadastraPDV();
    int CadastraPDVTelemarketing(char *pNumeroEmpresa, char *pNumeroLoja,
                                 char *pNumeroPDV, char *pMensagemTEF);
    int TransacaoCartaoCrediarioCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pNumeroParcelas, char *pDataParcela1, char *pValorParcela1,
        char *pValorEntrada, char *pPermiteAlteracao, char *pReservado);
    int TransacaoConsultaPlanos(char *pValorTransacao, char *pNumeroControle,
                                char *pReservado, char *pPlanos);
    int ConsultaCadastroPDV(char *pMensagem);
    int ConsultaCadastroPDVTelemarketing(char *pNumeroEmpresa,
                                         char *pNumeroLoja, char *pNumeroPDV,
                                         char *pMensagemTEF);
    int TransacaoSimulacaoSaque(char *pValorTransacao, char *pNumeroCupom,
                                char *pNumeroControle);
    int TransacaoBloqueioCartao(char *pNumeroCupom, char *pNumeroControle);
    int TransacaoDesbloqueioCartao(char *pNumeroCupom, char *pNumeroControle);
    int TransacaoConsultaCadastroCPFCompleta(
        char *pValorTransacao, char *pNumeroCupom, char *pNumeroControle,
        char *pTipoOperacao, char *pDocEmissor, char *pPermiteAlteracao,
        char *pReservado);
    int TransacaoDepositoCompleta(char *pValorTransacao, char *pNumeroCupom,
                                  char *pNumeroControle, char *pTipoOperacao,
                                  char *pDocEmissor, char *pDocDestinatario,
                                  char *pPermiteAlteracao, char *pReservado);
    int
    TransacaoSaqueDomesticoCompleta(char *pValorTransacao, char *pNumeroCupom,
                                    char *pNumeroControle, char *pTipoOperacao,
                                    char *pCodigoPIN, char *pDocDestinatario,
                                    char *pPermiteAlteracao, char *pReservado);
    int TransacaoAlteraSenhaUsuarioDTEF(char *pNumeroCupom,
                                        char *pNumeroControle);
    int DefineParametroTransacao(int iCodigoParametro, char *pValorParametro,
                                 int iTamanhoParametro);
    int DefineParametroConfirmacao(int iCodigoParametro, char *pValorParamtero,
                                   char *pTamanhoParametro);

    void
    setCallBackDisplayTerminal(ptrCallBackDisplayTerminal cbDisplayTerminal);
    void setCallBackDisplayErro(ptrCallBackDisplayErro cbCallBackDisplayErro);
    void setCallBackDisplayMensagem(ptrCallBackMensagem cbDisplayMensagem);
    void setCallBackBeep(ptrCallBackBeep cbBeep);
    void setCallBackSolicitaConfirmacao(
        ptrCallBackSolicitaConfirmacao cbSolicitaConfirmacao);
    void setCallBackEntraCartao(ptrCallBackEntraCartao cbEntraCartao);
    void setCallBackEntraDataValidade(
        ptrCallBackEntraDataValidade cbEntraDataValidade);
    void setCallBackEntraData(ptrCallBackEntraData cbEntraData);
    void setCallBackEntraCodigoSeguranca(
        ptrCallBackEntraCodigoSeguranca cbEntraCodigoSeguranca);
    void setCallBackSelecionaOpcao(ptrCallBackSelecionaOpcao cbSelecionaOpcao);
    void setCallBackEntraValor(ptrCallBackEntraValor cbEntraValor);
    void setCallBackEntraValorEspecial(
        ptrCallBackEntraValorEspecial cbEntraValorEspecial);
    void setCallBackEntraNumero(ptrCallBackEntraNumero cbEntraNumero);
    void setCallBackOperacaoCancelada(
        ptrCallBackOperacaoCancelada cbOperacaoCancelada);
    void setCallBackSetaOperacaoCancelada(
        ptrCallBackSetaOperacaoCancelada cbSetaOperacaoCancelada);
    void setCallBackProcessaMensagens(
        ptrCallBackProcessaMensagens cbProcessaMensagens);
    void setCallBackEntraString(ptrCallBackEntraString cbEntraString);
    void setCallBackConsultaAVS(ptrCallBackConsultaAVS cbConsultaAVS);
    void setCallBackMensagemAdicional(
        ptrCallBackMensagemAdicional cbMensagemAdicional);
    void
    setCallBackImagemAdicional(ptrCallBackImagemAdicional cbImagemAdicional);
    void setCallBackEntraCodigoBarras(
        ptrCallBackEntraCodigoBarras cbEntraCodigoBarras);
    void setCallBackEntraCodigoBarrasLido(
        ptrCallBackEntraCodigoBarrasLido cbEntraCodigoBarrasLido);
    void setCallBackMensagemAlerta(ptrCallBackMensagemAlerta cbMensagemAlerta);
    void setCallBackPreviewComprovante(
        ptrCallBackPreviewComprovante cbPreviewComprovante);
    void
    setCallBackSelecionaPlanos(ptrCallBackSelecionaPlanos cbSelecionaPlanos);
    void setCallBackSelecionaPlanosEx(
        ptrCallBackSelecionaPlanosEx cbSelecionaPlanosEx);

    void setTransacaoSemTelas();
    void RegistrarFuncoesCallBack();
    void descarregaDll();
    void recarregaDll(bool bOperacoesSemTelas);
};
