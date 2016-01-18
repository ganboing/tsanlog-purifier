#include <cstdio>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <memory>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace std;
using namespace boost::interprocess;

static const char tsanlog_sig_b[] = "=tSaNlOg ";
static const char tsanlog_sig_e[] = "=GoLnAsT ";

static constexpr auto
        *tsanlog_bf = tsanlog_sig_b,
        *tsanlog_bl = tsanlog_sig_b + sizeof(tsanlog_sig_b) - 1,
        *tsanlog_ef = tsanlog_sig_e,
        *tsanlog_el = tsanlog_sig_e + sizeof(tsanlog_sig_e) - 1;

typedef unique_ptr<FILE, decltype(ptr_fun(fclose))> auto_fp;

auto_fp auto_fopen(const char* name, const char* mode){
    return auto_fp(fopen(name, mode), ptr_fun(fclose));
};

void split(const char* log, size_t s, const char* prefix){
    map<int, auto_fp> outs;
    const char *i = log, *j = log + s, *k;
    while((k = search(i, j, tsanlog_bf, tsanlog_bl)) != j){
        fwrite(i, sizeof(char), k-i, stdout);
        int tid = strtol(tsanlog_bl - tsanlog_bf + k, (char**)&i, 0); //potential security risk
        i += 2; //hardcode
        assert(i[-1] == '=');
        k = search(i, j, tsanlog_ef, tsanlog_el);
        assert(k != j);
        int len = k - i, //int should be sufficient
        checklen = strtol(tsanlog_el - tsanlog_ef + k, (char**)&i, 0); //potential security risk
        assert(len == checklen);
        i += 2; //hardcode
        assert(i[-1] == '=');
        if(outs.find(tid) == outs.end()){
            stringstream name;
            name << prefix << "_" << tid << ".log";
            outs.insert(make_pair(tid, auto_fopen(name.str().c_str(), "w")));
        }
        fwrite(k-len, sizeof(char), len, outs[tid].get());
    }
    fwrite(i, sizeof(char), j - i, stdout);
}

int main(int, char** argv){
    file_mapping fm(argv[1], read_only);
    mapped_region mr(fm, read_only);
    split((const char*)mr.get_address(), mr.get_size(), argv[2]);
    return 0;
}