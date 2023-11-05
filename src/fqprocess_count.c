// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "fqheader.h"

// Define the global variables:
extern fqfsin f_in;
extern fqparser_callbacks callbacks;
extern fqbytecount n_reads;

fqbytecount fqprocess_count_readBuffer(fqflag pair, char *b, fqbytecount b_size){
    return fqfile_read(&(f_in.files[pair]->file), b, b_size);
}

void fqprocess_count_endRead(fqflag pair){
    if(pair == FQ_PAIR_1) n_reads ++;
}

fqstatus fqprocess_count(int argc, const char *argv[], fqglobal options){
    int option;
    fqstatus result;
    char finished = 0;

    //Parse the subcommand options:
    optind++; // Skip the subcommand argument
    while((option = getopt(argc, (char* const*)argv, "+h")) != -1){
        switch(option){
            case 'h':{fqprocess_count_help(); return FQ_STATUS_OK;}
            default: {fqprocess_count_usage(); return FQ_STATUS_FAIL;}
        }
    }

    //Prepare the IO file sets:
    result = prepare_filesets(&f_in, NULL, argc - optind, &(argv[optind]), &callbacks, options);
    if(result != FQ_STATUS_OK){
        fprintf(stderr, "ERROR: failed to initialize IO\n");
        return FQ_STATUS_FAIL;
    }

    //Set the callbacks:
    set_generic_callbacks(&callbacks);
    callbacks.readBuffer = fqprocess_count_readBuffer;
    callbacks.endRead = fqprocess_count_endRead;

    // Step through the input fileset:
    n_reads = 0;
    do finished = fqfsin_step(&f_in);
    while(finished != 1);
    result = f_in.status;
    
    // Clean up:
    fqfsin_close(&f_in);
    if(result == FQ_STATUS_OK) printf("%lu\n", n_reads);
    return result;
}
