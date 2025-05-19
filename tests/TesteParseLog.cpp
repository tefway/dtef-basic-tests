#include "ParseLog.hpp"
#include <Poco/JSON/Object.h>
#include <gtest/gtest.h>

// Helper to build a fixed-length string for a field
inline std::string fillField(const std::string &value, size_t len) {
    if (value.size() >= len)
        return value.substr(0, len);
    return value + std::string(len - value.size(), '0');
}

// Helper to build a test record string for a given type
inline std::string buildTestRecord(const std::string &tipoRegistro,
                                   const std::vector<FieldDef> &fields) {
    std::string s;
    s += fillField("1234", 4);       // NumeroEmpresa
    s += fillField("567890", 6);     // NumeroLoja
    s += fillField(tipoRegistro, 3); // TipoRegistro
    for (const auto &f : fields) {
        s += fillField(
            f.name.substr(0, std::min<size_t>(f.name.size(), f.length)),
            f.length);
    }
    return s;
}

TEST(ParseLogTest, parseTransacaoParaJson_CCR) {
    auto fields = getFieldsCCR();
    std::string record = buildTestRecord("CCR", fields);
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    parseTransacaoParaJson(record, obj);

    EXPECT_EQ(obj->getValue<std::string>("NumeroEmpresa"), "1234");
    EXPECT_EQ(obj->getValue<std::string>("NumeroLoja"), "567890");
    EXPECT_EQ(obj->getValue<std::string>("TipoRegistro"), "CCR");
    EXPECT_EQ(obj->getValue<std::string>("Data"), fillField("Data", 8));
    EXPECT_EQ(obj->getValue<std::string>("NumeroCartao"),
              fillField("NumeroCartao", 37));
    EXPECT_EQ(obj->getValue<std::string>("CRLF"), fillField("CRLF", 2));
}

TEST(ParseLogTest, parseTransacaoParaJson_CDB) {
    auto fields = getFieldsCDB();
    std::string record = buildTestRecord("CDB", fields);
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    parseTransacaoParaJson(record, obj);

    EXPECT_EQ(obj->getValue<std::string>("TipoRegistro"), "CDB");
    EXPECT_EQ(obj->getValue<std::string>("NumeroCartao"),
              fillField("NumeroCartao", 37));
    EXPECT_EQ(obj->getValue<std::string>("ValorTaxaEmbarque"),
              fillField("ValorTaxaEmbarque", 12));
}

TEST(ParseLogTest, parseTransacaoParaJson_CCD) {
    auto fields = getFieldsCCD();
    std::string record = buildTestRecord("CCD", fields);
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    parseTransacaoParaJson(record, obj);

    EXPECT_EQ(obj->getValue<std::string>("TipoRegistro"), "CCD");
    EXPECT_EQ(obj->getValue<std::string>("Reservado1"),
              fillField("Reservado1", 1));
    EXPECT_EQ(obj->getValue<std::string>("Reservado3"),
              fillField("Reservado3", 55));
}

TEST(ParseLogTest, parseTransacaoParaJson_PrivateLabel) {
    auto fields = getFieldsPrivateLabel();
    std::string record = buildTestRecord("CPR", fields);
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    parseTransacaoParaJson(record, obj);

    EXPECT_EQ(obj->getValue<std::string>("TipoRegistro"), "CPR");
    EXPECT_EQ(obj->getValue<std::string>("PlanoPagamento"),
              fillField("PlanoPagamento", 5));
    EXPECT_EQ(obj->getValue<std::string>("CodigoItem"),
              fillField("CodigoItem", 11));
}

TEST(ParseLogTest, parseTransacaoParaJson_QRC) {
    auto fields = getFieldsQRC();
    std::string record = buildTestRecord("QRC", fields);
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    parseTransacaoParaJson(record, obj);

    EXPECT_EQ(obj->getValue<std::string>("TipoRegistro"), "QRC");
    EXPECT_EQ(obj->getValue<std::string>("NumeroControleRede"),
              fillField("NumeroControleRede", 80));
    EXPECT_EQ(obj->getValue<std::string>("MetodoPagamento"),
              fillField("MetodoPagamento", 25));
}

TEST(ParseLogTest, parseTransacaoParaJson_UnknownType) {
    std::string record = fillField("1234", 4) + fillField("567890", 6) +
                         fillField("ZZZ", 3) + "EXTRAFIELDS";
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    EXPECT_THROW(parseTransacaoParaJson(record, obj), std::runtime_error);
}

static std::string lograw =
    "0001000265CDB1905202510053400015        "
    "522656000000001000S06456950******1280*********************"
    "0725AV00000000000000000000000000000000000000000000000000R23456004193R23456"
    "                                          000000000000000000000000        "
    " 02340001\r\n";

// Helper to extract a substring from lograw based on field offsets and lengths
std::string extractField(const std::string &raw, size_t offset, size_t length) {
    return raw.substr(offset, length);
}

TEST(TesteParseLog, ParseLogRaw_CDB_Values) {
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    parseTransacaoParaJson(lograw, obj);

    // Check some expected values from the raw log
    EXPECT_EQ(obj->getValue<std::string>("NumeroEmpresa"), "0001");
    EXPECT_EQ(obj->getValue<std::string>("NumeroLoja"), "000265");
    EXPECT_EQ(obj->getValue<std::string>("TipoRegistro"), "CDB");
    EXPECT_EQ(obj->getValue<std::string>("Data"), "19052025");
    EXPECT_EQ(obj->getValue<std::string>("NumeroCartao"),
              "456950******1280*********************");
    EXPECT_EQ(obj->getValue<std::string>("ValorTaxaEmbarque"), "000000000000");
    EXPECT_EQ(obj->getValue<std::string>("CRLF"), "\r\n");
}
