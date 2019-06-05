/*
 * staticthreadsched.{cc,hh} -- element statically assigns tasks to threads
 * Eddie Kohler
 *
 * Copyright (c) 2004-2008 Regents of the University of California
 * Copyright (c) 2004-2014 Click authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */
#include <click/config.h>
#include "threadoffload.hh"
#include <click/glue.hh>
#include <click/args.hh>
#include <click/task.hh>
#include <click/error.hh>
#include <click/tcpanno.hh>
#include <pthread.h>
CLICK_DECLS

ThreadOffload::ThreadOffload()
{
    assert(sizeof(ThreadOffload::Annotation) <= TCP_OFFLOAD_ANNO_SIZE);
    job_queue = 0;
}

ThreadOffload::~ThreadOffload()
{
    if (job_queue == 0)
    {
        printf("No worker thread found, exiting.\n");
        return;
    }
    printf("Push termination marker\n");
    while(rte_ring_mp_enqueue(job_queue, 0) <0);

    printf("Try join...\n");
    pthread_join(_worker_thread, 0);
    printf("Join finished.\n");
    rte_ring_free(job_queue);
}

void *ThreadOffload::worker()
{
    printf("Worker thread started!\n");
#define BURST_SIZE 32
    void *burst[BURST_SIZE];
    while (true)
    {
        int n = rte_ring_sc_dequeue_burst(job_queue, burst, BURST_SIZE, NULL);
        for (int i = 0; i < n; ++i)
        {
            void *ptr = burst[i];
            if (ptr == 0)
            {
                assert(i + 1 == n);
                goto break_loop;
            }
        }
    }
break_loop:
    printf("Worker thread ended!\n");
}

int ThreadOffload::configure(Vector<String> &conf, ErrorHandler *errh)
{
    if (Args(conf, this, errh)
            .read_p("CORE", _core_id)
            //.read_p("INCREASING", _increasing)
            .complete() < 0)
        return -1;

    int socket_id = rte_lcore_to_socket_id(_core_id);
    job_queue = rte_ring_create("threadoffload_jq", 1024, socket_id, RING_F_SC_DEQ);
    rte_mb();
    int ret = pthread_create(&_worker_thread, NULL, reinterpret_cast<void *(*)(void *)>(&ThreadOffload::worker), this);
    assert(ret == 0);

    return 0;
}

void ThreadOffload::push(int port, Packet *p)
{
    p->kill();
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(multithread)
EXPORT_ELEMENT(ThreadOffload)
