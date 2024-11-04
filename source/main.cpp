#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <string>

#include "public/Converter.hpp"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    if (argc < 2)
        throw std::runtime_error("Argument list doesn't match minimum requirments (2)!"
                                 "\n\tYou must provide at least one path for an XML file.");

    std::vector<std::string> xmlFiles;

    for (int i = 1; i < argc; ++i)
    {

        std::string filePath = argv[i];

        if (fs::is_regular_file(filePath))
        {
            if (filePath.size() < 4 || filePath.substr(filePath.size() - 4) != ".xml")
            {
                std::cerr << "Error: The file " << filePath << " does not have a .xml extension." << std::endl;
                continue;
            }
            xmlFiles.push_back(filePath);
        }
        else if (fs::is_directory(filePath))
        {
            for (const auto &entry : fs::directory_iterator(filePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".xml")
                    xmlFiles.push_back(entry.path().string());
            }
        }
        else
        {
            std::cerr << "Error: The path " << filePath << " is neither a vali file nor a directory.\n";
            continue;
        }
    }

    try
    {
        for (const auto &xmlFile : xmlFiles)
        {
            Converter converter(xmlFile, xmlFile + ".txt");
            converter.convertXml();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}