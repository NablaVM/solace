#include <libnabla/assembler.hpp>
#include <libnabla/json.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace
{
    //  Default file names for in/out
    //
    static const char * DEFAULT_FILE_IN  = "solace.in";
    static const char * DEFAULT_FILE_OUT = "solace.out";
    static const char * CONFIG_FILE      = "config.json";
    static const char * TEMP_FILE_NAME   = ".__solace__file_gen.asm";

    //  An argument struct
    //
    struct Args
    {
        std::string short_arg;
        std::string long_arg;
        std::string description;
    };

    //  Argument list - Mostly for help
    //
    std::vector<Args> SolaceArguments;
}

// ------------------------------------------------------------
//  Show the help message for Solace
// ------------------------------------------------------------

int help()
{
    std::cout << " ▼ Solace ▼ | Help" << std::endl 
              << "----------------------------------------------" 
              << std::endl
              << "Short\tLong\t\tDescription" << std::endl
              << "----------------------------------------------"
              << std::endl;

    for(auto & a : SolaceArguments)
    {
        std::cout << a.short_arg << "\t" << a.long_arg << "\t" << a.description << std::endl;
    }

    std::cout << "----------------------------------------------"
              << std::endl 
              << "Solace defaults to expect 'solace.in' and outputs 'solace.out' by default."
              << std::endl
              << "Solace only takes in a single file with the -f option."
              << std::endl;

    return 0;
}

// ------------------------------------------------------------
//  Check the file name to ensure we didn't goof and grab an arg
// ------------------------------------------------------------

bool checkObjectName(std::string obj)
{
    if (obj.size() == 0)
    {
        std::cerr << "Name given was somehow size '0'" << std::endl;
        return false;
    }

    for(auto & a : SolaceArguments)
    {
        if (obj == a.short_arg || obj == a.long_arg)
        {
            std::cerr << "Improper name given (you gave an argument where we expected a file / directory name) : " << obj << std::endl;
            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------
//  Remove file
// ------------------------------------------------------------

void removeTempFile(bool assembleVerbose)
{
    auto file_path = std::filesystem::path(TEMP_FILE_NAME);
    
    if(!std::filesystem::remove(TEMP_FILE_NAME))
    {
        if(assembleVerbose) { std::cout << "Failed to remove temporary file from project builder" << std::endl; }
    }
}

// ------------------------------------------------------------
//  Load a project directory (prototype for large method)
// ------------------------------------------------------------

void loadProject(std::string directory);

// ------------------------------------------------------------
//  Entry method
// ------------------------------------------------------------

int main(int argc, char ** argv)
{
    if(argc == 1)
    {
        std::cout << "No file name given" << std::endl;
        exit(EXIT_FAILURE);
    }

    SolaceArguments = {

        { "-h", "--solhelp",  "Display help message."},
        { "-v", "--verbose",  "Set assembler to VERBOSE mode" },
        { "-p", "--project",  "Assemble an asm project"},
        { "-f", "--filein",   "File to assemble"},
        { "-o", "--output",   "Output file name"}
    };

    bool assembleVerbose = false;
    bool assembleProject = false;

    std::string file_out = DEFAULT_FILE_OUT;
    std::string file_in  = DEFAULT_FILE_IN;
    std::string project_in = "";

    std::vector<std::string> args(argv, argv + argc);

    for(int i = 0; i < argc; i++)
    {
        //  Verbose Flag
        //
        if(args[i] == "-v" || args[i] == "--verbose")
        {
            assembleVerbose = true;
        }

        // Help
        //
        if(args[i] == "-h" || args[i] == "--solhelp")
        {
            return help();
        }

        //  Output file name
        //
        if(args[i] == "-o" || args[i] == "--output")
        {
            if(i == argc - 1)
            {
                std::cerr << "Error: Output file name not given for argument" << std::endl;
                return 1;
            }

            if(file_out != DEFAULT_FILE_OUT)
            {
                std::cerr << "Error: Output file name already given" << std::endl;
                return 1;
            }

            file_out = args[i+1];

            if(!checkObjectName(file_out))
            {
                std::cerr << "Error: Bad output file name given. Aborting" << std::endl;
                return 1;
            }
        }

        //  Input file name
        //
        if(args[i] == "-f" || args[i] == "--filein")
        {
            if(assembleProject)
            {
                std::cerr << "Error: Can not specify input file name if project flag active" << std::endl;
                return 1;
            }

            if(i == argc - 1)
            {
                std::cerr << "Error: Input file name not given for argument" << std::endl;
                return 1;
            }

            if(file_in != DEFAULT_FILE_IN)
            {
                std::cerr << "Error: Input file name already given" << std::endl;
                return 1;
            }

            file_in = args[i+1];

            if(!checkObjectName(file_in))
            {
                std::cerr << "Error: Bad input file name given. Aborting" << std::endl;
                return 1;
            }
        }
        
        //  Assemble a project
        //
        if(args[i] == "-p" || args[i] == "--project")
        {

            if(file_in != DEFAULT_FILE_IN)
            {
                std::cerr << "Error: Can not set project flag active if an input file name is given" << std::endl;
                return 1;
            }

            if(i == argc - 1)
            {
                std::cerr << "Error: Project directory name not given for argument" << std::endl;
                return 1;
            }

            project_in = args[i+1];

            if(!checkObjectName(project_in))
            {
                std::cerr << "Error: Bad directory name given. Aborting" << std::endl;
                return 1;
            }

            assembleProject = true;
        }
    }

    //  If we are assembling a project, we need to build the project temp file
    //
    if(assembleProject)
    {
        // If this fails, the program dies
        loadProject(project_in);

        file_in = TEMP_FILE_NAME;
    }

    //  Call into the assembler with the given parameters
    //
    std::vector<uint8_t> bytes;

    if(!ASSEMBLER::ParseAsm(file_in, bytes, assembleVerbose))
    {
        std::cerr << "Error: Failed to parse asm. Goodbye." << std::endl;

        //  Clean if we need to
        //
        if(assembleProject){ removeTempFile(assembleVerbose); }
        return 1;
    }

    //  Write the bytes generated out to the output file
    //
    std::ofstream out(file_out, std::ios::out | std::ios::binary);
    if(!out.is_open())
    {
        std::cerr << "Error: Unable to open " << file_out << " for writing" << std::endl;
        
        //  Clean if we need to
        //
        if(assembleProject){ removeTempFile(assembleVerbose); }
        return 1;
    }

    out.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
    out.close();

    // Indicate complete if they're into that
    //
    if(assembleVerbose){ std::cout << "Complete. " << bytes.size() << " bytes were generated." << std::endl; }

    //  Clean if we need to
    //
    if(assembleProject){ removeTempFile(assembleVerbose); }

    return 0;
}

// ------------------------------------------------------------
//  Load a project directory 
// ------------------------------------------------------------

void loadProject(std::string directory)
{
    //  Ensure given name is a directory
    //
    if(!std::filesystem::is_directory(std::filesystem::status(directory)))
    {
        std::cerr << "Error: \"" << directory << "\" is not a directory" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check for config.json
    //
    auto path = std::filesystem::path(directory);
    path /= CONFIG_FILE;

    std::ifstream ifs(path);
    if(!ifs.is_open()) 
    {
        std::cerr << "Error: Unable to open \"" << path.string() << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }


    // Load config
    //
    std::string entry_file;
    std::vector<std::string> project_files;
    njson::json configuration_file;
    std::vector<std::string> new_file;

    try
    {
        ifs >> configuration_file;

        configuration_file.at("entry").get_to(entry_file);
        configuration_file.at("support").get_to(project_files);
    }
    catch(...)
    {
        std::cerr << "Caught exception while loading \"" << CONFIG_FILE << "\" in \"" << directory << "\"" << std::endl;
        exit(EXIT_FAILURE);
    }

    ifs.close();

    //  Add entry file to project files and rotate to ensure its first
    //
    project_files.push_back(entry_file);
    std::rotate(project_files.rbegin(), project_files.rbegin() + 1, project_files.rend());


    // Load all project files
    //
    for(auto & file : project_files)
    {
        auto fpath = std::filesystem::path(directory); fpath /= file;

        if(!std::filesystem::is_regular_file(std::filesystem::status(fpath.string())))
        {
            std::cerr << "Error: Listed project file \"" << fpath.string() << "\" is not a regular file " << std::endl; 
            exit(EXIT_FAILURE);
        }

        std::ifstream inf(fpath);

        if(!inf.is_open())
        {
            std::cerr << "Error: Unable to open file \"" << fpath.string() << "\"" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string line;
        while(std::getline(inf, line))
        {
            if(line.size() > 0)
            {
                new_file.push_back((line + "\n"));
            }
        }

        inf.close();
    }

    //  Write temp build file
    //
    std::ofstream ofs(TEMP_FILE_NAME);

    if(!ofs.is_open())
    {
        std::cerr << "Error: Unable to open temporary file for writing project information" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(auto & line : new_file)
    {
        ofs << line;
    }

    ofs.close();
}