#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "requestparser.h"
#include "utils/url.h"

namespace Wizrd {
namespace Server {
using namespace std::string_literals;

RequestParser::RequestParser()
    :state_(Start),
     consumed_(0)
{
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
    if (state_ != Start && request.contentLenght != -1
        && consumed_++ >= request.contentLenght) {
        return Error;
    }

    switch (state_)
    {
    case Start:
        state_ = Method;
        //initializing only what matters in the request;
        request.contentLenght = -1;
        request.keepAlive = false;
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
                auto version = boost::lexical_cast<float>(data);
            }
            catch (boost::bad_lexical_cast)
            {
                return Error;
            }

            std::tie(request.versionMajor, request.versionMinor) = std::make_tuple(boost::lexical_cast<int>(data[0]),
                                                                                   boost::lexical_cast<int>(data[2]));
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
        ContentLenght
    } currentImportantHeader;
    static std::unordered_map<std::string> importantHeaders{{"content-lenght", ContentLenght},
                                                            {"content-type", ContentType}};
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
            std::string lowerData(data.size());
            std::transform(data.begin(), data.end(), lowerData.begin(), std::tolower);
            const auto& header = importantHeaders.find(lowerData);
            if (header != importantHeaders.end()) {
                currentImportantHeader = *header;
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
            request.headers.push_back(Header{std::move(currentHeader), data});
            if (currentImportantHeader == ContentType)
                request.contentType = data;
            else if (currentImportantHeader == ContentLenght)
                request.contentLenght = boost::lexical_cast<int>(data);
            currentHeader.clear();
            currentBuffer_.clear();
            currentBuffer_ << chr;
            headerState_ = Start;
        }
    }
}



} // Server namespace
} // Wizrd namespace
