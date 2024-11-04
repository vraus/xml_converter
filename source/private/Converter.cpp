#include "../public/Converter.hpp"

Converter::Converter(const std::string &filePath, const std::string &fileName)
{
    loadFile(filePath);

    patterns = {
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)\s+rdf:about=\"([^\"]+)\">)"), Types::about},
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)\s+rdf:nodeID=\"([^\"]+)\">)"), Types::parentNodeID},
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)\s+rdf:nodeID=\"([^\"]+)\"/>)"), Types::childNodeID},
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)\s+rdf:resource=\"([^\"]+)\")"), Types::resource},
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)\s+rdf:datatype=\"([^\"]+)\">([^<]*)</\1:\2>)"), Types::datatype},
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)\s+xml:lang=\"([a-zA-Z\-]+)\">([^<]*)</\1:\2>)"), Types::langLiteral},
        {std::regex(R"(<([a-zA-Z0-9]+):([a-zA-Z0-9]+)>([^<]*)</\1:\2>)"), Types::literal},
        {std::regex(R"(</([a-zA-Z0-9]+):([a-zA-Z0-9]+)>)"), Types::closingTag}};

    this->fileName = fileName;
}

Converter::~Converter() = default;

void Converter::convertXml()
{
    std::istringstream stream(xmlContent);
    std::string line;

    while (std::getline(stream, line))
        processNamespaces(line);

    stream.clear();
    stream.seekg(0, std::ios::beg);

    outputFile.open(fileName);

    while (std::getline(stream, line))
        processLine(line);

    std::cout << "Xml converted. Files are at the same location as xml files.\n";

    outputFile.close();
}

void Converter::loadFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error("Error: Cannot open file \"" + filePath + "\".");

    std::stringstream buffer;
    buffer << file.rdbuf();
    xmlContent = buffer.str();
}

void Converter::processNamespaces(const std::string &line)
{
    std::regex namespaceRegex(R"(xmlns:([a-zA-Z0-9]+)=\"([^\"]+)\")");
    std::smatch match;

    if (std::regex_search(line, match, namespaceRegex))
    {
        std::string prefix = match[1].str();
        std::string uri = match[2].str();
        namespaces[prefix] = uri;
    }
}

void Converter::processLine(const std::string &line)
{
    for (const auto &pattern : patterns)
    {
        auto matchResult = pattern.match(line);
        if (matchResult)
        {
            const std::smatch &match = *matchResult;

            switch (pattern.type)
            {
            case Types::about:
                processAboutTag(match);
                break;
            case Types::childNodeID:
                processChildNodeID(match);
                break;
            case Types::parentNodeID:
                processParentNodeID(match);
                break;
            case Types::resource:
                processResourceTag(match);
                break;
            case Types::datatype:
                processDatatypeTag(match);
                break;
            case Types::langLiteral:
                processLangLiteralTag(match);
                break;
            case Types::literal:
                processLiteralTag(match);
                break;
            case Types::closingTag:
                processClosingTag(match);
                break;
            default:
                // Ignore lines not recognized by the regex
                break;
            }
        }
    }
}

std::string Converter::replaceNamespacePrefixes(const std::string &line) const
{
    std::string result = line;
    for (const auto &[prefix, uri] : namespaces)
    {
        std::string pattern = prefix + ':';
        std::string replacement = uri;
        size_t pos = result.find(pattern);
        while (pos != std::string::npos)
        {
            result.replace(pos, pattern.length(), replacement);
            pos = result.find(pattern, pos + replacement.length());
        }
    }

    return result;
}

std::string Converter::createTriple(const std::string &subject, const std::string &predicat, const std::string &object) const
{
    return subject + " " + predicat + " " + object + " .\n";
}

void Converter::processAboutTag(const std::smatch &match)
{
    std::string object = "<" + replaceNamespacePrefixes(match[1].str() + ":" + match[2].str()) + ">";
    std::string objectTag = match[2].str();
    std::string subject = (match[3].str().compare(0, 7, "_:genid") ? ("<" + match[3].str() + ">") : match[3].str());
    std::string predicat = "<" + replaceNamespacePrefixes("rdf:type") + ">";

    element.push(subject);
    namespaces[match[1].str() + ":" + objectTag] = subject;

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processChildNodeID(const std::smatch &match)
{
    std::string predicat = "<" + replaceNamespacePrefixes(match[1].str() + ":" + match[2].str()) + ">";
    std::string uri = match[3].str();

    if (namespaces.find(uri) == namespaces.end())
    {
        std::string generatedNodeID = "_:genid" + std::to_string(++blankNodeCounter);
        namespaces[uri] = generatedNodeID;
    }

    std::string subject = element.top();
    std::string object = namespaces[uri];

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processParentNodeID(const std::smatch &match)
{
    std::string object = "<" + replaceNamespacePrefixes(match[1].str() + ':' + match[2].str()) + ">";
    std::string uri = match[3].str();
    std::string predicat = "<" + replaceNamespacePrefixes("rdf:type") + ">";

    if (namespaces.find(uri) == namespaces.end())
    {
        std::string generatedNodeID = "_:genid" + std::to_string(++blankNodeCounter);
        namespaces[uri] = generatedNodeID;
    }

    element.push(namespaces[uri]);
    std::string subject = element.top();

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processResourceTag(const std::smatch &match)
{
    std::string subject = element.top();
    std::string predicat = "<" + replaceNamespacePrefixes(match[1].str() + ':' + match[2].str()) + ">";
    std::string object = "<" + match[3].str() + ">";

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processDatatypeTag(const std::smatch &match)
{
    std::string subject = element.top();
    std::string predicat = "<" + replaceNamespacePrefixes(match[1].str() + ':' + match[2].str()) + ">";
    std::string object = "\"" + match[4].str() + "\"^^<" + match[3].str() + ">";

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processLangLiteralTag(const std::smatch &match)
{
    std::string subject = element.top();
    std::string predicat = "<" + replaceNamespacePrefixes(match[1].str() + ':' + match[2].str()) + ">";
    std::string object = "\"" + match[4].str() + "\"@" + match[3].str();

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processLiteralTag(const std::smatch &match)
{
    std::string subject = element.top();
    std::string predicat = "<" + replaceNamespacePrefixes(match[1].str() + ':' + match[2].str()) + ">";
    std::string object = "\"" + match[3].str() + "\"";

    outputFile << createTriple(subject, predicat, object);
}

void Converter::processClosingTag(const std::smatch &match)
{
    std::string closingKey = match[1].str() + ":" + match[2].str();
    if (namespaces.find(closingKey) != namespaces.end())
    {
        std::string expectedURI = namespaces[closingKey];
        if (element.top() == expectedURI)
            element.pop();
    }
}