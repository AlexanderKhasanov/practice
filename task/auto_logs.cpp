#include "auto_logs.hpp"

auto_logs::auto_logs( char* name_device )
    : _device (name_device)
{
    //current time detection
    chrono::system_clock::time_point now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);
    stringstream current_time;
    current_time << put_time(localtime(&time), "%F#%T#");
    string name_main_directory = current_time.str();

    string name_directory_ok = name_main_directory + "/OK";

    string name_directory_faild = name_main_directory + "/FAILD";

    string name_directory_skipped = name_main_directory + "/SKIPPED";

    //creating directories
    fs::create_directories( name_main_directory );
    fs::create_directories( name_directory_ok );
    fs::create_directories( name_directory_faild );
    fs::create_directories( name_directory_skipped );

    fs::path main_path = fs::current_path();
    main_path /= name_main_directory.c_str();
    _main_directory = main_path;
    fs::path testlist_path = main_path / "testlist.txt";

    //get a file with the names of all tests
    string testlist = "iscsi-test-cu -l > ";
    testlist += testlist_path.string();
    system( testlist.c_str() );

    //get all the tests
    get_testlist( testlist_path.string() );
}

auto_logs::auto_logs( char* name_device, char*  name_directory )
    : _device (name_device)
{
    //current time detection
    chrono::system_clock::time_point now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);
    stringstream current_time;
    current_time << put_time(localtime(&time), "%F#%T#");
    string name_main_directory = name_directory;
    name_main_directory += current_time.str();

    string name_directory_ok = name_main_directory + "/OK";

    string name_directory_faild = name_main_directory + "/FAILD";

    string name_directory_skipped = name_main_directory + "/SKIPPED";

    //creating directories
    fs::create_directories( name_main_directory );
    fs::create_directories( name_directory_ok );
    fs::create_directories( name_directory_faild );
    fs::create_directories( name_directory_skipped );

    fs::path main_path(name_main_directory);
    _main_directory = main_path;
    fs::path testlist_path = main_path / "testlist.txt";

    //get a file with the names of all tests
    string testlist = "iscsi-test-cu -l > ";
    testlist += testlist_path.string();
    system( testlist.c_str() );

    //get all the tests
    get_testlist( testlist_path.string() );
}

vector<string> auto_logs::parser_test_name (const string & test_name) const
{
    vector<string> result;
    if ( test_name.empty() )
        return result;

    char delim = '.';
    size_t prev = 0;
    size_t next = test_name.find(delim, prev);
    while ( next != string::npos )
    {
        result.push_back( test_name.substr(prev, next - prev) );
        prev = next + 1;
        next = test_name.find(delim, prev);
    }
    result.push_back( test_name.substr( prev, test_name.size() - prev ) );
    return result;
}

int auto_logs::check_category ( const string & name_file ) const
{
    /*FAILED - return 0;
     * SKIPPED - return 1;
     * SUCCESS - return 2;
     */
    ifstream file ( name_file.c_str() );
    bool test_begin = false;

    if( file.is_open() )
    {
        string str;
        while( getline( file, str ) )
        {
            if ( str.find( "FAILED" ) != string::npos )
                return 0;

            if ( str.find( "Test:" ) != string::npos )
            {
                if ( str.find ("[SKIPPED]") != string::npos )
                    return 1;
                test_begin = true;
            }

            if ( test_begin )
                if ( str.find( "passed" ) != string::npos )
                    return 2;
        }
        file.close();
    }
    return -1;
}

string auto_logs::find_test ( const string & test)
{
    vector<string> name_test = parser_test_name( test );
    if (name_test.size() != 3)
    {
        throw std::invalid_argument( "The test name should be: families.suites.tests" );
    }
    else
        return _all_test[name_test.at(0)][name_test.at(1)][name_test.at(2)];
}

void auto_logs::call_test ( const string & test ) const
{
    string name_result_file = test + ".txt";
    fs::path path_result_file = _main_directory / name_result_file;

    //call test
    //string call_string = "iscsi-test-cu /dev/sg0 --test " + test + " -V  > " + path_result_file.string();
    string call_string = "iscsi-test-cu " + _device + " --test " + test + " -V  > " + path_result_file.string();
    system( call_string.c_str() );

    int category = check_category ( path_result_file.string() ) ;
    fs::path new_path = _main_directory;
    switch ( category )
    {
        case 0:
        {
            new_path /= "FAILD";
            break;
        }
        case 1:
        {
            new_path /= "SKIPPED";
            break;
        }
        case 2:
        {
            new_path /= "OK";
            break;
        }
        default:
        {
            cout<<"Faild!\n";
            return;
        }
    }
    new_path /= name_result_file;

    //transfer file to the desired directory
    fs::rename ( path_result_file, new_path );
}

void auto_logs::get_saites_test ( const test_type & test ) const
{
    for ( auto & t : test )
        call_test( t.second );
}

void auto_logs::get_families_test ( const saites_type & test ) const
{
    for ( auto & t : test )
        get_saites_test( t.second );
}

void auto_logs::get_all_test ( const families_type  & test ) const
{
    for ( auto & t : test )
        get_families_test( t.second );
}

void auto_logs::get_testlist ( const string & name_file )
{
    string line;

    ifstream testlist( name_file.c_str() );

    if (testlist.is_open())
    {
        while( getline(testlist, line) )
        {
            vector<string> name_test = parser_test_name(line);
            if ( name_test.size() != 3 )
                continue;
            _all_test[name_test.at(0)][name_test.at(1)][name_test.at(2)] = line;
        }
        testlist.close();
    }
}

void auto_logs::test_execution( const string & command )
{
    vector<string> necessary_test = parser_test_name( command );
    switch ( necessary_test.size() )
    {
        case 0:
        {
            get_all_test( _all_test );
            break;
        }
        case 1:
        {
            get_families_test( _all_test[necessary_test.at(0)] );
            break;
        }
        case 2:
        {
            get_saites_test( _all_test[necessary_test.at(0)][necessary_test.at(1)] );
            break;
        }
        case 3:
        {
            call_test( _all_test[necessary_test.at(0)][necessary_test.at(1)][necessary_test.at(2)] );
            break;
        }
        default:
            throw std::invalid_argument( "The test name should be: families.suites.tests" );
    }
}