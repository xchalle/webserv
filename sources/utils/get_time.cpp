# include "../../include/utils.hpp"

long long int get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}