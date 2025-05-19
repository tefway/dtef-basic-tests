#pragma once
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <string>
#include <vector>

struct FieldDef {
    std::string name;
    size_t length;
};

// Definição dos campos fixos de cabeçalho para todas as transações
static const std::vector<FieldDef> HEADER_FIELDS = {
    {"NumeroEmpresa", 4}, {"NumeroLoja", 6}, {"TipoRegistro", 3}};

static auto getFieldsCCR() -> std::vector<FieldDef> {
    return {{"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"TipoColetaCartao", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"VencimentoCartao", 4},
            {"TipoOperacao", 2},
            {"NumeroParcelas", 2},
            {"CartaoPrePago", 1},
            {"Reservado1", 9},
            {"ValorParcela", 12},
            {"ValorTaxaServico", 12},
            {"Reservado2", 9},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"ValorTaxaEmbarque", 12},
            {"CPFCNPJ", 14},
            {"Reservado3", 29},
            {"CodigoOperadora", 4},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

static auto getFieldsCDB() -> std::vector<FieldDef> {
    return {// Cartão de Débito
            {"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"CartaoPrePago", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"VencimentoCartao", 4},
            {"TipoOperacao", 2},
            {"NumeroParcelas", 2},
            {"SequenciaParcela", 2},
            {"DataDebito", 8},
            {"ValorParcela", 12},
            {"ValorTaxaServico", 12},
            {"TipoTransacao", 1},
            {"CodigoSupervisor", 8},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"TextoGenerico", 22},
            {"ValorSaque", 12},
            {"ValorTaxaEmbarque", 12},
            {"Reservado3", 9},
            {"CodigoOperadora", 4},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

static auto getFieldsCCD() -> std::vector<FieldDef> {
    return {// Cartão Crediário
            {"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"Reservado1", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"VencimentoCartao", 4},
            {"TipoOperacao", 2},
            {"NumeroParcelas", 2},
            {"SequenciaParcela", 2},
            {"DataDebito", 8},
            {"ValorParcela", 12},
            {"Reservado2", 21},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"Reservado3", 55},
            {"CodigoOperadora", 4},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

// Campos Private Label (CPR, CPL, SPL, FPL)
static auto getFieldsPrivateLabel() -> std::vector<FieldDef> {
    return {{"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"TipoColetaCartao", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"VencimentoCartao", 4},
            {"TipoOperacao", 2},
            {"NumeroParcelas", 2},
            {"Reservado1", 10},
            {"ValorEntrada", 12},
            {"ValorTaxaServico", 12},
            {"Reservado2", 9},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"PlanoPagamento", 5},
            {"VencimentosAPular", 2},
            {"CodigoItem", 11},
            {"FormaPagamento", 1},
            {"Reservado3", 36},
            {"CodigoOperadora", 4},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

// Campos CCV (Cartão Voucher)
static auto getFieldsCCV() -> std::vector<FieldDef> {
    return {{"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"Reservado1", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"VencimentoCartao", 4},
            {"Reservado2", 47},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"Reservado3", 55},
            {"CodigoOperadora", 4},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

// Campos Fidelidade (CPF, DPF)
static auto getFieldsFidelidade() -> std::vector<FieldDef> {
    return {{"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"Reservado1", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"NSUCancelado", 6},
            {"NumeroDocumento", 20},
            {"PontosAdquiridos", 8},
            {"FatorCompra", 7},
            {"Reservado2", 10},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"Reservado3", 63},
            {"CRLF", 2}};
}

// Campos QRC (QR Code)
static auto getFieldsQRC() -> std::vector<FieldDef> {
    return {{"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 80},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 80},
            {"MetodoPagamento", 25},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

// Campos CNC (Cancelamento)
static auto getFieldsCNC() -> std::vector<FieldDef> {
    return {{"Data", 8},
            {"Hora", 6},
            {"CodigoTransacao", 2},
            {"NumeroEquipamento", 3},
            {"NumeroFiscalEquipamento", 8},
            {"NumeroCupom", 6},
            {"ValorTransacao", 12},
            {"TipoColetaCartao", 1},
            {"NumeroAutorizador", 2},
            {"NumeroCartao", 37},
            {"VencimentoCartao", 4},
            {"Reservado1", 4},
            {"NumeroControleCancelado", 6},
            {"Reservado2", 37},
            {"CodigoResposta", 2},
            {"NumeroControleRede", 9},
            {"NumeroControle", 6},
            {"NumeroAutorizacaoRede", 6},
            {"MensagemErro", 20},
            {"Reservado3", 55},
            {"CodigoOperadora", 4},
            {"NumeroAutorizador8", 4},
            {"CRLF", 2}};
}

// Mapeamento dos campos específicos por tipo de registro
static const std::map<std::string, std::vector<FieldDef>> RECORD_DEFS = {
    {"CCR", getFieldsCCR()},          {"CDB", getFieldsCDB()},
    {"CCD", getFieldsCCD()},          {"CPR", getFieldsPrivateLabel()},
    {"CPL", getFieldsPrivateLabel()}, {"SPL", getFieldsPrivateLabel()},
    {"FPL", getFieldsPrivateLabel()}, {"CCV", getFieldsCCV()},
    {"CPF", getFieldsFidelidade()},   {"DPF", getFieldsFidelidade()},
    {"QRC", getFieldsQRC()},          {"CNC", getFieldsCNC()}};

// Função genérica de parsing
inline auto parseTransacaoParaJson(const std::string &str,
                                   Poco::JSON::Object::Ptr &obj) {
    size_t pos = 0;
    // Preenche cabeçalho
    for (const auto &f : HEADER_FIELDS) {
        obj->set(f.name, str.substr(pos, f.length));
        pos += f.length;
    }
    std::string tipo = obj->getValue<std::string>("TipoRegistro");
    // Encontra definição de campos específicos
    auto it = RECORD_DEFS.find(tipo);
    if (it == RECORD_DEFS.end()) {
        throw std::runtime_error("Tipo de registro desconhecido: " + tipo);
    }
    // Preenche campos específicos
    for (const auto &f : it->second) {
        obj->set(f.name, str.substr(pos, f.length));
        pos += f.length;
    }
    return obj;
}
