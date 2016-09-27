#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************

                Return something for time accounting in microseconds.

                For example,

                ------------------------

    C/C++          double mrun(), t1, t2;
                   t1 = mrun();
                        ...
                   t2 = mrun() - t1;

                ------------------------

    Fortran        double precision mrun, t1, t2
                   t1 = mrun()
                        ...
                   t2 = mrun() - t1
                   
                ------------------------

                puts the difference in wall clock times in t2.  No guarantees
                are made about what mrun returns other than it will be in the
                format seconds.microseconds (assuming the machine supports
                microseconds).

 *****************************************************************************/


/* C/C++ */

double mrun() {

    double  ret;
    struct timeval tp;
    struct timezone tzp;

    gettimeofday( &tp, &tzp );

#ifdef TESTIT
    printf( "%ld %ld\n", (long)tp.tv_sec, (long)tp.tv_usec );
#endif
  
    return tp.tv_usec * 1.0e-6 + tp.tv_sec;
    }


/* Fortran */

double mrun_() {

    return mrun();
    }


#ifdef TESTIT
main() {

    printf( "%22.16le\n", mrun() );

    }
#endif
