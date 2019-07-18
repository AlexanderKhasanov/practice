#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

using test_type = map<string, string>;
using saites_type = map<string, test_type>;
using families_type = map<string, saites_type>;

class auto_logs
{
    fs::path _main_directory;
    families_type _all_test;
    string _device;

public:
    auto_logs( char* name_device );

    auto_logs( char * name_device, char* name_directory );

    ~auto_logs() = default;

    vector<string> parser_test_name (const string & test_name) const;

    int check_category ( const string & name_file ) const;

    string find_test ( const string & test);

    void call_test ( const string & test ) const;

    void get_saites_test ( const test_type & test ) const;

    void get_families_test ( const saites_type & test ) const;

    void get_all_test ( const families_type & test ) const;

    void test_execution ( const string & command);

private:
    void get_testlist ( const string & name_file );
};