#include <iostream>
#include "gtest/gtest.h"
#include "utils/url.h"


TEST(url_test_case, quote_test)
{
    EXPECT_EQ(Wizrd::URL::quote("http://en.wikipedia.org/wiki/Percent encoding"), "http%3A//en.wikipedia.org/wiki/Percent%20encoding");
    EXPECT_EQ(Wizrd::URL::quote("abc def"), "abc%20def");

}
TEST(url_test_case, quote_plus_test)
{
    EXPECT_EQ(Wizrd::URL::quotePlus("http://en.wikipedia.org/wiki/Percent encoding"), "http%3A//en.wikipedia.org/wiki/Percent+encoding");
    EXPECT_EQ(Wizrd::URL::quotePlus("abc def"), "abc+def");

}

TEST(url_test_case, unquote_test_regular)
{
    EXPECT_EQ(Wizrd::URL::unquote("%20%30"), " 0");
    EXPECT_EQ(Wizrd::URL::unquote("http%3A//en.wikipedia.org/wiki/Percent%20encoding"), "http://en.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquote("abc%20def"), "abc def");
}

TEST(url_test_case, unquote_test_unregular)
{
    EXPECT_EQ(Wizrd::URL::unquote("%20%30%"), " 0%");
    EXPECT_EQ(Wizrd::URL::unquote("http%3A//%4Hen.wikipedia.org/wiki/Percent%20encoding"), "http://%4Hen.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquote("%%abc%20def"), "%\xAB""c def");
}

TEST(url_test_case, unquote_plus_test_regular)
{
    EXPECT_EQ(Wizrd::URL::unquotePlus("%20%30"), " 0");
    EXPECT_EQ(Wizrd::URL::unquotePlus("http%3A//en.wikipedia.org/wiki/Percent+encoding"), "http://en.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquotePlus("abc%20def"), "abc def");
}

TEST(url_test_case, unquote_plus_test_unregular)
{
    EXPECT_EQ(Wizrd::URL::unquotePlus("%20%30%"), " 0%");
    EXPECT_EQ(Wizrd::URL::unquotePlus("http%3A//%4Hen.wikipedia.org/wiki/Percent%20encoding"), "http://%4Hen.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquotePlus("%%abc+def"), "%\xAB""c def");
}

TEST(url_test_case, url_decode_map_empty)
{
    auto result{Wizrd::URL::decodeMap("")};
    std::map<std::string, std::string> expect;

    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_map_common)
{
    auto result{Wizrd::URL::decodeMap("foo=bar&ba+=+baz+")};
    std::map<std::string, std::string> expect{{"foo", "bar"},
                                              {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_map_with_some_key_with_no_value)
{
    auto result{Wizrd::URL::decodeMap("foo&ba+=+baz+")};
    std::map<std::string, std::string> expect{{"foo", ""},
                                              {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_empty)
{

    auto result{Wizrd::URL::decode("")};
    std::vector<std::vector<std::string>> expect;
    EXPECT_EQ(result, expect);
}
TEST(url_test_case, url_decode_item_empty)
{

    auto result{Wizrd::URL::decode("foo=&bar=+")};
    std::vector<std::vector<std::string>> expect{{"foo", ""},{"bar", " "}};
    EXPECT_EQ(result, expect);
}


TEST(url_test_case, url_decode_common)
{

    auto result{Wizrd::URL::decode("foo=bar&ba+=+baz+")};
    std::vector<std::vector<std::string>> expect{{"foo", "bar"}, {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_with_some_keys_with_no_value)
{

    auto result{Wizrd::URL::decode("foo&ba+=+baz+")};
    std::vector<std::vector<std::string>> expect{{"foo"}, {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

