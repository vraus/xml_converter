#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <regex>
#include <stack>
#include <vector>
#include <optional>
#include <unordered_map>

enum Types
{
    about,
    parentNodeID,
    childNodeID,
    descriptionNodeID,
    resource,
    datatype,
    langLiteral,
    literal,
    closingTag,
    selfClosingTag
};

struct Pattern
{
    std::regex regex;
    Types type;

    std::optional<std::smatch> match(const std::string &line) const
    {
        std::smatch matchResult;
        if (std::regex_search(line, matchResult, regex))
            return matchResult;
        return std::nullopt;
    }
};

class Converter
{
public:
    Converter(const std::string &filePath, const std::string &fileName);
    ~Converter();

    void convertXml();

    std::string getContent() { return xmlContent; }

private:
    void loadFile(const std::string &filePath);
    void processNamespaces(const std::string &line);
    void processLine(const std::string &line);

    std::string replaceNamespacePrefixes(const std::string &line) const;
    std::string createTriple(const std::string &subject, const std::string &predicat, const std::string &object) const;

    void processAboutTag(const std::smatch &match);
    void processDescriptionNodeID(const std::smatch &match);
    void processParentNodeID(const std::smatch &match);
    void processChildNodeID(const std::smatch &match);
    void processResourceTag(const std::smatch &match);
    void processDatatypeTag(const std::smatch &match);
    void processLangLiteralTag(const std::smatch &match);
    void processLiteralTag(const std::smatch &match);
    void processClosingTag(const std::smatch &match);
    void processSelfClosingTag(const std::smatch &match);

    std::string xmlContent;
    std::stack<std::string> element;
    std::unordered_map<std::string, std::string> namespaces;

    std::string fileName;
    std::ofstream outputFile;

    int blankNodeCounter = 0;

    /**
     * About
     *  - [1] = prefix (namespace du sujet)
     *  - [2] = Tag (type d'objet pour le sujet)
     *  - [3] = URI (URI du sujet)
     *
     * NodeID
     *  - [1] : prefix (namespace du sujet)
     *  - [2] : Tag (type de node)
     *  - [3] : ID (identifiant unique du noeud, utilisé pour générer un blank node ID)
     *
     * Resource
     *  - [1] : prefix (namespace du prédicat)
     *  - [2] : Tag (nom du prédicat)
     *  - [3] : URI (URI de l'objet)
     *
     * Datatype
     *  - [1] : prefix (namespace du prédicat)
     *  - [2] : Tag (nom du prédicat)
     *  - [3] : URI (URI du type de données)
     *  - [4] : Literal (valeur littérale de l’objet)
     *
     * Littéral + lang
     *  - [1] : prefix (namespace du prédicat)
     *  - [2] : Tag (nom du prédicat)
     *  - [3] : lang (langue du littéral)
     *  - [4] : Literal (valeur littérale de l’objet)
     *
     * Littéral
     *  - [1] : prefix (namespace du prédicat)
     *  - [2] : Tag (nom du prédicat)
     *  - [3] : Literal (valeur littérale de l’objet)
     *
     * Fermeture de balise
     *  - [1] : prefix (namespace du tag)
     *  - [2] : Tag (nom du tag)
     */
    std::vector<Pattern> patterns;
};