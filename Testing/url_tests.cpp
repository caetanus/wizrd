#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
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
    Wizrd::paramsMap expect; // empty map

    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_map_common)
{
    auto result{Wizrd::URL::decodeMap("foo=bar&ba+=+baz+")};
    Wizrd::paramsMap expect{{"foo", "bar"},
                            {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_map_with_some_key_with_no_value)
{
    auto result{Wizrd::URL::decodeMap("foo&ba+=+baz+")};
    Wizrd::paramsMap expect{{"foo", ""},
                            {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_empty)
{

    auto result{Wizrd::URL::decode("")};
    Wizrd::params expect;
    EXPECT_EQ(result, expect);
}
TEST(url_test_case, url_decode_item_empty)
{

    auto result{Wizrd::URL::decode("foo=&bar=+")};
    Wizrd::params expect{{"foo", ""}, {"bar", " "}};
    EXPECT_EQ(result, expect);
}


TEST(url_test_case, url_decode_common)
{

    auto result{Wizrd::URL::decode("foo=bar&ba+=+baz+")};
    Wizrd::params expect{{"foo", "bar"}, {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_with_some_keys_with_no_value)
{

    auto result{Wizrd::URL::decode("foo&ba+=+baz+")};
    Wizrd::params expect{{"foo"}, {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_encode_common)
{
    Wizrd::params params{{"foo", "bar"}, {"  foo  ", "ba@"}};
    Wizrd::paramsMap map{{"foo", "bar"}, {"  foo  ", "ba@"}};
    std::string expect{"foo=bar&++foo++=ba%40"};
    auto result1{Wizrd::URL::encode(params)};
    auto result2{Wizrd::URL::encode(map)};
    EXPECT_EQ(result1, expect);
    // EXPECT_EQ(result2, expect);
    // not testing that because of the order,
    // try reencoding and see if the result is equal
    EXPECT_EQ(Wizrd::URL::decodeMap(result2), map);
}

TEST(url_test_case, url_encode_empty)
{
    Wizrd::params params;
    Wizrd::paramsMap map;
    std::string expect{""};
    auto result1{Wizrd::URL::encode(params)};
    auto result2{Wizrd::URL::encode(map)};
    EXPECT_EQ(result1, expect);
    EXPECT_EQ(result2, expect);

}

TEST(url_test_case, url_encode_invalid_number_of_parameters_should_throw_URLEncodeError)
{
    //creating a parameter vector with itens with key, value and one item having 3 items (that is invalid)
    Wizrd::params params{{"foo", "bar"}, {"  foo  "}, {"a", "b", "c"}};
    EXPECT_THROW(Wizrd::URL::encode(params), Wizrd::URLEncodeError);
}

