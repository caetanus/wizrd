#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <algorithm>
#include <cstdio>
#include <boost/utility/string_ref.hpp>
#include "requestparser.h"
#include "utils/url.h"

namespace Wizrd {
namespace Server {
using namespace std::string_literals;

RequestParser::RequestParser()
    :state_(Start),
     consumedContent_(0)
{
}


//initializing only what matters in the request;
void RequestParser::reset(Request &request)
{
    request.contentLenght = -1;
    request.keepAlive = false;
    request.connectionTimeout = 15;
    consumedContent_ = 0;
}

RequestParser::ResultType RequestParser::consume(Request &request, char chr)
{
    static std::unordered_map<std::string, Wizrd::Server::Method> methodTable{{"GET", Method::GET},
                                                                              {"HEAD", Method::HEAD},
                                                                              {"POST", Method::POST},
                                                                              {"PUT", Method::PUT},
                                                                              {"DELETE", Method::DELETE},
                                                                              {"TRACE", Method::TRACE},
                                                                              {"OPTIONS", Method::OPTIONS},
                                                                              {"CONNECT", Method::CONNECT},
                                                                              {"PATCH", Method::PATCH}};

    // when there is a content lenght header, it should be respected
    // due to HTTP/1.1
    // the request.contentLenght must be initialized as -1 in Start case

    switch (state_)
    {
    case Start:
        reset(request);
        state_ = Method;
    case Method:
        if (isUpperAlpha(chr))
            currentBuffer_ << chr;
        else if (isSpace(chr)) {
            const auto& data = currentBuffer_.str();
            const auto method = methodTable.find(data);

            if (method != methodTable.end()) {
                request.method = method->second;
            }
            else {
                request.method = Method::CUSTOM;
            }
            request.methodString = std::move(data);
            currentBuffer_.clear();
            state_ = Space_1;
        }
        else
            return Error;
        break;
    case Space_1:
        if (!isSpace(chr))
            state_ = Url;
        break;
    case Url:
        if(isSpace(chr))
        {
            const auto& data = currentBuffer_.str();
            request.url = std::move(Wizrd::URL::unquotePlus(data));
            state_ = Space_2;
            currentBuffer_.clear();
        }
        else
            currentBuffer_ << chr;
        break;
    case Space_2:
        if(!isSpace(chr))
            state_ = Http;
        break;
    case Http:
        if(isUpperAlpha(chr))
            currentBuffer_ << chr;
        else if (isSlash(chr)) {
            const auto& data = currentBuffer_.str();
            if (data == "HTTP") {
                state_ = Version;
                currentBuffer_.clear();
            }
            else return Error;
        }
        else {
            return Error;
        }
    case Version:
        if(isFloat(chr))
        {
            currentBuffer_ << chr;
        }
        else if(isNewLine(chr))
        {
            const auto& data = currentBuffer_.str();
            state_ = NewLine;
            try{
                // the only versions of HTTP accepted is \d.\d
                if (data.length() != 3)
                    return Error;
                request.versionMajor = boost::lexical_cast<int>(data[0]);
                request.versionMinor = boost::lexical_cast<int>(data[2]);

            }
            catch (boost::bad_lexical_cast)
            {
                return Error;
            }

            request.versionString = std::move(data);
            currentBuffer_.clear();
            currentBuffer_ << chr;
        }
        break;
    case NewLine:
        if(!isNewLine(chr)){
            const auto& data = currentBuffer_.str();
            if (data != "\r\n")
                return Error;
            currentBuffer_.clear();
            currentBuffer_ << chr;
            state_ = Headers;
            headerState_ = Key;
        }
        else {
            currentBuffer_ << chr;
        }
        break;
    case Headers:
        return consumeHeaders(request, chr);
        break;
    case NewLine2:
        if(!isNewLine(chr)){
            const auto& data = currentBuffer_.str();
            if (data != "\r\n\r\n")
                return Error;
            currentBuffer_.clear();
            state_ = Data;
        }
        else {
            currentBuffer_ << chr;
            break;
        }
    case Data:
        if (isNewLine(chr)) {
            return Ok;
        }
        else {
            currentBuffer_ << chr;
        }

    }
    return Processing;
}

RequestParser::ResultType RequestParser::consumeHeaders(Request &request, char chr)
{
    static enum {
        ContentType,
        ContentLenght,
        Connection,
        KeepAlive,
        Max,
        Host
    } currentImportantHeader;
    static std::unordered_map<std::string, decltype(currentImportantHeader)> importantHeaders{{"host", Host},
                                                                                              {"content-lenght", ContentLenght},
                                                                                              {"content-type", ContentType},
                                                                                              {"connection", Connection},
                                                                                              {"keep-alive", KeepAlive},
                                                                                              {"max", Max}};
    static std::string currentHeader;
    switch(headerState_) {
    case Start:
        if (isNewLine(chr)) {
            currentBuffer_ << chr;
            state_ = NewLine2;
            break;
        }
        headerState_ = Key;
    case Key:
        if(isComma(chr))
        {
            const std::string& data = currentBuffer_.str();
            auto lowerData = std::move(boost::algorithm::to_lower_copy(data));
            const auto& header = importantHeaders.find(lowerData);
            if (header != importantHeaders.end()) {
                currentImportantHeader = header->second;
            }
            std::string currentHeader = data;
            currentBuffer_.clear();
            headerState_ = Space;
        }
        else if (!isSpace(chr)) {
            currentBuffer_ << chr;
        }
        else {
            return Error;
        }
        break;
    case Space:
        if (!isSpace(chr)) {
            headerState_ = Value;
        }
        else
            break;
    case Value:
        if(!isNewLine(chr)) {
            currentBuffer_ << chr;
        }
        else {
            const auto& data = currentBuffer_.str();
            switch (currentImportantHeader) {
            case ContentType:
                //@TODO: check it if multipart later
                request.contentType = data;
                break;
            case ContentLenght:
                try {
                    request.contentLenght = boost::lexical_cast<int>(data);
                }
                catch(boost::bad_lexical_cast){
                    return Error;
                }

                break;
            case Connection:
                request.keepAlive = boost::iequals("keep-alive", data);
                break;
            case KeepAlive:
                std::string timeout (data.begin() + data.find('=') + 1, data.end());
                try {
                   request.connectionTimeout = boost::lexical_cast<int>(timeout);
                }
                catch (boost::bad_lexical_cast) {}
                break;
            }


            request.headers.push_back(Header{std::move(currentHeader), data});
            currentBuffer_.clear();
            currentBuffer_ << chr;
            headerState_ = HeaderStart;
        }
    }
}



} // Server namespace
} // Wizrd namespace
