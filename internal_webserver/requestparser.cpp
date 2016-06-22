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
    currentBuffer_.clear();
    currentBuffer_.reserve(8192);
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
            currentBuffer_ += chr;
        else if (isSpace(chr)) {
            const auto method = methodTable.find(currentBuffer_);

            if (method != methodTable.end()) {
                request.method = method->second;
            }
            else {
                request.method = Method::CUSTOM;
            }
            request.methodString = std::move(currentBuffer_);
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
            request.url = std::move(currentBuffer_);
            state_ = Space_2;
            currentBuffer_.clear();
        }
        else
            currentBuffer_ += chr;
        break;
    case Space_2:
        if(!isSpace(chr))
            state_ = Http;
        break;
    case Http:
        if(isUpperAlpha(chr))
            currentBuffer_ += chr;
        else if (isSlash(chr)) {
            if (currentBuffer_== "HTTP") {
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
            currentBuffer_ += chr;
        }
        else if(isNewLine(chr))
        {
            state_ = NewLine;
            try{
                // the only versions of HTTP accepted is \d.\d
                if (currentBuffer_.length() != 3 || currentBuffer_[1] != '.')
                    return Error;
                request.versionMajor = boost::lexical_cast<int>(currentBuffer_[0]);
                request.versionMinor = boost::lexical_cast<int>(currentBuffer_[2]);

            }
            catch (boost::bad_lexical_cast)
            {
                return Error;
            }

            request.versionString = std::move(currentBuffer_);
            currentBuffer_.clear();
            currentBuffer_ += chr;
        }
        break;
    case NewLine:
        if(!isNewLine(chr)){
            if (currentBuffer_!= "\r\n")
                return Error;
            currentBuffer_.clear();
            currentBuffer_ += chr;
            state_ = Headers;
            headerState_ = Key;
        }
        else {
            currentBuffer_ += chr;
        }
        break;
    case Headers:
        return consumeHeaders(request, chr);
        break;
    case NewLine2:
        if(!isNewLine(chr)){
            if (currentBuffer_ != "\r\n\r\n")
                return Error;
            currentBuffer_.clear();
            if (request.contentLenght != -1)
                currentBuffer_.reserve(request.contentLenght);
            state_ = Data;
        }
        else {
            currentBuffer_ += chr;
            break;
        }
    case Data:
        // there actually two possible workflows here
        // when you have content lenght (in a possible keep alive connection
        // or when the connection is closed after the last byte
        if (request.contentLenght == -1 ||
            request.contentLenght > consumedContent_++)
            currentBuffer_ += chr;
        else if (request.contentLenght != -1) {
            request.data = std::move(currentBuffer_);
            currentBuffer_.clear();
            consumedContent_ = 0;
            state_ = Start;
            return Ok;
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

    static std::unordered_map<std::string,
                              decltype(currentImportantHeader)>  importantHeaders{{"host", Host},
                                                                                  {"content-lenght", ContentLenght},
                                                                                  {"content-type", ContentType},
                                                                                  {"connection", Connection},
                                                                                  {"keep-alive", KeepAlive},
                                                                                  {"max", Max}};
    static std::string currentHeader;
    switch(headerState_) {
    case HeaderStart:
        if (isNewLine(chr)) {
            currentBuffer_ += chr;
            state_ = NewLine2;
            break;
        }
        headerState_ = Key;
    case Key:
        if(isComma(chr))
        {
            auto lowerData = std::move(boost::algorithm::to_lower_copy(currentBuffer_));
            const auto& header = importantHeaders.find(lowerData);
            if (header != importantHeaders.end()) {
                currentImportantHeader = header->second;
            }
            std::string currentHeader = std::move(currentBuffer_);
            currentBuffer_.clear();
            headerState_ = Space;
        }
        else if (!isSpace(chr)) {
            currentBuffer_ += chr;
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
            currentBuffer_ += chr;
        }
        else {
            switch (currentImportantHeader) {
            case ContentType:
                //@TODO: check it if multipart later
                request.contentType = currentBuffer_;
                break;
            case ContentLenght:
                try {
                    request.contentLenght = boost::lexical_cast<int>(currentBuffer_);
                }
                catch(boost::bad_lexical_cast){
                    return Error;
                }

                break;
            case Connection:
                request.keepAlive = boost::iequals("keep-alive", currentBuffer_);
                break;
            case KeepAlive:
                boost::string_ref timeout(boost::string_ref(currentBuffer_).substr(currentBuffer_.find('=') + 1));
                try {
                   request.connectionTimeout = boost::lexical_cast<int>(timeout);
                }
                catch (boost::bad_lexical_cast) {}
                break;
            }


            request.headers.push_back(Header{std::move(currentHeader),
                                             std::move(currentBuffer_)});
            currentBuffer_.clear();
            currentBuffer_ += chr;
            headerState_ = HeaderStart;
        }
    }
}



} // Server namespace
} // Wizrd namespace
