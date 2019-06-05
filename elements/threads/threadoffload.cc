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
#include <rte_cycles.h>
CLICK_DECLS

ThreadOffload::ThreadOffload()
{
    assert(sizeof(ThreadOffload::Annotation) <= TCP_OFFLOAD_ANNO_SIZE);
    job_queue = 0;
    stop_signal = RTE_ATOMIC16_INIT(0);
}

ThreadOffload::~ThreadOffload()
{
    if (job_queue == 0)
    {
        printf("[ThreadOffload] No worker thread found, exiting.\n");
        return;
    }
    printf("[ThreadOffload] Push termination marker\n");
    rte_atomic16_exchange(&stop_signal, 1);

    rte_mb();

    printf("[ThreadOffload] Try join...\n");
    pthread_join(_worker_thread, 0);
    printf("[ThreadOffload] Join finished.\n");
    rte_ring_free(job_queue);
}

void *ThreadOffload::worker()
{
    printf("[ThreadOffload] Worker thread started!\n");
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(_core_id, &set);
    pthread_setaffinity_np(_worker_thread, sizeof(cpu_set_t), &set);
#define BURST_SIZE 32
    void *burst[BURST_SIZE];
    while (true)
    {
        int n = rte_ring_sc_dequeue_burst(job_queue, burst, BURST_SIZE, NULL);
        for (int i = 0; i < n; ++i)
        {
            void *ptr = burst[i];
            Packet* p = (Packet*)ptr;
            ThreadOffload::Annotation* anno = get_anno(p);
            rte_atomic16_exchange(&anno->state, 1);
            rte_wmb();
            rte_pktmbuf_release_ref2(p->mbuf());
        } 
        if (n == 0 && stop_signal == 1)
            goto break_loop;
    }
break_loop:
    printf("[ThreadOffload] Worker thread ended!\n");
}

int ThreadOffload::configure(Vector<String> &conf, ErrorHandler *errh)
{
    if (Args(conf, this, errh)
            .read_p("CORE", _core_id)
            //.read_p("INCREASING", _increasing)
            .complete() < 0)
        return -1;

    int socket_id = rte_lcore_to_socket_id(_core_id);
    job_queue = rte_ring_create("threadoffload_jq", 8192, socket_id, RING_F_SC_DEQ | RING_F_SP_ENQ);
    rte_mb();
    int ret = pthread_create(&_worker_thread, NULL, (void *(*)(void *))&ThreadOffload::worker, this);
    assert(ret == 0);

    return 0;
}

Packet *ThreadOffload::simple_action(Packet *p)
{
    DO_MICROBENCH_WITH_INTERVAL(500000);
    get_anno(p)->created_at = rte_rdtsc();
    rte_pktmbuf_acquire_ref2(p->mbuf());
    SET_TCP_HAS_OFFLOAD_ANNO(p, 1);
    rte_wmb();
    while(rte_ring_sp_enqueue(job_queue, (void*)p) <0);

    return p;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(multithread)
EXPORT_ELEMENT(ThreadOffload)
