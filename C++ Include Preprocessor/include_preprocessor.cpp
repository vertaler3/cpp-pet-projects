#include <cassert>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}


path FileInDirector(string name, const vector<path>& include_directories){
    for(size_t i=0; i<include_directories.size();i++){
        if(filesystem::exists(include_directories[i] / name)){
            return path(include_directories[i]/name);
        }
    }
    return path{};
}

void PrintUnknownInclude(const string& include_name, const path& file, int line_number){
    cout << "unknown include file " << include_name
        << " at file " << file.string()
        << " at line " << line_number << endl;
}

bool ProcessForOneFile(const path& file, ofstream& out, const vector<path>& include_directories){
    ifstream in_file(file);
    if (!in_file){
        return false;
    }

    static regex local_include_regex(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static regex system_include_regex(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");

    string line;
    int line_number = 0;
    while (getline(in_file, line)) {
        ++line_number;
        smatch include_match;
        if (regex_match(line, include_match, local_include_regex)) {
            path found = file.parent_path() / include_match[1].str();

            if(!filesystem::exists(found)){
                found = FileInDirector(include_match[1].str(), include_directories);
                if(found.empty()){
                    PrintUnknownInclude(include_match[1].str(), file, line_number);
                    return false;
                }
            }

            if(!ProcessForOneFile(found, out, include_directories)){
                return false;
            }

        } else if (regex_match(line, include_match, system_include_regex)) {
            path found = FileInDirector(include_match[1].str(), include_directories);
            if(found.empty()){
                PrintUnknownInclude(include_match[1].str(), file, line_number);
                return false;
            }

            if(!ProcessForOneFile(found, out, include_directories)){
                return false;
            }

        } else {
            out << line << '\n';
        }
    }
    return true;
}

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream initial_file(in_file);
    if (!initial_file) {
        return false;
    }
    ofstream res_file(out_file);
    if (!res_file) {
        return false;
    }

    return ProcessForOneFile(in_file, res_file, include_directories);
}



string GetFileContents(string file) {
    ifstream stream(file);

    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p})));

    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}