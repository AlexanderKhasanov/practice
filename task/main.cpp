#include "auto_logs.hpp"

int main( int argc, char** argv )
{
//    auto_logs l("/home/sj_12/practice/task/cmake-build-debug/logs/");
//    string c = "SCSI.Read6";
//    l.test_execution(c);

    try
    {
        string command = argv[2];
        //string command = "1.1.1.1.1.1.1";
        //string command = "SCSI.Read6";

        if ( argc == 3 )
        {
            auto_logs logs( argv[1] );
            logs.test_execution( command );
        }
        else if ( argc == 4 )
        {
            auto_logs logs ( argv[1], argv[3] );
            logs.test_execution( command );
        }
        else
        {
            throw std::logic_error( "Faild!\nUse [--parameter(name device] [--parameter(necessary tests)]"
                                    " [--parameter(directory for logs)]" );
//        cout<<"Use [--parameter(necessary tests)] [--parameter(directory for logs)]"<<endl;
//        return 1;
        }
    }
    catch ( exception & e )
    {
        cout << e.what()<<endl;
    }

    return 0;
}