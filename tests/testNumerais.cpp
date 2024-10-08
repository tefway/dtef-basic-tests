#include "ClasseIntegracao.h"
#include <gtest/gtest.h>

TEST(CampoNumericoTests, FirstTest) {
    CampoNumerico<5> campo;
    ASSERT_STREQ(campo.get(), "00000");
    campo.set("123");
    ASSERT_STREQ(campo.get(), "00123");
    campo.set("123456");
    ASSERT_STREQ(campo.get(), "23456");
    campo.clear();
    ASSERT_STREQ(campo.get(), "00000");
}
