/*
 * fam_atl_get_put.cpp
 * Copyright (c) 2021 Hewlett Packard Enterprise Development, LP. All rights
 * reserved. Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * See https://spdx.org/licenses/BSD-3-Clause
 *
 */
#include <assert.h>
#include <chrono>
#include <fam/fam.h>
#include </home/bhatkg/GT-year-2/internal/OpenFAM/include/fam/fam_exception.h>
#include <fam_atl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>

using namespace chrono;

using namespace std;
using namespace openfam;
#define DATA_REGION "test"

/* Use OpenFAM's default options */
#if 0
#define TEST_OPENFAM_MODEL "memory_server"
#define TEST_CIS_INTERFACE_TYPE "rpc"
#define TEST_CIS_SERVER "127.0.0.1"
#define TEST_GRPC_PORT "8787"
#define TEST_LIBFABRIC_PROVIDER "sockets"
#define TEST_FAM_THREAD_MODEL "FAM_THREAD_SERIALIZE"
#define TEST_FAM_CONTEXT_MODEL "FAM_CONTEXT_DEFAULT"
#define TEST_RUNTIME "PMIX"
#endif

#ifdef MEMSERVER_PROFILE
/* #undef TEST_MEMSERVER_IP */
#endif

// Runtime configuration
uint64_t get_time() {
    long int time = static_cast<long int>(
        duration_cast<nanoseconds>(
            high_resolution_clock::now().time_since_epoch())
            .count());
    return time;
}

uint64_t time_diff_nanoseconds(uint64_t start, uint64_t end) {
    return (end - start);
}

void init_fam_options(Fam_Options *famOpts) {
  memset((void *)famOpts, 0, sizeof(Fam_Options));
/* Use OpenFAM's default options */
#if 0
  famOpts->openFamModel = strdup(TEST_OPENFAM_MODEL);
  famOpts->cisInterfaceType = strdup(TEST_CIS_INTERFACE_TYPE);
  famOpts->cisServer = strdup(TEST_CIS_SERVER);
  famOpts->grpcPort = strdup(TEST_GRPC_PORT);
  famOpts->libfabricProvider = strdup(TEST_LIBFABRIC_PROVIDER);
  famOpts->famThreadModel = strdup(TEST_FAM_THREAD_MODEL);
  famOpts->famContextModel = strdup(TEST_FAM_CONTEXT_MODEL);
  famOpts->runtime = strdup(TEST_RUNTIME);
#endif
}

int main(int argc, char **argv) {
	int iteration = atoi(argv[1]);
	int datasize = atoi(argv[2]);
  fam *my_fam = new fam();
  ATLib *myatlib = new ATLib();
  Fam_Options fam_opts;
  Fam_Region_Descriptor *dataRegion = NULL;
  Fam_Descriptor *item1 = NULL;
  char itemName[32];
  //int i;
  int *my_rank;
  //bool compflag = true;
  memset((void *)&fam_opts, 0, sizeof(Fam_Options));
  init_fam_options(&fam_opts);
  //  cout << "PID ready: gdb attach " << getpid() <<endl;
  //  sleep(30);
  //cout << " Calling fam_initialize" << endl;
  try {
    my_fam->fam_initialize("default", &fam_opts);
  } catch (Fam_Exception &e) {
    cout << "fam initialization failed" << endl;
    exit(1);
  }
  myatlib->initialize(my_fam);
  my_rank = (int *)my_fam->fam_get_option(strdup("PE_ID"));
  //try {
  //  dataRegion = my_fam->fam_lookup_region(DATA_REGION);
  //} catch (Fam_Exception &e) {
    //cout << "data Region not found" << endl;
    dataRegion = my_fam->fam_create_region(DATA_REGION, 274877906944, 0777, NULL);
 // }
  //char msg1[200] = {0};
  //char msg2[200] = {0};
  sprintf(itemName,"item1_%d",*my_rank);
 // try {
  //  item1 = my_fam->fam_lookup(itemName, DATA_REGION);
 // } catch (Fam_Exception &e) {
    //.item1 = my_fam->fam_allocate(itemName, datasize, 0777, dataRegion);
    item1 = my_fam->fam_allocate(itemName, 137438953472, 0777, dataRegion);
 // }
  //cout << "Test 1: starting complete Get & Put atomic" << endl;
  //for (i = 0; i < 200; i++)
  //  msg1[i] = 'X';
  //auto start = std::chrono::high_resolution_clock::now();
  char *local = new char[datasize];
  memset(local, 'a', datasize);
  uint64_t start_hr, end_hr, total_time=0, offset=0;
    std::srand((uint32_t)std::time(nullptr)); // Use current time as seed for random generator

    uint64_t minRange = 0; // Define minimum range
    uint64_t maxRange = 136365211648; // Define maximum range 
#if 1
  for(int i=0; i<iteration; i++) {
  start_hr = get_time();
  try {
  myatlib->fam_put_atomic(local, item1, offset, datasize);
  } catch(Fam_Exception &e) {
	  cout << "put error : " << e.fam_error_msg() << " itr no. : " << i << endl;
  }
  end_hr = get_time();
  total_time += time_diff_nanoseconds(start_hr, end_hr);
  offset = minRange + (std::rand() % (maxRange - minRange + 1));
  //offset += datasize;
  }
  //auto end = std::chrono::high_resolution_clock::now();
  //std::chrono::duration<double> elapsed_seconds = end - start;
  //cout << "put atomic elapsed time: " << time_diff_nanoseconds(start_hr, end_hr)/iteration << endl;
  cout << "put_atomic," << iteration << "," << datasize << "," << total_time/iteration << endl;
#endif
  //start = std::chrono::high_resolution_'clock::now();
#if 0
  char *local2 = new char[datasize];
  offset = 0;
 // Initialize random seed
  for(int i=0; i<iteration; i++) {
  start_hr = get_time();
  try {
  myatlib->fam_get_atomic(local2, item1, offset, datasize); // strlen(msg1));
  } catch(Fam_Exception &e) {
	            cout << "gett error : " << e.fam_error_msg() << endl;
		      }
  end_hr = get_time();
  total_time += time_diff_nanoseconds(start_hr, end_hr);
  offset = minRange + (std::rand() % (maxRange - minRange + 1));
  //offset += datasize;
  }
  //end = std::chrono::high_resolution_clock::now();
  //elapsed_seconds = end - start;
  //cout << "get atomic elapsed time: " << time_diff_nanoseconds(start_hr, end_hr)/iteration << endl;
  cout << "get_atomic," << iteration << "," << datasize << "," << total_time/iteration << endl;
#endif
#if 0
  //cout << msg2 << endl;
  if (strncmp(local, local2, datasize) == 0)
    cout << "Comparison of full string successful" << endl;
  else
    cout << "Test 1 failed: Comparison of full string failed" << endl;
#endif
#if 0
  cout << "complete atomic Get & Put completed " << endl;
  cout << "--------------------------------------" << endl;
  cout << "Test 2: starting partial Get & Put atomic with offset" << endl;
  compflag = true;
  for (i = 0; i < 200; i++)
    msg1[i] = 'Y';
  memset(msg2, 0, 200);
  auto start = std::chrono::high_resolution_clock::now();
  myatlib->fam_put_atomic((void *)msg1, item1, 20, 150);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed_seconds = end - start;
  cout << "put atomic elapsed time: " << elapsed_seconds.count() << endl;

  myatlib->fam_get_atomic((void *)msg2, item1, 20, 150);
  cout << msg2 << endl;
  if (strncmp(msg2, strchr(msg1, 'Y'), 150) == 0)
    cout << "Test 2: Comaparion of partial string successful" << endl;
  else
    cout << "Test 2:Comaparion of partial string failed" << endl;
  cout << "reading the complete data" << endl;
  memset(msg2, 0, 200);
  myatlib->fam_get_atomic((void *)msg2, item1, 0, 200);
  cout << msg2 << endl;
  for (i = 0; i < 20; i++) {
    if (msg2[i] != 'X')
      compflag = false;
  }
  for (i = 20; i < 170; i++) {
    if (msg2[i] != 'Y')
      compflag = false;
  }
  for (i = 170; i < 200; i++) {
    if (msg2[i] != 'X')
      compflag = false;
  }
  if (compflag)
    cout << "Comaparion of full string successful" << endl;
  else
    cout << "Comaparion of full string failed" << endl;
  cout << "partial atomic Get & Put completed " << endl;
  cout << "--------------------------------------" << endl;
  cout << "Test 3: starting small Get & Put atomic (data embeded in message)"
       << endl;
  compflag = true;
  memset(msg2, 0, 200);
  for (i = 0; i < 200; i++)
    msg1[i] = 'Z';
  start = std::chrono::high_resolution_clock::now();
  myatlib->fam_put_atomic((void *)msg1, item1, 50, 100);
  end = std::chrono::high_resolution_clock::now();
  elapsed_seconds = end - start;
  cout << "put atomic elapsed time: " << elapsed_seconds.count() << endl;

  myatlib->fam_get_atomic((void *)msg2, item1, 50, 100);
  cout << msg2 << endl;
  if (strncmp(msg2, strchr(msg1, 'Z'), 100) == 0)
    cout << "Comaparion of partial string successful" << endl;
  else
    cout << "Comaparion of partial string failed" << endl;

  cout << "reading the complete data" << endl;
  memset(msg2, 0, 200);
  myatlib->fam_get_atomic((void *)msg2, item1, 0, 200);
  cout << msg2 << endl;
  for (i = 0; i < 20; i++) {
    if (msg2[i] != 'X')
      compflag = false;
  }
  for (i = 20; i < 50; i++) {
    if (msg2[i] != 'Y')
      compflag = false;
  }
  for (i = 50; i < 150; i++) {
    if (msg2[i] != 'Z')
      compflag = false;
  }
  for (i = 150; i < 170; i++) {
    if (msg2[i] != 'Y')
      compflag = false;
  }
  for (i = 170; i < 200; i++) {
    if (msg2[i] != 'X')
      compflag = false;
  }
  if (compflag)
    cout << "Test3: Comaparison of full string successful" << endl;
  else
    cout << "Test3: Comaparison of full string failed" << endl;

  cout << "partial atomic Get & Put completed " << endl;

  //    my_fam->fam_destroy_region(dataRegion);
 #endif
  my_fam->fam_deallocate(item1);
      my_fam->fam_destroy_region(dataRegion);
      //my_fam->fam_service_dump_profile();
  myatlib->finalize();
  my_fam->fam_finalize("default");
  //cout << "fam finalize successful" << endl;
  //    delete myatlib;
  //    delete my_fam;
}
