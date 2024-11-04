# XML Converter to N-Triples

## Content Tables

1. [Informations](#informations)
1. [How to use](#how-to-use)
1. [Source](#source)

## Informations

This program uses C++ simple code to convert a RDF/XML file into a N-Triples file. Only those tags are handled by the program one th current version :

- rdf:about
- rdf:resource
- rdf:datatype
- rdf:nodeID
- xml:lang

The `/datas/` folder contains a sample of .xml files comptabiles with the program.

## How to Use

Use one of the following command to use.

```bash
./main.exe <path_to_xml_file>
./main.exe <path_to_folder>
./bin/main.exe <path_to_xml_file>
./bin/main.exe <path_to_folder>
```

The first one to give only one file to the program and convert it to N-Triples.

The second one to convert every .xml files in the given folder.

The destination of the converted files are in the same location as the .xml source file.

If you prefer using the binary file use one of the following command for the same result.

```bash
./main <path_to_xml_file>
./main <path_to_folder>
./bin/main <path_to_xml_file>
./bin/main <path_to_folder>
```

## Source

In the `/source/` folder lies the code. You can modify as you please just use the makefile commands :

```bash
make clean
make
make release
```

To:

- Clean the build
  - Recommanded if you see any errors...
- Make the simple binary file
- Release as .exe or binary in the `/bin` folder and ship the project.
  - To choose between `.exe` or binary, just comment and uncomment the `RELEASE_TARGET` line on the makefile (line 6 or 7).
