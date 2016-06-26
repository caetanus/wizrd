/*
 * Copyright (c) 2016 - Wizrd Team
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <iostream>
#include <tuple>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/utility/string_ref.hpp>
#include "../internal_webserver/requestparser.h"

using namespace Wizrd;

#define LOG BOOST_LOG_TRIVIAL(info)


TEST(request_parser_test, test_http11_request)
{
    Server::RequestParser parser;
    std::string test_get("GET /index.html HTTP/1.1\r\n"
                         "Host: www.example.com\r\n"
                         "Connection: keep-alive\r\n"
                         "X-App-Test: Foo-Bar\r\n"
                         "\r\n");
    Server::Request req;
    Server::Headers headers{{"Host", "www.example.com"}, {"Connection", "keep-alive"}, {"X-App-Test", "Foo-Bar"}};

    auto response = parser.parse(req, test_get.begin(), test_get.end());
    ASSERT_FALSE(std::get<1>(response) == Server::RequestParser::Error);
    EXPECT_EQ(req.host, "www.example.com");
    EXPECT_EQ(req.method, Server::Method::GET);
    EXPECT_EQ(req.versionMajor, 1);
    EXPECT_EQ(req.versionMinor, 1);
    EXPECT_EQ(true, req.keepAlive);
    EXPECT_EQ(headers, req.headers);
    EXPECT_TRUE(std::get<0>(response) == test_get.end());

}

TEST(request_parser_test, test_http10_request)
{
    Server::RequestParser parser;
    std::string test_post("POST /index.html HTTP/1.0\r\n"
                         "X-App-Test: Foo-Bar\r\n"
                         "\r\n"
                         "someDatablalalala");
    Server::Request req;

    Server::Headers headers{{"X-App-Test", "Foo-Bar"}};

    auto response = parser.parse(req, test_post.begin(), test_post.end());
    ASSERT_FALSE(std::get<1>(response) == Server::RequestParser::Error);
    EXPECT_EQ(req.host, "");
    EXPECT_EQ(req.method, Server::Method::POST);
    EXPECT_EQ(req.versionMajor, 1);
    EXPECT_EQ(req.versionMinor, 0);
    EXPECT_EQ(false, req.keepAlive);
    EXPECT_EQ("someDatablalalala", req.data);
    EXPECT_EQ(headers, req.headers);
    EXPECT_TRUE(std::get<0>(response) == test_post.end());

}

TEST(request_parser_test1, test_http11_request_with_keep_alive)
{
    Server::RequestParser parser;
    std::string test_post("POST /index.html HTTP/1.1\r\n"
                                 "X-App-Test: Foo-Bar\r\n"
                                 "Content-Length: 17\r\n"
                                 "Connection: Keep-Alive\r\n"
                                 "\r\n"
                                 "someDatablalalala"
                                 "\nthis data is outside the content lenght and should not be processed.");
    Server::Request req;
    Server::Headers headers{{"X-App-Test", "Foo-Bar"}, {"Content-Length", "17"}, {"Connection", "Keep-Alive"}};

    auto response = parser.parse(req, test_post.begin(), test_post.end());
    ASSERT_FALSE(std::get<1>(response) == Server::RequestParser::Error);
    EXPECT_EQ(req.host, "");
    EXPECT_EQ(req.method, Server::Method::POST);
    EXPECT_EQ(req.versionMajor, 1);
    EXPECT_EQ(req.contentLength, 17);
    EXPECT_EQ(req.versionMinor, 1);
    EXPECT_EQ(true, req.keepAlive);
    EXPECT_EQ("someDatablalalala", req.data);
    EXPECT_EQ(headers, req.headers);
    EXPECT_FALSE(std::get<0>(response) == test_post.end());

}

TEST(request_parser_test2, test_custom_method)
{
    Server::RequestParser parser;
    std::string test_get("RYU /index.html HTTP/1.1\r\n"
                         "Host: www.example.com\r\n"
                         "Connection: keep-alive\r\n"
                         "X-App-Test: Foo-Bar\r\n"
                         "\r\n");
    Server::Request req;
    Server::Headers headers{{"Host", "www.example.com"}, {"Connection", "keep-alive"}, {"X-App-Test", "Foo-Bar"}};

    auto response = parser.parse(req, test_get.begin(), test_get.end());
    ASSERT_FALSE(std::get<1>(response) == Server::RequestParser::Error);
    EXPECT_EQ(req.host, "www.example.com");
    EXPECT_EQ(req.method, Server::Method::CUSTOM);
    EXPECT_EQ(req.methodString, "RYU");
    EXPECT_EQ(req.versionMajor, 1);
    EXPECT_EQ(req.versionMinor, 1);
    EXPECT_EQ(true, req.keepAlive);
    EXPECT_EQ(headers, req.headers);
    EXPECT_TRUE(std::get<0>(response) == test_get.end());

}


TEST(request_parser_test2, test_invalid_request)
{
    Server::RequestParser parser;
    std::string test_get("RYU o.O /index.html HTTP/1.1\r\n"
                         "Host: www.example.com\r\n"
                         "Connection: keep-alive\r\n"
                         "X-App-Test: Foo-Bar\r\n"
                         "\r\n");
    Server::Request req;
    Server::Headers headers{{"Host", "www.example.com"}, {"Connection", "keep-alive"}, {"X-App-Test", "Foo-Bar"}};

    auto response = parser.parse(req, test_get.begin(), test_get.end());
    ASSERT_TRUE(std::get<1>(response) == Server::RequestParser::Error);
    EXPECT_FALSE(std::get<0>(response) == test_get.end());

}
